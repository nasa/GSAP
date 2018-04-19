//
// TCPSocketTests.cpp
// UnitTestAll
// Created by Julian Vu on 3/22/18.
//

#include "TCPSocketTests.h"
#include "TCPSocket.h"
#include "TCPSocketServer.h"
#include "Test.h"
#include <cstring>
#include <thread>

using namespace PCOE;
using namespace PCOE::Test;

void testTCPctor() {
    try {
        TCPSocket testSocket = TCPSocket(AF_INET);
    }
    catch (...) {
        Assert::Fail("Ctor using AF_INET failed.");
    }
    try {
        TCPSocket testSocket2 = TCPSocket(AF_INET6);
    }
    catch (std::system_error ec) {
        if (ec.code().value() != EAFNOSUPPORT) {
            Assert::Fail("Ctor using AF_INET6 failed.");
        }
    }

    TCPSocket testSocket = TCPSocket(AF_INET);
    try {
        TCPSocket testSocket4(std::move(testSocket));
    }
    catch (...) {
        Assert::Fail("Move Ctor failed.");
    }
    TCPSocket testSocket4, testSocket5;
    testSocket4 = TCPSocket(AF_INET);
    try {
        testSocket5 = std::move(testSocket4);
    }
    catch (...) {
        Assert::Fail("= operator failed.");
    }
}

void testTCPServerCtor() {
    TCPSocketServer testServer = TCPSocketServer(AF_INET);
    TCPSocketServer testServer2 = TCPSocketServer(AF_INET, "127.0.0.1", 55555);
    TCPSocketServer testServer3(std::move(testServer));
    TCPSocketServer testServer4;
    testServer4 = std::move(testServer3);
}

void testTCPSendAndReceive() {
    TCPSocketServer testServer = TCPSocketServer(AF_INET);
    testServer.Listen();

    TCPSocket testClient = TCPSocket(AF_INET);

    testClient.Connect("127.0.0.1", 8080);

    char buffer[1024] = "Hello, this is a test message.";
    testClient.Send(buffer, sizeof(buffer) / sizeof(buffer[0]));
    testServer.Accept();
    char serverBuffer[1024] = {0};
    testServer.Receive(serverBuffer, sizeof(serverBuffer) / sizeof(serverBuffer[0]));

    Assert::AreEqual(0, strcmp(buffer, serverBuffer));

    char serverReturnMessage[1024] = "Hello from Server.";
    testServer.Send(
        0, serverReturnMessage, sizeof(serverReturnMessage) / sizeof(serverReturnMessage[0]));

    char receiveBuffer[1024] = {0};
    testClient.Receive(receiveBuffer, sizeof(receiveBuffer) / sizeof(receiveBuffer[0]));
    std::cout << receiveBuffer << std::endl;

    TCPSocket::size_type result = testClient.Available();
    Assert::AreEqual(0, result, "Bytes available to read not 0");

    testServer.Listen();

    TCPSocket testClientTwo = TCPSocket("127.0.0.1", 8080);

    char buffer2[] = "Hello, this is a second message from ClientTwo.";
    testClientTwo.Send(buffer2, strlen(buffer2));
    testServer.Accept();
    char serverBuffer2[1024] = {0};
    testServer.Receive(serverBuffer2, sizeof(serverBuffer2) / sizeof(serverBuffer2[0]));

    Assert::AreEqual(0, strcmp(buffer2, serverBuffer2));

    //testServer.Listen();

    struct sockaddr_in sa = {};
    sa.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr);
    sa.sin_family = AF_INET;

    TCPSocket testClientThree = TCPSocket(AF_INET);
    testClientThree.Connect((struct sockaddr*)&sa, sizeof(sa), AF_INET);

    char buffer3[] = "Hello, this is a third message from ClientThree";
    testClientThree.Send(buffer3, strlen(buffer3));
    testServer.Accept();
    char serverBuffer3[1024] = {0};
    testServer.Receive(2, serverBuffer3, sizeof(serverBuffer3) / sizeof(serverBuffer3[0]));

    char sendAllMessage[] = "Hello, this is a message from Server to all Clients.";
    testServer.SendAll(sendAllMessage, strlen(sendAllMessage));
}

