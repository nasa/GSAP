#include "TCPTests.h"
#include "TCPSocket.h"
#include "Test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>


using namespace PCOE;
using namespace PCOE::Test;

std::mutex mtx, mtx2, mtx3;


void newServer()
{
  int socket_desc, new_socket, c;
  int n;
  struct sockaddr_in server, client;
  char *message;

  //Create socket
  socket_desc =socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1)
  {
    printf("Could not create socket");
  }

  //Prepare the sockaddr_in structure
  server.sin_family=AF_INET;
  server.sin_addr.s_addr= inet_addr("127.0.0.1");
  server.sin_port= htons(8887);
  //bind
  if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) <0)
  {
    printf("bind failed");
  }


  mtx.unlock();
  //listen
  listen(socket_desc,3);

  c= sizeof(struct sockaddr_in);

  new_socket= accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

  if (new_socket<0)
  {
    printf("accept failed");
  }

/*  message ="Hello client, I have received connection";

  char buf[255];
  n= read(new_socket,buf,255);
  std::cout<<buf<<std::endl;


  n= write(new_socket,message,30);
*/

close(socket_desc);

}


void serverReceive()
{
  int socket_desc1, new_socket1, c;
  int n;
  struct sockaddr_in server1, client;
  char *message;

  //Create socket
  socket_desc1 =socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc1 == -1)
  {
    printf("Could not create socket");
  }

  //Prepare the sockaddr_in structure
  server1.sin_family=AF_INET;
  server1.sin_addr.s_addr= inet_addr("127.0.0.1");
  server1.sin_port= htons(5556);

  //bind
  if(bind(socket_desc1,(struct sockaddr *)&server1, sizeof(server1)) <0)
  {
    printf("bind failed");
  }

  //listen
  mtx2.unlock();
  listen(socket_desc1,3);

  c= sizeof(struct sockaddr_in);
  new_socket1= accept(socket_desc1, (struct sockaddr *)&client, (socklen_t*)&c);
  if (new_socket1<0)
  {
    printf("accept failed");
  }

  char buf[12];
  n= read(new_socket1,buf,12);
  close(socket_desc1);

}

void serverSend()
{
  printf("line 118");
  int socket_desc2, new_socket2, c;
  int n;
  struct sockaddr_in server2, client;
  char *message;

  //Create socket
  socket_desc2 =socket(AF_INET, SOCK_STREAM, 0);
  printf("line 126");
  if (socket_desc2 == -1)
  {
    printf("Could not create socket");
  }

  //Prepare the sockaddr_in structure
  server2.sin_family=AF_INET;
  server2.sin_addr.s_addr= inet_addr("127.0.0.1");
  server2.sin_port= htons(8887);

  printf("line 137");
  //bind
  if(bind(socket_desc2,(struct sockaddr *)&server2, sizeof(server2)) <0)
  {
    printf("bind failed");
  }
  printf("line 143");
  //listen
  mtx3.unlock();
  listen(socket_desc2,3);
  printf("line 147");
  c= sizeof(struct sockaddr_in);
  new_socket2= accept(socket_desc2, (struct sockaddr *)&client, (socklen_t*)&c);
  printf("line 150");
  if (new_socket2<0)
  {
    printf("accept failed");
  }

  //message="Hello";
  //n= write(new_socket2,message,5);
  printf("line 158");
  close(new_socket2);
  printf("line 160");
}

void testNoDelay()
{
  TCPSocket foo;
  foo.Connect("localhost",630);
  bool value = foo.NoDelay();

  Assert::AreEqual(0, value);
  foo.Close();
  printf("Line no");

}

void testRecieveBufferSize()
{
  TCPSocket foo;
  foo.Connect("localhost",630);
  int value =foo.ReceiveBufferSize();
  Assert::AreNotEqual(0,value);

}

void testSendBufferSize()
{
  TCPSocket foo;
  TCPSocket::size_type bar;
  foo.Connect("localhost",630);
  bar=foo.SendBufferSize();
  Assert::IsNotNull(bar);

}


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void testConnect()
{
  mtx.lock();
  std::thread first (newServer);


  TCPSocket foo;
  mtx.lock();
  foo.Connect("127.0.0.1",8887);

  foo.Close();
  first.join();

   }

   void testSend()
   {
     mtx2.lock();
     std::thread first (serverReceive);


     TCPSocket foo;
     mtx2.lock();
     foo.Connect("127.0.0.1",5556);

     char buffer[]="Hello World";
     int bar= 12;
     foo.Send(buffer,bar);


     first.join();
     foo.Close();
   }


  void testReceive()
  {
    printf("line 232");
    mtx3.lock();
    std::thread first (serverSend);

    printf("line 239");
    TCPSocket foo;

    mtx3.lock();
    foo.Connect("127.0.0.1",8887);

    char buff[6];
    foo.Receive(buff,6);
    printf("line 250");
    //Assert::AreEqual(buff,"Hello");

    printf("line 253");
    first.join();
    printf("line 254");
    foo.Close();
  }
