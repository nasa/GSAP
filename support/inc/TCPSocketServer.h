/// @class      TCPSocketServer
/// @brief      A wrapper around a TCP socket for servers
/// @details    The TCPSocketServer class provides methods for creating
///             sockets meant to be servers, sending, and receiving
///             stream data over a network.
/// @remarks    This class is meant to be used in conjunction with the
///             TCPSocket class. The TCPSocket class is meant for creating
///             client sockets. This class assumes Linux.
///
/// @author     Julian Vu <julian.vu@sjsu.edu>
/// @version    0.1.0
/// @date       2018-03-27
///
/// @copyright  Copyright (c) 2018 United States Government as represented by
///             the Administrator of the National Aeronautics and Space
///             Administration. All Rights Reserved.

#ifndef GSAP_TCPSOCKETSERVER_H
#define GSAP_TCPSOCKETSERVER_H

#include <map>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <system_error>

#include <sys/types.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using ssize_t = intptr_t;
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace PCOE {
    class TCPSocketServer {
    private:
        using ssize_type = ssize_t;

    public:
        using size_type = size_t;
        using sock_type = int;
        using timeout_type = struct timeval;

        /// @brief Initializes a new instance of the @see{TCPSocketServer} class.
        ///        This constructor is equivalent to @code{TCPSocketServer(AF_UNSPEC)}.
        TCPSocketServer() : TCPSocketServer(AF_UNSPEC) {}

        /// @brief   Initializes a new instance of the @see{TCPSocketServer} class
        ///          with the specified address family.
        /// @remarks The most common values for @p{af} are AF_INET for IPV4
        ///          and AF_INET6 for IPV6. If AF_UNSPEC is specified,
        ///          the socket is eventually created with either AF_INET or
        ///          AF_INET6 based on the result of a call to getaddrinfo.
        ///
        /// @param af The address family of the IP protocol.
        /// @exception std::invalid_argument The specified address family is
        ///            not supported.
        /// @exception std::system_error An error occurred when attempting to
        ///            create the socket.
        explicit TCPSocketServer(int af);

        /// @brief Initializes a new instance of the @see{TCPSocketServer} class and
        ///        connects to the specified port on the specified host.
        ///
        /// @param hostname The name of the default remote host to which the
        ///                 socket will connect.
        /// @param port     The port number of the remote host to which the
        ///                 socket will connect.
        /// @exception std::system_error An error occurred when attempting to
        ///            create or connect the socket.

        TCPSocketServer(int af, const std::string& hostname, const unsigned short port);

        /// @brief Deletes the @see{TCPSocketServer} copy constructor. The underlying
        ///        socket is automatically closed when the instance is
        ///        destroyed, so it can't be shared between instances.
        TCPSocketServer(const TCPSocketServer&) = delete;

        /// @brief Moves the underlying socket of @p{other} to the current
        ///        instance.
        ///
        /// @param other Another @see{TCPSocketServer} instance.
        TCPSocketServer(TCPSocketServer&& other);

        /// @brief Closes the underlying TCP connection and releases the
        ///        resources used by the @see{TCPSocketServer}.
        ~TCPSocketServer() noexcept;

        /// @brief Deletes the @see{TCPSocketServer} copy assignment operator. The
        ///        underlying socket is automatically closed when the instance
        ///        is destroyed, so it can't be shared between instances.
        TCPSocketServer& operator=(const TCPSocketServer&) = delete;

        /// @brief Moves the underlying socket of @p{other} to the current
        ///        instance.
        ///
        /// @param other Another @see{TCPSocketServer} instance.
        TCPSocketServer& operator=(TCPSocketServer&& other);

        /// @brief   Gets the address family of the socket.
        /// @remarks The address family is either the family specified when
        ///          the TCPSocketServer was constructed, or the address family used
        ///          to open the connection if AF_UNSPEC was set at
        ///          construction and the socket has subsequently been opened.
        inline int AddressFamily() {
            return family;
        }

        /// @brief Closes the underlying TCP connection and releases the
        ///        resources used by the @see{TCPSocketServer}.
        void Close();

        /// @brief Closes underlying TCP connection of connected client and releases
        ///        the resources used by the @see{TCPSocket}.
        ///
        /// @param socketKey The key corresponding to a connected client
        void Close(int socketKey);

        /// @brief Closes all the underlying TCP connections of clients and releases
        ///        the resources used by @see{TCPSocket}.
        void CloseAll();

        /// @brief Listens for a client connection.
        ///
        /// @param backlog Backlog for connections waiting to be accepted
        void Listen(const int backlog = SOMAXCONN);

        /// @brief Accepts a client attempting to connect to this @see{TCPSocketServer}.
        /// @return the accepted socket's fd
        sock_type Accept();

        /// @brief Sends the given data.
        ///
        /// @param socketKey    The key corresponding to a connected client
        /// @param buffer       A pointer to the data to send
        /// @param len          The number of bytes to send
        /// @return             The number of bytes sent
        size_type Send(int socketKey, const char buffer[], size_type len);

        /// @brief Sends the given data to all connected clients
        /// @param buffer   A pointer to the data to send
        /// @param len      The number of bytes to send
        /// @return         The number of bytes sent
        size_type SendAll(const char buffer[], size_type len);

        /// Receives the specified amount of data from the last connected client.
        /// @param buffer   A buffer to hold the data received
        /// @param len      The number of bytes to read
        /// @return         The number of bytes read
        size_type Receive(char buffer[], const size_type len);

        /// Receives the specified amount of data from a specified client.
        /// @param socketKey    The key corresponding to a connected client
        /// @param buffer       A buffer to hold the data received
        /// @param len          The number of bytes to read
        /// @return             The number of bytes read
        size_type Receive(int socketKey, char buffer[], const size_type len);

        /// @brief Gets the underlying socket.
        inline sock_type Socket() noexcept {
            return sock;
        }

        /// @brief Sets the underlying socket.
        inline void Socket(sock_type s) {
            sock = s;
            family = AF_UNSPEC;
        }

        const static sock_type InvalidSocket;

    private:
        void CreateServer(int af);
        void CreateServer(int af, const std::string& hostname, const unsigned short port);
        int clientKeys;
        std::map<int, sock_type> mapOfClients;
        sock_type sock;
        int family;
    };
}

#endif
