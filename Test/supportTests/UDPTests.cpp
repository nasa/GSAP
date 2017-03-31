/**  UDPSocket Class - cpp
*   @ingroup   GPIC++
*   @ingroup   support
*
*   @brief     Unit tests for UDPSocket Class.
*
*   @author    Micah Ricks
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Micah Ricks (mricks1@bulldogs.aamu.edu)
*      Created: 3/28/2017
       Updated: 3/25/2017
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/


#include "UDPTests.h"
//#include "TCPServer.h"
#include "UDPSocket.h"
#include "UDPServer.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

//declaration of mutex
std::mutex mtx4, mtx5, mtx6;

 void newServer_UDP(){
  UDPServer server =  UDPServer(8887);

  mtx4.unlock();

//listen
   server.receive();
   //close(client);
   server.close();
}

void serverSend_UDP(){
UDPServer server =  UDPServer(8888);

  mtx5.unlock();

//listen
   server.send();
   //close(client);
   server.close();
}

void testConnect_UDP() {
  mtx4.lock();
  std::thread first (newServer_UDP);

  UDPSocket foo;
  mtx4.lock();
  foo.Connect("127.0.0.1",8887);

  char buffer[]="Hello World";
  int bar= 12;
  int y = foo.Send(buffer,bar);

  Test::Assert::AreNotEqual(y,0);

  foo.Close();
  first.join();
}

void testReceive_UDP() {
  mtx5.lock();
  std::thread first (serverSend_UDP);

  UDPSocket foo;
  mtx5.lock();
  foo.Connect("127.0.0.1",8888);
  char buffer[]= "Hello Server";
  int  bSize= 13;
  char buff[12]={0};
  foo.Send(buffer,bSize);
  foo.Receive(buff,12);

  Test::Assert::AreEqual(std::string(buff), std::string("Hello World"));

  foo.Close();
  first.join();
}
