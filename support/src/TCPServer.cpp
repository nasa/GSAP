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

namespace PCOE{

   //constructor which takes in port number
   TCPServer::TCPServer(const int p) {

     #ifdef _WIN32
           struct addrinfo server;
     #else
           struct sockaddr_in server;
     #endif

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int port=p;
    int enable=1;
    if (sock == -1) {
        perror("Socket Creation Failed: The following error occurred");
        exit(1);
                    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        throw "Failed to set SO_REUSEADDR";
                                                                              }

    bzero((char *) &server, sizeof(server));

    server.sin_family=AF_INET;
    server.sin_addr.s_addr= inet_addr("127.0.0.1");
    server.sin_port= htons(port);

    //bind
    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) <0) {
        perror("Bind Failed: The following error occurred");
        exit(1);
                                                                  }
}

//listens for incoming connection
int TCPServer::listen() {
    struct sockaddr_in client;
    int iResult;
    iResult=::listen(sock,3);
    if (iResult == -1) {
      #ifdef _WIN32
      printf("listen failed with error: %d\n", WSAGetLastError());
      closesocket(sock);
      WSACleanup();
      exit (1);

      #else
      printf("listen failed ");
      exit (1);
      #endif

   }

    socklen_t c = sizeof(struct sockaddr_in);
    client_sock = accept(sock, NULL,NULL);
    if (client_sock<0) {
        printf("accept failed \n");
        exit(1);
    }
    return client_sock;
                            }

void TCPServer::close() {

#ifdef _WIN32
        closesocket(sock);
#else
        ::close(sock);
#endif

                        }
}