void testTCPClose() {
    TCPSocketServer testServer(AF_INET);

    TCPSocket testClient1(AF_INET);
    TCPSocket testClient2(AF_INET);
    TCPSocket testClient3(AF_INET);
    TCPSocket testClient4(AF_INET);

    testServer.Listen();

    testClient1.Connect("127.0.0.1", 8080);
    testServer.Accept();

    testServer.Listen();

    testClient2.Connect("127.0.0.1", 8080);
    testServer.Accept();

    testServer.Listen();

    testClient3.Connect("127.0.0.1", 8080);
    testServer.Accept();

    testServer.Close(1);

    testServer.Listen();
    testClient4.Connect("127.0.0.1", 8080);
    testServer.Accept();

    testServer.Close(0);

    try {
        char messageToSend[] = "Hello from Server.";
        testServer.Send(0, messageToSend, strlen(messageToSend));
        Assert::Fail("Server sent message to closed  client.");
    }
    catch (...) {
    }

    testServer.CloseAll();

    for (auto i = 1; i <= 3; ++i) {
        try {
            char messageToSend[] = "Hello from Server.";
            testServer.Send(i, messageToSend, strlen(messageToSend));
            Assert::Fail("Server sent message to closed client.");
        }
        catch (...) {
        }
    }
}

void testTCPNoDelay() {
    TCPSocketServer testServer(AF_INET);

    TCPSocket testSocket(AF_INET);

    Assert::IsFalse(testSocket.NoDelay());
    testSocket.NoDelay(true);
    Assert::IsTrue(testSocket.NoDelay());
}

void testTCPReceiveBufferSize() {
    TCPSocket testSocket(AF_INET);

    testSocket.ReceiveBufferSize(2048);
#if !defined(_WIN32) && !defined(__APPLE__)
    Assert::AreEqual(4096, testSocket.ReceiveBufferSize(), "Size of receive buffer not equal to set value.");
#else
    Assert::AreEqual(2048, testSocket.ReceiveBufferSize(), "Size of receive buffer not equal to set value.");
#endif
}

void testTCPReceiveTimeout() {
    TCPSocket testSocket(AF_INET);

    testSocket.ReceiveTimeout(100.0);
#ifdef WIN32
    Assert::AreEqual(testSocket.ReceiveTimeout(), 100 * 1e3);

    TCPSocket::timeout_type value = 200;
    testSocket.ReceiveTimeout(value);
    Assert::AreEqual(200, testSocket.ReceiveTimeout());
#else
    Assert::AreEqual(testSocket.ReceiveTimeout().tv_sec, 100);

    TCPSocket::timeout_type value = {};
    value.tv_sec = 200;
    testSocket.ReceiveTimeout(value);
    Assert::AreEqual(200, testSocket.ReceiveTimeout().tv_sec);
#endif
}

void testTCPSendBufferSize() {
    TCPSocket testSocket(AF_INET);

    testSocket.SendBufferSize(4096);
#if !defined(_WIN32) && !defined(__APPLE__)
    Assert::AreEqual(8192, testSocket.SendBufferSize(), "Size of send buffer not equal to set value.");
#else
    Assert::AreEqual(4096, testSocket.SendBufferSize(), "Size of send buffer not equal to set value.");
#endif
}

void testTCPSendTimeout() {
    TCPSocket testSocket(AF_INET);

    testSocket.SendTimeout(100.0);
#ifdef WIN32
    Assert::AreEqual(testSocket.SendTimeout(), 100 * 1e3);

    TCPSocket::timeout_type value = 200;
    testSocket.SendTimeout(value);
    Assert::AreEqual(200, testSocket.SendTimeout());
#else
    Assert::AreEqual(100, testSocket.SendTimeout().tv_sec);

    TCPSocket::timeout_type value = {};
    value.tv_sec = 200;
    testSocket.SendTimeout(value);
    Assert::AreEqual(200, testSocket.SendTimeout().tv_sec);
#endif
}

