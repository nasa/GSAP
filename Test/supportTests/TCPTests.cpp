/**  TCPSocket Class - cpp
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


#include "TCPTests.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

//declaration of mutex
std::mutex mtx, mtx2, mtx3;


//function to create a server and connect to TCPSocket
void newServer() {
  TCPServer server=  TCPServer(8887);

    mtx.unlock();

  //listen
    int client = server.Listen();
    close(client);
    server.Close();
  }

//creates a TCPServer and receives message from TCPSocket
void serverReceive() {
    TCPServer server(5556);
    //listen
    mtx2.unlock();
    int client = server.Listen();

    char buf[12];
    int n = read(client,buf,12);
    close(client);
    server.Close();
}


//creates a TCPServer and sends message to TCPSocket
void serverSend() {
    TCPServer server(8888);
    //listen
    mtx3.unlock();
    int client = server.Listen();
    char message[] = "Hello";

    int n = write(client,message,6);
    mtx3.unlock();

    close(client);
    server.Close();
}



void testConnect() {
  mtx.lock();
  std::thread first (newServer);


  TCPSocket foo;
  mtx.lock();
  foo.Connect("127.0.0.1",8887);

  foo.Close();
  first.join();

}

void testSend() {
     mtx2.lock();
     std::thread first (serverReceive);


     TCPSocket foo;
     mtx2.lock();
     foo.Connect("127.0.0.1",5556);

     char buffer[]="Hello World";
     int bar= 12;
     int y = foo.Send(buffer,bar);

     Test::Assert::AreNotEqual(y,0);


     first.join();
     foo.Close();
}


void testReceive() {
    mtx3.lock();
    std::thread first(serverSend);
    TCPSocket foo;

    mtx3.lock();
    foo.Connect("127.0.0.1",8888);
    char buff[6]={0};

    mtx3.lock();
    foo.Receive(buff,6);
    Test::Assert::AreEqual(std::string(buff), std::string("Hello"));

    first.join();
    foo.Close();
}
