/**  TCPServer - Header
*   @ingroup   GPIC++
*   @ingroup   support
*
*   @brief     Server class.
*
*   @author    Micah Ricks
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Micah Ricks (mricks1@bulldogs.aamu.edu)
*      Created: 3/25/2017
*
*   @copyright Copyright (c) 2017 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/
#ifndef PCOE_TCPSERVER_H
#define PCOE_TCPSERVER_H

#include <thread>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

namespace PCOE {
  class TCPServer {
   private:
    #ifdef _WIN32
            using ssize_type = int;
    #else
            using ssize_type = ssize_t;
    #endif
    #ifdef _WIN32
            using size_type = int;
            using sock_type = SOCKET;
    #else
            using size_type = size_t;
            using sock_type = int;
    #endif
    sock_type sock;
   public:
     /// @brief   Initializes a new instance of the @see{TCPServer} class
     ///          with the specified port number.
     /// @remarks The value for the port is usaully a unique value such as 8887.
     ///          The host address of the socket is local host or 127.0.0.1.
     ///          Also binds the socket to the port
     ///
     /// @param const int Is the port number, pointed to an unused port
    TCPServer(const int);

    /// @brief   Function that calls the listen function for the object as the
    ///          server waits for the connection.
    /// @remarks Returns a value of sock_type. That value represents the client
    ///          socket. If that value is positive then the connection was a
    ///          success. Also accepts the client connection.
    sock_type listen();

    /// @brief   Closes the TCPServer socket
    void close();
  };
}
#endif
