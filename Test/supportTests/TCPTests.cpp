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

class TCPServer{

public:


   TCPServer(int p)
  {
    struct sockaddr_in server;
    sock = socket(AF_INET, SOCK_STREAM, 0);
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

int Listen1() {
    struct sockaddr_in client;

    listen(sock,3);
    int c = sizeof(struct sockaddr_in);
    int client_sock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock<0) {
        printf("accept failed \n");
      //  exit(1);
    }
    return client_sock;
}

private:
  int sock;

};

void newServer() {

  //Create socket

    TCPServer server(8887);

    //int server = openServer(8887);

    mtx.unlock();
  //listen
    int client = server.Listen1();

    //close(server);
    close(client);
  }


void serverReceive() {
    //Create socket
    TCPServer server(5556);
    //listen
    mtx2.unlock();
    int client = server.Listen1();

    char buf[12];
    int n = read(client,buf,12);
    close(client);
}

void serverSend() {
    //Create socket
    TCPServer server(8888);
    //listen
    mtx3.unlock();
    int client = server.Listen1();

    // Sends response
    char message[] = "Hello";
    //message="hello";

    int n = write(client,message,6);
    mtx3.unlock();

    close(client);
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

     Assert::AreNotEqual(y,0);


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
    Assert::AreEqual(std::string(buff), std::string("Hello"));

    first.join();
    foo.Close();
}
