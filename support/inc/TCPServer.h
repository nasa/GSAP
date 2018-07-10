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
#include "TCPSocket.h"

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
    class TCPServer {
    private:
        using ssize_type = ssize_t;

    public:
        using size_type = size_t;
        using sock_type = int;
        using timeout_type = struct timeval;

        /// @brief Initializes a new instance of the @see{TCPServer} class.
        ///        This constructor is equivalent to @code{TCPServer(AF_UNSPEC)}.
        TCPServer() : TCPServer(AF_UNSPEC, "0.0.0.0", 0) {}

        /// @brief Initializes a new instance of the @see{TCPServer} class and
        ///        connects to the specified port on the specified host.
        ///
        /// @param hostname The name of the default remote host to which the
        ///                 socket will connect.
        /// @param port     The port number of the remote host to which the
        ///                 socket will connect.
        /// @exception std::system_error An error occurred when attempting to
        ///            create or connect the socket.

        TCPServer(int af, const std::string& hostname, const unsigned short port);

        /// @brief Deletes the @see{TCPServer} copy constructor. The underlying
        ///        socket is automatically closed when the instance is
        ///        destroyed, so it can't be shared between instances.
        TCPServer(const TCPServer&) = delete;

        /// @brief Moves the underlying socket of @p{other} to the current
        ///        instance.
        ///
        /// @param other Another @see{TCPServer} instance.
        TCPServer(TCPServer&& other);

        /// @brief Closes the underlying TCP connection and releases the
        ///        resources used by the @see{TCPServer}.
        ~TCPServer() noexcept;

        /// @brief Deletes the @see{TCPServer} copy assignment operator. The
        ///        underlying socket is automatically closed when the instance
        ///        is destroyed, so it can't be shared between instances.
        TCPServer& operator=(const TCPServer&) = delete;

        /// @brief Moves the underlying socket of @p{other} to the current
        ///        instance.
        ///
        /// @param other Another @see{TCPServer} instance.
        TCPServer& operator=(TCPServer&& other);

        /// @brief   Gets the address family of the socket.
        /// @remarks The address family is either the family specified when
        ///          the TCPSocketServer was constructed, or the address family used
        ///          to open the connection if AF_UNSPEC was set at
        ///          construction and the socket has subsequently been opened.
        inline int AddressFamily() {
            return family;
        }

        /// @brief Closes the underlying TCP connection and releases the
        ///        resources used by the @see{TCPServer}.
        void Close();

        /// @brief Listens for a client connection.
        ///
        /// @param backlog Backlog for connections waiting to be accepted
        void Listen(const int backlog = SOMAXCONN);

        /// @brief Accepts a client attempting to connect to this @see{TCPServer}.
        /// @return the accepted socket as a TCPSocket object
        TCPSocket Accept();

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

        unsigned int getPort() const;

    private:
//        void CreateServer(int af);
        void CreateServer(int af, const std::string hostname, const unsigned short port);
        sock_type sock;
        int family;
    };
}

#endif
