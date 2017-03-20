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

int openServer(int port) {
    struct sockaddr_in server;

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("Socket Creation Failed: The following error occurred");
        exit(1);
    }

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        throw "Failed to set SO_REUSEADDR";
    }

    bzero((char *) &server, sizeof(server));

    //Prepare the sockaddr_in structure
    server.sin_family=AF_INET;
    server.sin_addr.s_addr= inet_addr("127.0.0.1");
    server.sin_port= htons(port);
    //bind
    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) <0) {
        perror("Bind Failed: The following error occurred");
        exit(1);
    }

    return sock;
}

int serverListen(int server) {
    struct sockaddr_in client;

    listen(server,3);
    int c = sizeof(struct sockaddr_in);
    int sock = accept(server, (struct sockaddr *)&client, (socklen_t*)&c);
    if (sock<0) {
        perror("accept failed");
        exit(1);
    }
    return sock;
}

void newServer() {

  //Create socket
    int server = openServer(8887);

    mtx.unlock();
  //listen
    int client = serverListen(server);

    close(server);
    close(client);
}


void serverReceive() {
    //Create socket
    int  server = openServer(5556);

    //listen
    mtx2.unlock();
    int client = serverListen(server);

    char buf[12];
    int n = read(client,buf,12);
    close(server);
    close(client);
}

void serverSend() {
    //Create socket
    int server = openServer(8887);

    //listen
    mtx3.unlock();
    int client = serverListen(server);

    // Sends response
    char message[10] = "Hello";
    int n = write(client,message,5);
    mtx3.unlock();

    close(client);
    close(server);
}

void testNoDelay() {
  TCPSocket foo;
  foo.Connect("localhost",630);
  bool value = foo.NoDelay();

  Assert::AreEqual(0, value);
  foo.Close();
  printf("Line no");

}

void testRecieveBufferSize() {
  TCPSocket foo;
  foo.Connect("localhost",630);
  int value =foo.ReceiveBufferSize();
  Assert::AreNotEqual(0,value);

}

void testSendBufferSize() {
  TCPSocket foo;
  TCPSocket::size_type bar;
  foo.Connect("localhost",630);
  bar=foo.SendBufferSize();
  Assert::IsNotNull(bar);

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
     foo.Send(buffer,bar);
     first.join();
     foo.Close();
}

void testReceive() {
    mtx3.lock();
    std::thread first(serverSend);

    TCPSocket foo;

    mtx3.lock();
    foo.Connect("127.0.0.1",8887);

    char buff[6] = {0};

    mtx3.lock();
    foo.Receive(buff,6);

    Assert::AreEqual(std::string(buff), std::string("Hello"));

    first.join();
    foo.Close();
}
