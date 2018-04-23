/// @file
/// @copyright Copyright (c) 2018 United States Government as represented by
///            the Administrator of the National Aeronautics and Space
///            Administration. All Rights Reserved.

#include "UDPSocket.h"

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#define _EAFNOSUPPORT WSAEAFNOSUPPORT
#define sockerr WSAGetLastError()
#define _close closesocket
#define _ioctl ioctlsocket
#else
#define _EAFNOSUPPORT EAFNOSUPPORT
#include <sys/ioctl.h>
#define sockerr errno
#define _close close
#define _ioctl ioctl
#endif

namespace PCOE {
    /// @brief RAII wrapper around and addrinfo pointer created by getaddrinfo
    class AddressInfo {
    public:
        AddressInfo() : result(nullptr) {}
        AddressInfo(const AddressInfo&) = delete;
        AddressInfo(AddressInfo&& other) : AddressInfo() {
            std::swap(result, other.result);
        }
        AddressInfo& operator=(const AddressInfo&) = delete;
        AddressInfo& operator=(AddressInfo&& other) {
            std::swap(result, other.result);
            return *this;
        }

        AddressInfo(const char* hostname, const char* port, addrinfo* hints) : AddressInfo() {
            int status = getaddrinfo(hostname, port, hints, &result);
            if (status) {
                std::error_code ec(sockerr, std::generic_category());
                throw std::system_error(ec, gai_strerror(status));
            }
        }

        ~AddressInfo() {
            if (result != nullptr) {
                freeaddrinfo(result);
            }
        }

        operator addrinfo() {
            return *result;
        }
        addrinfo* operator&() {
            return result;
        }

    private:
        addrinfo* result;
    };

    static AddressInfo GetAddressInfo(const std::string& hostname, unsigned short port, int af) {
        // Try to resolve the given host using the address family specified in
        // the constructor. after calling getaddrinfo, result is a singly
        // linked list of zero or more valid addresses for the host.
        // Note: freeaddrinfo must be called on result before exiting
        std::string portStr = std::to_string(port);
        addrinfo hints = {};
        hints.ai_family = af;
        hints.ai_protocol = IPPROTO_UDP;
        hints.ai_socktype = SOCK_DGRAM;

        return AddressInfo(hostname.c_str(), portStr.c_str(), &hints);
    }

#ifdef _WIN32
    const UDPSocket::sock_type UDPSocket::InvalidSocket = INVALID_SOCKET;
#else
    const UDPSocket::sock_type UDPSocket::InvalidSocket = -1;
#endif

    UDPSocket::UDPSocket(int af) {
#ifdef _WIN32
        // After the first call to WSAStartup by the current application, this just increments a ref
        // count.
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "WSAStartup failed");
        }
        if (wsa.wVersion != MAKEWORD(2, 2)) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(
                ec, "Not using Winsock 2.2. How did you even get this code to run on Windows 3.1?");
        }
