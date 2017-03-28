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
    sock_type sock, client_sock;

  public:
    TCPServer(const int);
    sock_type listen();
    void close();


  };
}


#endif
