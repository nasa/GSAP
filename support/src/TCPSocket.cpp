/// @file
/// @copyright Copyright (c) 2018 United States Government as represented by
///            the Administrator of the National Aeronautics and Space
///            Administration. All Rights Reserved.

#include <cstdio>
#include <cstring>

#include "TCPSocket.h"

#ifdef _WIN32
#define _EAFNOSUPPORT WSAEAFNOSUPPORT
#define sockerr WSAGetLastError()
#define _close closesocket
#define _ioctl ioctlsocket
#else
#define _EAFNOSUPPORT EAFNOSUPPORT
#include <netinet/tcp.h>
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
        AddressInfo& operator                      =(AddressInfo&& other) {
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
        addrinfo hints      = {};
        hints.ai_family     = af;
        hints.ai_protocol   = IPPROTO_TCP;
        hints.ai_socktype   = SOCK_STREAM;

        return AddressInfo(hostname.c_str(), portStr.c_str(), &hints);
    }

#ifdef _WIN32
    const TCPSocket::sock_type TCPSocket::InvalidSocket = INVALID_SOCKET;
#else
    const TCPSocket::sock_type TCPSocket::InvalidSocket = -1;
#endif

    TCPSocket::TCPSocket(int af) : sock(InvalidSocket), family(af) {
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
        if (af != AF_UNSPEC) {
            CreateSocket(af);
        }
    }

    TCPSocket::TCPSocket(const std::string& hostname, const unsigned short port) : TCPSocket() {
        Connect(hostname, port);
    }

    TCPSocket::TCPSocket(TCPSocket&& other) : sock(other.sock), family(other.family) {
        other.sock = InvalidSocket;
    }

    TCPSocket::~TCPSocket() noexcept {
        Close();
#ifdef _WIN32
        // This just decrements a ref count unless this is the last object using WSA.
        WSACleanup();
#endif
    }

    TCPSocket& TCPSocket::operator=(TCPSocket&& other) {
        Close();
        sock       = other.sock;
        family     = other.family;
        other.sock = InvalidSocket;
        return *this;
    }

    TCPSocket::size_type TCPSocket::Available() {
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

    void TCPSocket::Close() noexcept {
        if (sock == InvalidSocket) {
            return;
        }
        // close/closesocket can produce errors, but there isn't anything we
        // can really do about them and we are done with the socket anyway, so
        // ignore them.
        _close(sock);
        sock = InvalidSocket;
    }

    void TCPSocket::Connect(const std::string& hostname, const unsigned short port) {
        AddressInfo result = GetAddressInfo(hostname, port, family);

        // Try to connect to all returned addresses in turn. Recreate the
        // socket for each address in case multiple address families are
        // returned.
        addrinfo* ai;
        bool unspec = sock == InvalidSocket;
        for (ai = &result; ai != nullptr; ai = ai->ai_next) {
            if (TryConnect(ai->ai_addr, ai->ai_addrlen, ai->ai_family)) {
                family = ai->ai_family;
                break;
            }
            else if (unspec) {
                Close();
            }
        }

        if (ai == nullptr) {
            // Failed to connect to all addresses
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Socket connect failed");
        }
    }

    void TCPSocket::Connect(const sockaddr* address, std::size_t len, int af) {
        if (!TryConnect(address, len, af)) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Socket connect failed");
        }
    }

    bool TCPSocket::NoDelay() {
        int value     = 0;
        socklen_t len = sizeof(value);
        int result =
            getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&value), &len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Get NoDelay failed");
        }
        return value != 0;
    }

    void TCPSocket::NoDelay(bool value) {
        int nval      = value ? 1 : 0;
        socklen_t len = sizeof(nval);
        int result =
            setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&nval), len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Set NoDelay failed");
        }
    }

    TCPSocket::size_type TCPSocket::ReceiveBufferSize() {
        size_type value = 0;
        socklen_t len   = sizeof(value);
        int result = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&value), &len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Get receive buffer size failed");
        }
        return value;
    }

    void TCPSocket::ReceiveBufferSize(size_type value) {
        socklen_t len = sizeof(value);
        int result = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&value), len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Set receive buffer size failed");
        }
    }

    TCPSocket::timeout_type TCPSocket::ReceiveTimeout() {
        timeout_type value;
        socklen_t len = sizeof(value);
        int result =
            getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&value), &len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Get receive tiemout failed");
        }
        return value;
    }

    void TCPSocket::ReceiveTimeout(timeout_type value) {
        socklen_t len = sizeof(value);
        int result =
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&value), len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Set receive timeout failed");
        }
    }

    void TCPSocket::ReceiveTimeout(double value) {
#ifdef _WIN32
        timeout_type to = static_cast<DWORD>(value * 1e3);
#else
        timeout_type to;
        to.tv_sec  = static_cast<long int>(value);
        to.tv_usec = static_cast<long int>((value - to.tv_sec) * 1e6);
#endif
        ReceiveTimeout(to);
    }

    TCPSocket::size_type TCPSocket::SendBufferSize() {
        size_type value = 0;
        socklen_t len   = sizeof(value);
        int result = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&value), &len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Get send buffer size failed");
        }
        return value;
    }

    void TCPSocket::SendBufferSize(size_type value) {
        socklen_t len = sizeof(value);
        int result = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&value), len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Set send buffer size failed");
        }
    }

    TCPSocket::timeout_type TCPSocket::SendTimeout() {
        timeout_type value;
        socklen_t len = sizeof(value);
        int result =
            getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&value), &len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Get send tiemout failed");
        }
        return value;
    }

    void TCPSocket::SendTimeout(timeout_type value) {
        socklen_t len = sizeof(value);
        int result =
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&value), len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Set send timeout failed");
        }
    }

    void TCPSocket::SendTimeout(double value) {
#ifdef _WIN32
        timeout_type to = (long int)value * 1e3;
#else
        timeout_type to;
        to.tv_sec  = static_cast<long int>(value);
        to.tv_usec = static_cast<long int>((value - to.tv_sec) * 1e6);
#endif
        socklen_t len = sizeof(to);

        int result = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&to), len);
        if (result == -1) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Set send timeout failed");
        }
    }

    TCPSocket::size_type TCPSocket::Send(const char buffer[], const size_type len) {
        ssize_type result = send(sock, buffer, len, 0);
        if (result < 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Send operation failed");
        }
        return static_cast<size_type>(result);
    }

    TCPSocket::size_type TCPSocket::Receive(char buffer[], const size_type len) {
        ssize_type result = recv(sock, buffer, len, 0);
        if (result < 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Read operation failed");
        }
        return static_cast<size_type>(result);
    }

    void TCPSocket::CreateSocket(int af) {
        sock = socket(af, SOCK_STREAM, IPPROTO_TCP);
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

    bool TCPSocket::TryConnect(const sockaddr* address, size_type len, int af) {
        if (sock == InvalidSocket) {
            CreateSocket(af);
        }
        return connect(sock, address, len) == 0;
    }
}
