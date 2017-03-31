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
#include "UDPServer.h"

namespace PCOE{

   //constructor which takes in port number
   UDPServer::UDPServer(const int p) {

     #ifdef _WIN32
           struct addrinfo server;
		   BOOL bOptVal;
     #else
           struct sockaddr_in server;
     #endif

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int port=p;
    int enable=1;
    if (sock == -1) {
        perror("Socket Creation Failed: The following error occurred");
        exit(1);
                    }

#ifdef _WIN32
	bOptVal = TRUE;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal, sizeof(int)) < 0) {
        throw "Failed to set SO_REUSEADDR";
                                                                              }
#else
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		throw "Failed to set SO_REUSEADDR";
	}
#endif

#ifdef _WIN32
	ZeroMemory(&server, sizeof(server));
	server.ai_family = AF_INET;
	server.ai_socktype = SOCK_STREAM;
	server.ai_protocol = SOCK_DGRAM;
	server.ai_flags = AI_PASSIVE;
#else
	bzero((char *)&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(port);
#endif

    //bind
    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) <0) {
        perror("Bind Failed: The following error occurred");
        exit(1);
                                                                  }
}

//listens for incoming connection
  void UDPServer::receive() {
   /*
   * recvfrom: receive a UDP datagram from a client
   */
    bzero(buf, BUFSIZE);
    n = recvfrom(sock, buf, BUFSIZE, 0,
    (struct sockaddr *) &client, &c);
    if (n < 0)
    {
      printf("ERROR in recvfrom");
      exit(1);
    }
}

//listens for incoming connection
  void UDPServer::send() {

    recvfrom(sock, buf, BUFSIZE, 0,
    (struct sockaddr *) &client, &c);

    socklen_t c = sizeof(struct sockaddr_in);
    bzero(buf, BUFSIZE);
    char buf[]= "Hello World";
    n = sendto(sock, buf, strlen(buf), 0,
   	       (struct sockaddr *) &client, c);
    if (n < 0)
    {
      printf("ERROR in sendto");
      exit(1);
    }
}

void UDPServer::close() {

#ifdef _WIN32
        closesocket(sock);
#else
        ::close(sock);
#endif
                        }
}