#endif
        CreateSocket(af);
    }

    UDPSocket::UDPSocket(int af, unsigned short port) : UDPSocket(af) {
        switch (af) {
        case AF_INET: {
            sockaddr_in si = {};
            si.sin_family = AF_INET;
            si.sin_port = htons(port);
            if (bind(sock, reinterpret_cast<sockaddr*>(&si), sizeof(si))) {
                std::error_code ec(sockerr, std::generic_category());
                throw std::system_error(ec, "Socket bind failed");
            }
            break;
        }
        case AF_INET6: {
            sockaddr_in6 si = {};
            si.sin6_family = AF_INET6;
            si.sin6_port = htons(port);
            if (bind(sock, reinterpret_cast<sockaddr*>(&si), sizeof(si))) {
                std::error_code ec(sockerr, std::generic_category());
                throw std::system_error(ec, "Socket bind failed");
            }
            break;
        }
        default:
            std::string what = "Address family " + std::to_string(af) +
                               " not supported. Please use the sockaddr constructor for families "
                               "other than AF_INET and AF_INET6.";
            throw std::invalid_argument(what);
        }
    }

    UDPSocket::UDPSocket(sockaddr* addr, socklen_t addrlen) : UDPSocket(addr->sa_family) {
        if (bind(sock, addr, addrlen)) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Socket bind failed");
        }
    }

    UDPSocket::UDPSocket(const std::string& hostname, unsigned short port) : UDPSocket() {
        Connect(hostname, port);
    }

    UDPSocket::UDPSocket(UDPSocket&& other) : sock(other.sock), family(other.family) {
        other.sock = InvalidSocket;
    }

    UDPSocket& UDPSocket::operator=(UDPSocket&& other) {
        Close();
        sock = other.sock;
        family = other.family;
        other.sock = InvalidSocket;
        return *this;
    }

    UDPSocket::~UDPSocket() {
        Close();
#ifdef _WIN32
        // This just decrements a ref count unless this is the last object using WSA.
        WSACleanup();
#endif
    }

    UDPSocket::size_type UDPSocket::Available() {
        size_type result;
        int status;
#ifdef _WIN32
        u_long argp;
        status = ioctlsocket(sock, FIONREAD, &argp);
        result = static_cast<size_type>(argp);
#else
        int argp;
        status = ioctl(sock, FIONREAD, &argp);
        result = static_cast<size_type>(argp);
#endif
        if (status != 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Get available failed");
        }
        return result;
    }

    void UDPSocket::Close() noexcept {
        if (sock == InvalidSocket) {
            return;
        }
        // close/closesocket can produce errors, but there isn't anything we
        // can really do about them and we are done with the socket anyway, so
        // ignore them.
        _close(sock);
        sock = InvalidSocket;
    }

    void UDPSocket::Connect(const std::string& hostname, unsigned short port) {
        AddressInfo result = GetAddressInfo(hostname, port, family);

        // Try to connect to all returned addresses in turn.
        addrinfo* ai;
        for (ai = &result; ai != nullptr; ai = ai->ai_next) {
            if (connect(sock, ai->ai_addr, ai->ai_addrlen) == 0) {
                break;
            }
        }

        if (ai == nullptr) {
            // Failed to connect to all addresses
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Socket connect failed");
        }
    }

    void UDPSocket::Connect(const sockaddr* addr, size_type addrlen) {
        if (connect(sock, addr, addrlen)) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Socket connect failed");
        }
    }

    UDPSocket::size_type UDPSocket::Receive(char buffer[], size_type len) const {
        return Receive(buffer, len, nullptr, nullptr);
    }

    UDPSocket::size_type UDPSocket::Receive(char buffer[],
                                            size_type len,
                                            sockaddr* remoteAddr,
                                            socklen_t* addrLen) const {
        ssize_type status = recvfrom(sock, buffer, len, 0, remoteAddr, addrLen);
        if (status < 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Receive failed");
        }
        return static_cast<size_type>(status);
    }

    UDPSocket::size_type UDPSocket::Send(const char buffer[], size_type len) const {
        ssize_type result = send(sock, buffer, len, 0);
        if (result < 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Send operation failed");
        }
        return static_cast<size_type>(result);
    }

    UDPSocket::size_type UDPSocket::Send(const char buffer[],
                                         size_type len,
                                         const std::string& hostname,
                                         unsigned short port) const {
        AddressInfo addresses = GetAddressInfo(hostname, port, family);

        // Try to send on all returned addresses in turn, stopping once we
        // succeed.
        addrinfo* ai;
        for (ai = &addresses; ai != nullptr; ai = ai->ai_next) {
            ssize_type result = sendto(sock, buffer, len, 0, ai->ai_addr, ai->ai_addrlen);
            if (result > 0) {
                return static_cast<size_type>(result);
            }
        }

        // Failed to send on all addresses
        std::error_code ec(sockerr, std::generic_category());
        throw std::system_error(ec, "Socket connect failed");
    }

    UDPSocket::size_type UDPSocket::Send(const char buffer[],
                                         size_type len,
                                         sockaddr* address,
                                         socklen_t addrLen) const {
        ssize_type result = sendto(sock, buffer, len, 0, address, addrLen);
        if (result < 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Send operation failed");
        }
        return static_cast<size_type>(result);
    }

    void UDPSocket::CreateSocket(int af) {
        sock = socket(af, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == InvalidSocket) {
            int err = sockerr;
            if (err == _EAFNOSUPPORT) {
                throw std::invalid_argument("Address family not supported.");
            }
            else {
                std::error_code ec(err, std::generic_category());
                throw std::system_error(ec, "Socket creation failed");
            }
        }
        family = af;
    }
}
