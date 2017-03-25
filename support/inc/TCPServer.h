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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <arpa/inet.h>



namespace PCOE {
  class TCPServer {

  public:

    TCPServer(const int);
    int Listen();
    void Close();


  private:
    int sock, client_sock;



  };
}


#endif
