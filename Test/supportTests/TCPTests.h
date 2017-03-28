/**  TCPSocket Class - Header
*   @ingroup   GPIC++
*   @ingroup   support
*
*   @brief     Unit tests for TCPSocket Class.
*
*   @author    Micah Ricks
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Micah Ricks (mricks1@bulldogs.aamu.edu)
*      Created: 3/7/2017
       Updated: 3/25/2017
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <thread>
#include <mutex>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif



#ifndef TCPTESTS_H
#define TCPTESTS_H

// TCP tests

void testConnect();
void testReceive();
void testSend();
void newSever();
void serverSend();
void serverReceive();
void error(char *msg);

#endif // TCPTESTS_H
