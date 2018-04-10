/// @class     TCPSocket
/// @brief     A wrapper around a TCP socket.
/// @details   The TCPSocket class provides simple methods for connecting,
///            sending, and receiving stream data over a network.
///
/// @remarks   If the TCPSocket is constructed using the default constructor
///            or with the AF_UNSPEC address family, socket creation is
///            defered until the connection is opened to allow multiple
///            address families from a call to getaddrinfo to be tried. As a
///            consequence, none of the properties that set socket options,
///            nor the Socket property will work until a connection is opened
///            for AF_UNSPEC TCPSockets.
///
/// @author    Jason Watkins <jason-watkins@outlook.com>
/// @author    Chris Teubert <christopher.a.teubert@nasa.gov>
/// @version   0.1.0
/// @date      2016-02-08
///
/// @copyright Copyright (c) 2016 United States Government as represented by
///            the Administrator of the National Aeronautics and Space
///            Administration. All Rights Reserved.

#ifndef PCOE_TCPSOCKET_H
#define PCOE_TCPSOCKET_H

#include <stdexcept>
#include <string>
#include <system_error>

#include <sys/types.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace PCOE {
    class TCPSocket {
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
        /// @brief Initializes a new instance of the @see{TCPSocket} class.
        ///        This constructor is equivalent to @code{TCPSocket(AF_UNSPEC)}.
        TCPSocket() : TCPSocket(AF_UNSPEC) {}

        /// @brief   Initializes a new instance of the @see{TCPSocket} class
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
        explicit TCPSocket(int af);

        /// @brief Initializes a new instance of the @see{TCPSocket} class and
        ///        connects to the specified port on the specified host.
        ///
        /// @param hostname The name of the default remote host to which the
        ///                 socket will connect.
        /// @param port     The port number of the remote host to which the
        ///                 socket will connect.
        /// @exception std::system_error An error occurred when attempting to
        ///            create or connect the socket.
        TCPSocket(const std::string& hostname, const unsigned short port);

        /// @brief Deletes the @see{TCPSocket} copy constructor. The underlying
        ///        socket is automatically closed when the instance is
        ///        destroyed, so it can't be shared between instances.
        TCPSocket(const TCPSocket&) = delete;

        /// @brief Moves the underlying socket of @p{other} to the current
        ///        instance.
        ///
        /// @param other Another @see{TCPSocket} instance.
        TCPSocket(TCPSocket&& other);

        /// @brief Closes the underlying TCP connection and releases the
        ///        resources used by the @see{TCPSocket}.
        ~TCPSocket() noexcept;

        /// @brief Deletes the @see{TCPSocket} copy assignment operator. The
        ///        underlying socket is automatically closed when the instance
        ///        is destroyed, so it can't be shared between instances.
        TCPSocket& operator=(const TCPSocket&) = delete;

        /// @brief Moves the underlying socket of @p{other} to the current
        ///        instance.
        ///
        /// @param other Another @see{TCPSocket} instance.
        TCPSocket& operator=(TCPSocket&& other);

        /// @brief   Gets the address family of the socket.
        /// @remarks The address family is either the family specified when
        ///          the TCPSocket was constructed, or the address family used
        ///          to open the connection if AF_UNSPEC was set at
        ///          construction and the socket has subsequently been opened.
        inline int AddressFamily() {
            return family;
        }

        /// @brief Gets the amount of data that has been received from the
        ///        network and is available to be read.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        size_type Available();

        /// @brief Closes the underlying TCP connection and releases the
        ///        resources used by the @see{TCPSocket}.
        void Close() noexcept;

        /// @brief Connects the client to the specified port on the specified
        ///        host.
        ///
        /// @param hostname A string representing the host to which the socket
        ///                 will connect.
        /// @param port     The port number of the remote host to which the
        ///                 socket will connect.
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        void Connect(const std::string& hostname, unsigned short port);

        /// @brief Connects the client to the remote host using the specified
        ///        address.
        ///
        /// @param address A pointer to a sockaddr struct containing the
        ///                destination host's information.
        /// @param len     The size of the structure pointed to by @p{address}.
        /// @param af      The address family of the address. If a value other
        ///                than AF_UNSPEC was specified during construction,
        ///                this parameter is ignored. Otherwise, the value of
        ///                @p{af} should be a specific address family that is
        ///                supported by the operating system.
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        void Connect(const sockaddr* address, std::size_t len, int af = AF_UNSPEC);

        /// @brief Gets a value that disables a delay when send or receive
        ///        buffers are not full.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        bool NoDelay();

        /// @brief Sets a value that disables a delay when send or receive
        ///        buffers are not full.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        void NoDelay(bool value);

        /// Receives the specified amount of data.
        ///
        /// @param buffer A buffer to hold the data received.
        /// @param len    The number of bytes to read.
        /// @returns      The number of bytes read.
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        size_type Receive(char buffer[], size_type len);

        /// @brief Gets the size of the receive buffer.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        size_type ReceiveBufferSize();

        /// @brief Sets the size of the receive buffer.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        void ReceiveBufferSize(size_type value);

        /// @brief Gets the amount of time a TcpClient will wait to receive
        ///        data once a read operation is initiated. The value is a
        ///        DWORD representing milliseconds on windows and a timeval
        ///        on *NIX platforms.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        timeout_type ReceiveTimeout();

        /// @brief Sets the amount of time a TcpClient will wait to receive
        ///        data once a read operation is initiated. The value is a
        ///        DWORD representing milliseconds on windows and a timeval
        ///        on *NIX platforms.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        void ReceiveTimeout(timeout_type value);

        /// @brief Sets the amount of time the TcpSocket will wait to receive
        ///        data once a read operation is initiated.
        ///
        /// @param value The number of seconds to wait for data before timing
        ///              out.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        void ReceiveTimeout(double value);

        /// @brief Gets the size of the send buffer.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        size_type SendBufferSize();

        /// @brief Sets the size of the send buffer.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        void SendBufferSize(size_type value);

        /// @brief Gets the amount of time a TcpClient will wait for a send
        ///        operation to complete successfully. The value is a DWORD
        ///        representing milliseconds on windows and a timeval on *NIX
        ///        platforms.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        timeout_type SendTimeout();

        /// @brief Sets the amount of time a TcpClient will wait for a send
        ///        operation to complete successfully. The value is a DWORD
        ///        representing milliseconds on windows and a timeval on *NIX
        ///        platforms.
        ///
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        void SendTimeout(timeout_type value);

        /// @brief Cross platform way of setting the  amount of time a TcpClient will wait in
        ///        seconds for the send operation to finish successfully
        void SendTimeout(double value);

        /// Sends the given data.
        ///
        /// @param buffer A pointer to the data to send.
        /// @param len    The number of bytes to send.
        /// @returns      The number of bytes sent.
        /// @exception std::system_error An error occurred when
        ///            attempting to access the socket.
        size_type Send(const char buffer[], size_type len);

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
        void CreateSocket(int af);
        bool TryConnect(const sockaddr* address, size_type len, int af);

        sock_type sock;
        int family;
    };
}

#endif // PCOE_TCPSOCKET_H
