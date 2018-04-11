//
// UDPSocketTests.cpp
// UnitTestAll
// Created by Julian Vu on 3/23/18.
//

#include "UDPSocketTests.h"
#include "Test.h"
#include "UDPSocket.h"
#include <cstring>

using namespace PCOE;
using namespace PCOE::Test;

void testUDPCtor() {
    try {
        UDPSocket socket1 = UDPSocket(AF_INET, 55555);
        UDPSocket socket2 = UDPSocket(AF_INET, 55556);
        sockaddr_in si = {};
        si.sin_family = AF_INET;
        si.sin_port = htons(55557);
        UDPSocket socket3 = UDPSocket((struct sockaddr*)&si, sizeof(si));
        UDPSocket socket4 = UDPSocket("127.0.0.1", 55558);
        UDPSocket socket5 = std::move(socket4);
        UDPSocket socket6;
        socket6 = std::move(socket5);
    }
    catch (...) {
        Assert::Fail("Constructor(s) failed.");
    }
    UDPSocket socket8 = UDPSocket(AF_INET6, 55559);
    try {
        UDPSocket socket9 = UDPSocket(AF_UNIX, 55560);
        Assert::Fail("Socket created with unsupported address family.");
    }
    catch (...) {
    }
    try {
        UDPSocket socket10 = UDPSocket(65000, 55561);
        Assert::Fail("Socket created with unsupported address family.");
    }
    catch (...) {
    }
}

void testUDPSendandReceive() {
    size_t expectedByteSize = 30;
    UDPSocket socket1 = UDPSocket(AF_INET, 55555);
    UDPSocket socket2 = UDPSocket(AF_INET, 55556);

    char buffer[128] = "Hello. This is a test message.";

    socket1.Send(buffer, 30, "127.0.0.1", 55556);

    char buffer2[128] = {0};
    size_t actual = socket2.Receive(buffer2, 30);

    Assert::AreEqual(expectedByteSize, actual, "Byte sizes are not the same.");
    Assert::IsTrue(strcmp(buffer, buffer2) == 0, "Buffers are not the same.");

    socket1.Connect("127.0.0.1", 55556);
    socket1.Send(buffer, 30);
    socket2.Receive(buffer2, 30);

    Assert::AreEqual(expectedByteSize, actual, "Byte sizes are not the same.");
    Assert::IsTrue(strcmp(buffer, buffer2) == 0, "Buffers are not the same.");

    int port = 55557;
    sockaddr_in si = {};
    si.sin_family = AF_INET;
    si.sin_port = htons(port);
    UDPSocket socket3 = UDPSocket((struct sockaddr*)&si, sizeof(si));
    socket1.Connect((struct sockaddr*)&si, sizeof(si));
    // socket1.Send(buffer, sizeof(buffer)/sizeof(buffer[0]), (struct sockaddr*)&si, sizeof(si));
}

void testExceptionHandling() {
    UDPSocket socket1 = UDPSocket(AF_INET, 55555);
    try {
        UDPSocket socket2 = UDPSocket(AF_INET, 55555);
        Assert::Fail("Socket created using taken port.");
    }
    catch (...) {
    }

    UDPSocket socket3 = UDPSocket(AF_INET6, 55556);
    try {
        UDPSocket socket4 = UDPSocket(AF_INET6, 55556);
        Assert::Fail("Socket created using taken port.");
    }
    catch (...) {
    }

    try {
        UDPSocket socket5 = UDPSocket(-1, 55557);
        Assert::Fail("Socket created with unsupported address family.");
    }
    catch (...) {
    }

    int port = 55555;
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    try {
        UDPSocket socket6 = UDPSocket((struct sockaddr*)&addr, sizeof(addr));
        Assert::Fail("Socket created using taken port.");
    }
    catch (...) {
    }

    char buffer[] = "Hello, this is a test message.";
    socket1.Send(buffer, sizeof(buffer) / sizeof(buffer[0]), "127.0.0.1", 55556);
    char buffer2[128];
    UDPSocket::size_type result = socket3.Available();
#if !defined(_WIN32) && !defined(__APPLE__)
    Assert::AreEqual(31, result, "Bytes available to read is not same as bytes sent.");
#else
    Assert::AreEqual(59, result, "Bytes available to read is not same as bytes sent.");
#endif
    socket3.Receive(buffer2, sizeof(buffer) / sizeof(buffer[0]));
    result = socket3.Available();
    Assert::AreEqual(
        0, result, "Available() returns bytes even though no more bytes are being sent.");

    port = 55558;
    addr.sin_family = AF_UNIX;
    addr.sin_port = htons(port);
    try {
        socket1.Connect((struct sockaddr*)&addr, sizeof(addr));
        Assert::Fail("Connected socket to socket with unsupported address family");
    }
    catch (...) {
    }

    try {
        // socket1.Close();
        socket1.Send(
            buffer, sizeof(buffer) / sizeof(buffer[0]), (struct sockaddr*)&addr, sizeof(addr));
        Assert::Fail("Invalid socket sent data.");
    }
    catch (...) {
    }

    port = 55556;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    socket1 = UDPSocket(AF_INET);
    socket1.Connect((struct sockaddr*)&addr, sizeof(addr));
    try {
        socket3.Close();
        socket1.Send(
            buffer, sizeof(buffer) / sizeof(buffer[0]), (struct sockaddr*)&addr, sizeof(addr));
        socket3.Receive(
            buffer2, 30, (struct sockaddr*)&addr, reinterpret_cast<socklen_t*>(sizeof(addr)));
        Assert::Fail("Invalid socket received data.");
    }
    catch (...) {
    }

    try {
        char messageToSend[] = "Hello, this is a test message, not meant to be sent.";
        UDPSocket failSocket(AF_INET, 55561);
        UDPSocket failSocket2(AF_INET, 55562);
        failSocket.Connect("127.0.0.1", 55562);
        failSocket.Close();
        failSocket.Send(messageToSend, strlen(messageToSend));
    }
    catch (...) {
    }

    try {
        char messageToSend[] = "Hello, this is a test message, not meant to be sent.";
        UDPSocket failSocket(AF_INET, 55561);
        UDPSocket failSocket2(AF_INET, 55562);
        failSocket.Close();
        failSocket.Send(messageToSend, strlen(messageToSend), "127.0.0.1", 55562);
    }
    catch (...) {
    }

    try {
        UDPSocket failSocket(AF_INET, 55561);
        UDPSocket failSocket2(AF_INET, 55562);
        failSocket.Close();
        failSocket.Connect("127.0.0.1", 55562);
    }
    catch (...) {
    }

    try {
        UDPSocket failSocket(AF_INET, 55561);
        UDPSocket failSocket2(AF_INET, 55562);
        failSocket.Close();
        failSocket.Available();
    }
    catch (...) {
    }
}
