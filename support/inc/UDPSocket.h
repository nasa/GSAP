/// @class     UDPSocket
/// @brief     A wrapper around a UCP socket.
/// @details   The UDPSocket class provides simple methods for  sending and
// /           receiving datagrams over a network.
///
/// @author    Jason Watkins <jason-watkins@outlook.com>
/// @author    Chris Teubert <christopher.a.teubert@nasa.gov>
/// @version   0.1.0
/// @date      Intial Version: 2015-04-10
/// @date        Last Updated: 2016-08-11
///
/// @copyright Copyright (c) 2013-2016 United States Government as represented
///            by the Administrator of the National Aeronautics and Space
///            Administration. All Rights Reserved.

#ifndef PCOE_UDPSOCKET_H
#define PCOE_UDPSOCKET_H

#include <stdexcept>
#include <string>
#include <system_error>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#elif (__APPLE__ || __linux)
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif


namespace PCOE
{
    class UDPSocket
    {
    private:
#ifdef _WIN32
        using ssize_type = int;
#else
        using ssize_type = ssize_t;
#endif
    public:
#ifdef _WIN32
        using size_type = int;
        using sock_type = SOCKET;
        using timeout_type = DWORD;
#else
        using size_type = size_t;
        using sock_type = int;
        using timeout_type = struct timeval;
#endif
        /// @brief Initializes a new instance of the @see{UDPSocket} class.
        ///        This constructor is equivalent to @code{UDPSocket(AF_INET)}.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to create the socket.
        UDPSocket() : UDPSocket(AF_INET) { }

        /// @brief   Initializes a new instance of the @see{UDPSocket} class
        ///          with the specified address family.
        ///
        /// @param af The address family of the IP protocol.
        /// @exception std::invalid_argument The specified address family is
        ///            not supported.
        /// @exception std::system_error An error occurred when attempting to
        ///            create the socket.
        explicit UDPSocket(int af);

        /// @brief   Initializes a new instance of the @see{UDPSocket} class
        ///          with the specified address family and binds it to the
        ///          local port number provided. Only the AF_INET and AF_INET6
        ///          families are supported by this constructor. for other
        ///          families, use the sockaddr constructor.
        ///
        /// @param af   The address family of the IP protocol.
        /// @param port The local port number that the socket will receive data
        ///             on.
        /// @exception std::invalid_argument The specified address family is
        ///            not supported.
        /// @exception std::system_error An error occurred when attempting to
        ///            create or bind the socket.
        UDPSocket(int af, unsigned short port);

        /// @brief   Initializes a new instance of the @see{UDPSocket} class
        ///          and binds it according to the sockaddr structure provided.
        ///
        /// @param addr    A pointer to a structure that contains information
        ///                for binding the local end of the socket.
        /// @param addrlen The size of @p{local}
        /// @exception std::invalid_argument The specified address family is
        ///            not supported.
        /// @exception std::system_error An error occurred when attempting to
        ///            create or bind the socket.
        UDPSocket(sockaddr* addr, socklen_t addrlen);

        /// @brief   Initializes a new instance of the @see{UDPSocket} class
        ///          and establishes a default remote host.
        /// @remarks This constructor initializes a new UDPSocket and
        ///          establishes a remote host using the @p{hostname} and
        ///          @p{port} parameters. Establishing a default remote host
        ///          is optional. If you use this constructor, you do not have
        ///          to specify a remote host in each call to the @see{Send}
        ///          method. Specifying a default remote host limits you to
        ///          that host only. You can change the default remote host at
        ///          any time by calling the @see{Connect} method. If you want
        ///          to specify a remote host in your call to the Send method,
        ///          do not use this constructor.
        ///
        /// @param hostname The name of the default remote host to which the
        ///                 socket will connect.
        /// @param port     The port number of the remote host to which the
        ///                 socket will connect.
        /// @exception std::system_error An error occurred when attempting to
        ///            create or connect the socket.
        UDPSocket(const std::string& hostname, unsigned short port);

        /// @brief Deletes the @see{UDPSocket} copy constructor. The underlying
        ///        socket is automatically closed when the instance is
        ///        destroyed, so it can't be shared between instances.
        UDPSocket(const UDPSocket&) = delete;

        /// @brief Moves the underlying socket of @p{other} to the current
        ///        instance.
        ///
        /// @param other Another @see{UDPSocket} instance.
        UDPSocket(UDPSocket&& other);

        /// @brief Closes the underlying TCP connection and releases the
        ///        resources used by the @see{TCPSocket}.
        ~UDPSocket() noexcept;

