/// @file
/// @copyright Copyright (c) 2018 United States Government as represented by
///            the Administrator of the National Aeronautics and Space
///            Administration. All Rights Reserved.

#include "TCPServer.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <thread>

#ifdef _WIN32
#define _EAFNOSUPPORT WSAEAFNOSUPPORT
#define sockerr WSAGetLastError()
#define _close closesocket
#define _ioctl ioctlsocket
#else
#define _EAFNOSUPPORT EAFNOSUPPORT

#include <TCPSocket.h>
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
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_socktype = SOCK_STREAM;

        return AddressInfo(hostname.c_str(), portStr.c_str(), &hints);
    }

    TCPServer::TCPServer(int af, const unsigned short port) : TCPServer(af, "0.0.0.0", port) {}

    TCPServer::TCPServer(int af, const std::string& hostname, const unsigned short port) {
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
            CreateServer(af, hostname, port);
        }
    }

    TCPServer::TCPServer(TCPServer&& other) : sock(other.sock), family(other.family) {
        other.sock = TCPSocket::InvalidSocket;
        other.family = AF_UNSPEC;
    }

    TCPServer::~TCPServer() noexcept {
        Close();
#ifdef _WIN32
        // This just decrements a ref count unless this is the last object using WSA.
        WSACleanup();
#endif
    }

    TCPServer& TCPServer::operator=(TCPServer&& other) {
        Close();
        sock = other.sock;
        family = other.family;
        other.sock = TCPSocket::InvalidSocket;
        other.family = AF_UNSPEC;
        return *this;
    }

    void TCPServer::Close() {
        _close(sock);
        sock = TCPSocket::InvalidSocket;
    }

    void TCPServer::Listen(const int backlog) {
        if (listen(sock, backlog) < 0) {
            int err = sockerr;
            std::error_code ec(err, std::generic_category());
            throw std::system_error(ec, "Listen failed.");
        }
    }

    TCPSocket TCPServer::Accept() {
        socklen_t size = sizeof(struct sockaddr_in);
        struct sockaddr_in their_addr;
        sock_type socketToAccept =
            accept(sock, reinterpret_cast<struct sockaddr*>(&their_addr), &size);
        if (socketToAccept == -1) {
            int err = sockerr;
            std::error_code ec(err, std::generic_category());
            throw std::system_error(ec, "Accept failed.");
        }
        TCPSocket socketObject;
        socketObject.sock = socketToAccept;
        return socketObject;
    }

    void TCPServer::CreateServer(int af, const std::string hostname, const unsigned short port) {
        if ((sock = socket(af, SOCK_STREAM, IPPROTO_TCP)) == TCPSocket::InvalidSocket) {
            int err = sockerr;
            if (err == _EAFNOSUPPORT) {
                throw std::invalid_argument("Address family not supported.");
            }
            else {
                std::error_code ec(err, std::generic_category());
                throw std::system_error(ec, "Socket creation failed--socket.");
            }
        }

        family = af;

        size_type value = 1;
        if (setsockopt(sock,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       reinterpret_cast<char*>(&value),
                       sizeof(int)) == -1) {
            int err = sockerr;
            std::error_code ec(err, std::generic_category());
            throw std::system_error(ec, "Socket creation failed--setsockopt.");
        }

        AddressInfo result = GetAddressInfo(hostname, port, family);
        addrinfo* ai = &result;
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) == -1) {
            int err = sockerr;
            std::error_code ec(err, std::generic_category());
            throw std::system_error(ec, "Socket creation failed--bind.");
        }
    }
}