void testTCPExceptions() {
    TCPSocketServer testServer(AF_INET);
    TCPSocket testSocket1(AF_INET);
    TCPSocket testSocket2(AF_INET);
    TCPSocket testSocket3(AF_INET);
    char messageFromServer[] = "Hello from Server";
    char hugeMessage[INT16_MAX] = {0};

    //testServer.Listen();

    // Server's Listen() exception
    try {
        TCPSocketServer testServer2(AF_INET, "0.0.0.0", 8080);
        testServer2.Close();
        testServer2.Listen();
        Assert::Fail("Server attempted to listen for connections after closing.");
    }
    catch (std::system_error ec) {
    }
    printf("hi!");

    // Server's Accept() exception tests
    try {
        TCPSocketServer testServer2(AF_INET, "0.0.0.0", 8080);
        testServer2.Accept();
        Assert::Fail("Server tried accepting a connection when it's not listening.");
    }
    catch (std::system_error ec) {
        
    }

    // Server's CreateServer() exception tests
    try {
        TCPSocketServer failServer(AF_INET6);
        Assert::Fail("Created server with unsupported address family.");
    }
    catch (std::system_error ec) {}
    catch (std::invalid_argument) {}

#if !defined(WIN32) && !defined(__APPLE__)
    // AF_PACKET doesn't exist on Windows
    try {
        TCPSocketServer failServer(AF_PACKET);
        Assert::Fail("Created server with unsupported address family.");
    }
    catch (std::system_error ec) {}
    catch (std::invalid_argument) {}
#endif

    try {
        TCPSocketServer failServer(1024);
        Assert::Fail("Created server with unsupported address family.");
    }
    catch (std::system_error ec) {}
    catch (std::invalid_argument) {}

    try {
        TCPSocketServer failServer(AF_INET, "bad hostname", 55555);
        Assert::Fail("Created server with bad hostname.");
    }
    catch (std::system_error ec) {
    }

    try {
        TCPSocketServer failServer(AF_INET, "127.0.0.1", 80);
        Assert::Fail("Created server with bad port.");
    }
    catch (std::system_error ec) {
    }

    try {
        TCPSocketServer failServer(1024, "127.0.0.1", 8080);
        Assert::Fail("Created server with unsupported address family.");
    }
    catch (std::system_error) {
    }
    catch (std::invalid_argument) {}

#if !defined(WIN32) && !defined(__APPLE__)
    // AF_PACKET doesn't exist on Windows
    try {
        TCPSocketServer failServer(AF_PACKET, "127.0.0.1", 8080);
        Assert::Fail("Created server with unsupported address family.");
    }
    catch (std::system_error) {
    }
    catch (std::invalid_argument) {}
#endif

    // Client's Connect() exception tests
    testSocket1.Close();
    try {
        TCPSocketServer testServer2(AF_INET);
        testSocket1.Connect("127.0.0.1", 8080);
        Assert::Fail("Socket attempted connection after closing.");
    }
    catch (std::system_error) {
    }

    TCPSocket testSocket4(AF_INET);
    try {
        testSocket4.Connect("bad hostname", 8080);
        Assert::Fail("Socket connected to bad hostname.");
    }
    catch (...) {
    }

#ifndef WIN32
    // Client's Receive() exception tests
    testServer.Close();
    TCPSocketServer testReceiveExceptionServer(AF_INET);
    testReceiveExceptionServer.Listen();
    TCPSocket testSocket5("127.0.0.1", 8080);
    testReceiveExceptionServer.Accept();
    try {

        char recvBuffer[1024];
        testReceiveExceptionServer.Send(0, messageFromServer, strlen(messageFromServer));
        testSocket5.Close();
        testSocket5.Receive(recvBuffer, strlen(messageFromServer));
        Assert::Fail("Socket received message after closing.");
    }
    catch (std::system_error ec) {
        testReceiveExceptionServer.Close();
    }
#endif

    // Client-creation exception tests
    try {
        TCPSocket failSocket(1024);
        TCPSocket failSocket2(AF_UNIX);
#if !defined(WIN32) && !defined(__APPLE__)
        TCPSocket failSocket3(AF_PACKET);
#endif
        Assert::Fail("Socket created with bad address family.");
    }
    catch (...) {
    }

    TCPSocketServer testConnectExceptionServer(AF_INET);
    testConnectExceptionServer.Listen();

    // Client's Connect() with sockaddr exception test
    testSocket4.Close();
    struct sockaddr_in sa = {};
    sa.sin_port = htons(80);
    inet_pton(AF_INET, "bad address", &sa.sin_addr);
    sa.sin_family = AF_INET;
    try {
        testSocket4.Connect((struct sockaddr*)&sa, sizeof(sa), AF_INET);
        Assert::Fail("Client connected to Server with bad port/address.");
    }
    catch (...) {
    }

    // Client's NoDelay exception tests
    testSocket4.Close();
    try {
        testSocket4.NoDelay();
        Assert::Fail("Invalid socket got delay value.");
    }
    catch (...) {
    }

    try {
        testSocket4.NoDelay(true);
        testSocket4.NoDelay(false);
        Assert::Fail("Invalid socket set delay value.");
    }
    catch (...) {
    }

    // Client's ReceiveBufferSize exception tests
    try {
        testSocket4.ReceiveBufferSize();
        Assert::Fail("Invalid socket got recv buffer size value.");
    }
    catch (...) {
    }

    try {
        testSocket4.ReceiveBufferSize(100);
        Assert::Fail("Invalid socket set recv buffer size value.");
    }
    catch (...) {
    }

    // Client's ReceiveTimeout exception tests
    try {
        testSocket4.ReceiveTimeout();
        Assert::Fail("Invalid socket got timeout value.");
    }
    catch (...) {
    }

    try {
        testSocket4.ReceiveTimeout(100.0);
        Assert::Fail("Invalid socket set timeout value.");
    }
    catch (...) {
    }

    try {
#ifdef WIN32
        TCPSocket::timeout_type val = 100;
#else
        TCPSocket::timeout_type val{};
        val.tv_sec = 100;
#endif

        testSocket4.ReceiveTimeout(val);
    }
    catch (...) {
    }

    // Client's SendBufferSize() exception tests
    try {
        testSocket4.SendBufferSize();
        Assert::Fail("Invalid socket got send buffer size value.");
    }
    catch (...) {
    }

    try {
        testSocket4.SendBufferSize(100);
        Assert::Fail("Invalid socket set send buffer size value.");
    }
    catch (...) {
    }

    // Client's SendTimeOut() exception tests
    try {
        testSocket4.SendTimeout();
        Assert::Fail("Invalid socket got send timeout value.");
    }
    catch (...) {
    }

    try {
        testSocket4.SendTimeout(100.0);
        Assert::Fail("Invalid socket set send timeout value.");
    }
    catch (...) {
    }

    try {
#ifdef WIN32
        TCPSocket::timeout_type t = 100;
#else
        TCPSocket::timeout_type t = {};
        t.tv_sec = 100;
#endif
        testSocket4.SendTimeout(t);
        Assert::Fail("Invalid socket set send timeout value.");
    }
    catch (...) {
    }

    // Client's Send() exception test
    try {
        char messageToSend[] = "Hello from client. This message should not be able to be sent.";
        testSocket4.Send(messageToSend, strlen(messageToSend));
        Assert::Fail("Invalid socket sent message.");
    }
    catch (...) {
    }

    try {
        testSocket4.Available();
        Assert::Fail("Invalid socket got bytes available to read.");
    }
    catch (...) {
    }
}
