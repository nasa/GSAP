/// @file
/// @copyright Copyright (c) 2018 United States Government as represented by
///            the Administrator of the National Aeronautics and Space
///            Administration. All Rights Reserved.

#include "TCPSocketServer.h"
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
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#define sockerr errno
#define _close close
#define _ioctl ioctl
#endif

#define SERVER_PORT 8080

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

    const TCPSocketServer::sock_type TCPSocketServer::InvalidSocket = -1;

    TCPSocketServer::TCPSocketServer(int af) : sock(InvalidSocket), family(af) {
        if (af != AF_UNSPEC) {
            CreateServer(af);
        }
    }

    TCPSocketServer::TCPSocketServer(int af,
                                     const std::string& hostname,
                                     const unsigned short port) {
        if (af != AF_UNSPEC) {
            CreateServer(af, hostname, port);
        }
    }

    TCPSocketServer::TCPSocketServer(TCPSocketServer&& other)
        : sock(other.sock), family(other.family) {
        other.sock = InvalidSocket;
        other.family = AF_UNSPEC;
    }

    TCPSocketServer::~TCPSocketServer() noexcept {
        CloseAll();
        Close();
    }

    TCPSocketServer& TCPSocketServer::operator=(TCPSocketServer&& other) {
        Close();
        sock = other.sock;
        family = other.family;
        other.sock = InvalidSocket;
        other.family = AF_UNSPEC;
        return *this;
    }

    void TCPSocketServer::Close() {
        _close(sock);
        sock = InvalidSocket;
        CloseAll();
    }

    void TCPSocketServer::Close(int socketKey) {
        _close(mapOfClients.at(socketKey));
        mapOfClients.at(socketKey) = InvalidSocket;
        mapOfClients.erase(socketKey);
    }

    void TCPSocketServer::CloseAll() {
        for (auto& s : mapOfClients) {
            _close(s.second);
            s.second = InvalidSocket;
        }

        mapOfClients.clear();
    }

    void TCPSocketServer::Listen(const int backlog) {
        if (listen(sock, backlog) < 0) {
            int err = sockerr;
            std::error_code ec(err, std::generic_category());
            throw std::system_error(ec, "Listen failed.");
        }
    }

    TCPSocketServer::sock_type TCPSocketServer::Accept() {
        socklen_t size = sizeof(struct sockaddr_in);
        struct sockaddr_in their_addr;
        sock_type socketToAccept = accept(sock, (struct sockaddr*)&their_addr, &size);
        mapOfClients.emplace(clientKeys, socketToAccept);
        if (mapOfClients.at(clientKeys) == -1) {
            int err = sockerr;
            std::error_code ec(err, std::generic_category());
            throw std::system_error(ec, "Accept failed.");
        }
        ++clientKeys;
        return socketToAccept;
    }

    TCPSocketServer::size_type TCPSocketServer::Send(int socketKey,
                                                     const char* buffer,
                                                     TCPSocketServer::size_type len) {
        sock_type socketToSend = mapOfClients.at(socketKey);
        ssize_type result = send(socketToSend, buffer, len, 0);
        if (result < 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Send operation failed");
        }
        return static_cast<size_type>(result);
    }

    TCPSocketServer::size_type TCPSocketServer::SendAll(const char* buffer,
                                                        TCPSocketServer::size_type len) {
        for (auto& sock : mapOfClients) {
            Send(sock.first, buffer, len);
        }
    }

    TCPSocketServer::size_type TCPSocketServer::Receive(char* buffer,
                                                        const TCPSocketServer::size_type len) {
        ssize_type result = recv(mapOfClients.at(mapOfClients.size() - 1), buffer, len, 0);
        if (result < 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Read operation failed");
        }
        return static_cast<size_type>(result);
    }

    TCPSocketServer::size_type TCPSocketServer::Receive(int socketKey,
                                                        char* buffer,
                                                        const TCPSocketServer::size_type len) {
        ssize_type result = recv(mapOfClients.at(socketKey), buffer, len, 0);
        if (result < 0) {
            std::error_code ec(sockerr, std::generic_category());
            throw std::system_error(ec, "Read operation failed");
        }
        return static_cast<size_type>(result);
    }

    void TCPSocketServer::CreateServer(int af) {
        if ((sock = socket(af, SOCK_STREAM, IPPROTO_TCP)) == InvalidSocket) {
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
        if (setsockopt(
                sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&value), sizeof(int)) ==
            -1) {
            int err = sockerr;
            std::error_code ec(err, std::generic_category());
            throw std::system_error(ec, "Socket creation failed--setsockopt.");
        }

        AddressInfo result = GetAddressInfo("127.0.0.1", SERVER_PORT, family);
        addrinfo* ai = &result;
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) == -1) {
            int err = sockerr;
            std::error_code ec(err, std::generic_category());
            throw std::system_error(ec, "Socket creation failed--bind.");
        }

        clientKeys = 0;
    }

    void TCPSocketServer::CreateServer(int af,
                                       const std::string& hostname,
                                       const unsigned short port) {
        if ((sock = socket(af, SOCK_STREAM, IPPROTO_TCP)) == InvalidSocket) {
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
        if (setsockopt(
                sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&value), sizeof(int)) ==
            -1) {
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

        clientKeys = 0;
    }
}