        /// @brief Deletes the @see{TCPSocket} copy assignment operator. The
        ///        underlying socket is automatically closed when the instance
        ///        is destroyed, so it can't be shared between instances.
        UDPSocket& operator=(const UDPSocket&) = delete;

        /// @brief Moves the underlying socket of @p{other} to the current
        ///        instance.
        ///
        /// @param other Another @see{UDPSocket} instance.
        UDPSocket& operator=(UDPSocket&& other);

        /// @brief   Gets the address family of the socket.
        /// @remarks The address family is either the family specified when
        ///          the TCPSocket was constructed, or the address family used
        ///          to open the connection if AF_UNSPEC was set at
        ///          construction and the socket has subsequently been opened.
        inline int AddressFamily() { return family; }

        /// @brief Gets the amount of data that has been received from the
        ///        network and is available to be read.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        size_type Available();

        /// @brief Closes the underlying TCP connection and releases the
        ///        resources used by the @see{TCPSocket}.
        void Close() noexcept;

        /// @brief   Establishes a default remote host.
        ///
        /// @param hostname The name of the default remote host to which the
        ///                 socket will connect.
        /// @param port     The port number of the remote host to which the
        ///                 socket will connect.
        /// @exception std::system_error An error occurred when attempting to
        ///            connect the socket.
        void Connect(const std::string& hostname, unsigned short port);

        /// @brief   Establishes a default remote host.
        ///
        /// @param addr    A pointer to a sockaddr struct containing the
        ///                destination host's information.
        /// @param addrlen The size of the structure pointed to by @p{address}.
        /// @exception std::system_error An error occurred when attempting to
        ///            connect the socket.
        void Connect(const sockaddr* addr, size_type addrlen);

        /// @brief Receives a UDP datagram from the remote host established in
        ///        the @see{Connect} method.
        ///
        /// @param buffer     The array to copy the data into.
        /// @param len        The size of @p{buffer}
        /// @returns          The number of bytes read.
        size_type Receive(char buffer[], size_type len) const;

        /// @brief Receives a UDP datagram that was sent by a remote host.
        ///
        /// @param buffer     The array to copy the data into.
        /// @param len        The size of @p{buffer}
        /// @param remoteAddr When at least one byte is read, contains the address
        ///                   of the remote host.
        /// @param addrLen    The size of the struct pointed to by @p{address}.
        /// @returns          The number of bytes read.
        size_type Receive(char buffer[], size_type len,
                          sockaddr* remoteAddr, socklen_t* addrLen) const;

        /// @brief Sends a UDP datagram to the remote host established in the
        ///        @see{Connect} method.
        ///
        /// @param buffer The data to be sent.
        /// @param len    The number of bytes to send.
        /// @returns      The number of bytes sent.
        /// @exception std::system_error An error occurred when attempting to
        ///            access the socket, or the socket is not already bound to
        ///            a default remote host.
        size_type Send(const char buffer[], size_type len) const;

        /// @brief Sends a UDP datagram to a specified port on a specified
        ///        remote host.
        ///
        /// @param buffer   The data to be sent.
        /// @param len      The number of bytes to send.
        /// @param hostname The name of the remote host to which the data will
        ///                 be sent.
        /// @param port     The port of the remote host to which the data will
        ///                 be sent.
        /// @returns        The number of bytes sent.
        /// @exception std::system_error An error occurred when attempting to
        ///            access the socket.
        size_type Send(const char buffer[], size_type len,
                  const std::string& hostname, unsigned short port) const;

        /// @brief Sends a UDP datagram to a specified port on a specified
        ///        remote host.
        ///
        /// @param buffer  The data to be sent.
        /// @param len     The number of bytes to send.
        /// @param address A pointer to a sockaddr struct containing the
        ///                destination host's information.
        /// @param addrLen The size of the struct pointed to by @p{address}.
        /// @returns       The number of bytes sent.
        /// @exception std::system_error An error occurred when attempting to
        ///            access the socket.
        size_type Send(const char buffer[], size_type len,
                  sockaddr* address, socklen_t addrLen) const;

        /// @brief Gets the underlying socket.
        inline sock_type Socket() noexcept { return sock; }

        /// @brief Sets the underlying socket.
        inline void Socket(sock_type s) {
            sock = s;
            family = AF_UNSPEC;
        }

        const static sock_type InvalidSocket;

    private:
        void CreateSocket(int af);

        int family;
        sock_type sock;
    };
}
#endif // PCOE_UDPSOCKET_H
//Contact GitHub API Training Shop Blog About
