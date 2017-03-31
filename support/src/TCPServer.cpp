/**  TCPServer - cpp
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
#include "TCPServer.h"

namespace PCOE {
  // constructor which takes in port number
  TCPServer::TCPServer(const int p) {
      #ifdef _WIN32
           struct addrinfo server;   // socket structure in windows
           BOOL bOptVal;
      #else
           struct sockaddr_in server;  // socket structure for linux
      #endif

      sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    // creates socket
      int port = p;
      int enable = 1;
      if (sock == -1) {
          perror("Socket Creation Failed: The following error occurred");
          exit(1);
        }

        #ifdef _WIN32
        bOptVal = TRUE;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal,
        sizeof(int)) < 0) {
            throw "Failed to set SO_REUSEADDR";
          }
        #else
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable,
          sizeof(int)) < 0) {
            throw "Failed to set SO_REUSEADDR";
          }
       #endif

       #ifdef _WIN32
       ZeroMemory(&server, sizeof(server));
       server.ai_family = AF_INET;
       server.ai_socktype = SOCK_STREAM;
       server.ai_protocol = IPPROTO_TCP;
       server.ai_flags = AI_PASSIVE;
       #else
       bzero((char *)&server, sizeof(server));
       server.sin_family = AF_INET;
       server.sin_addr.s_addr = inet_addr("127.0.0.1");
       server.sin_port = htons(port);
      #endif
      // binds the address to the socket
     if (bind(sock, (struct sockaddr *)&server, sizeof(server)) <0) {
         perror("Bind Failed: The following error occurred");
         exit(1);
  }
}

  // listens for incoming connection
  TCPServer::sock_type TCPServer::listen() {
      int iResult;
      sock_type client_sock;
      iResult = ::listen(sock, 3);
      if (iResult == -1) {
        #ifdef _WIN32
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();

        #else
        printf("listen failed ");
        #endif
        exit(1);
      }
    client_sock = accept(sock, NULL, NULL);  // accepts the connection
    if (client_sock <0) {
        printf("accept failed \n");
        exit(1);
    }
    return client_sock;
  }

// closes socket
  void TCPServer::close() {
    #ifdef _WIN32
          closesocket(sock);
    #else
          ::close(sock);
    #endif
  }
}  // namespace PCOE
