//
// UDPSocketTests.cpp
// UnitTestAll
// Created by Julian Vu on 3/23/18.
//

#include <cstring>
#include "UDPSocketTests.h"
#include "UDPSocket.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

void testUDPCtor() {
    try {
        UDPSocket socket1 = UDPSocket(AF_INET, 55555);
        UDPSocket socket2 = UDPSocket(AF_INET, 55556);
        char port = '5';
        sockaddr si = {AF_INET, port};
        UDPSocket socket3 = UDPSocket(&si, sizeof(si));
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
    catch (...) {}
    try {
        UDPSocket socket10 = UDPSocket(0, 55561);
        Assert::Fail("Socket created with unsupported address family.");
    }
    catch (...) {}
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
    sockaddr si = {AF_INET, port};
    UDPSocket socket3 = UDPSocket(&si, sizeof(si));
    socket1.Connect(&si, sizeof(si));
    socket1.Send(buffer, sizeof(buffer)/sizeof(buffer[0]), &si, sizeof(si));

}

void testExceptionHandling() {
    UDPSocket socket1 = UDPSocket(AF_INET, 55555);
    try {
        UDPSocket socket2 = UDPSocket(AF_INET, 55555);
        Assert::Fail("Socket created using taken port.");
    }
    catch (...) {}

    UDPSocket socket3 = UDPSocket(AF_INET6, 55556);
    try {
        UDPSocket socket4 = UDPSocket(AF_INET6, 55556);
        Assert::Fail("Socket created using taken port.");
    }
    catch (...) {}

    try {
        UDPSocket socket5 = UDPSocket(-1, 55557);
        Assert::Fail("Socket created with unsupported address family.");
    }
    catch (...) {}

    int port = 55555;
    sockaddr addr = {AF_INET, port};
    try {
        UDPSocket socket6 = UDPSocket(&addr, sizeof(addr));
        Assert::Fail("Socket created using taken port.");
    }
    catch  (...) {}

    char buffer[] = "Hello, this is a test message.";
    socket1.Send(buffer, sizeof(buffer)/ sizeof(buffer[0]), "127.0.0.1", 55556);
    char buffer2[128];
    UDPSocket::size_type result = socket3.Available();
    Assert::AreEqual(31, result, "Bytes available to read is not same as bytes sent.");
    socket3.Receive(buffer2, sizeof(buffer)/ sizeof(buffer[0]));
    result = socket3.Available();
    Assert::AreEqual(0, result, "Available() returns bytes even though no more bytes are being sent.");

    port = 55558;
    addr = {AF_UNIX, port};
    try {
        socket1.Connect(&addr, sizeof(addr));
        Assert::Fail("Connected socket to socket with unsupported address family");
    }
    catch (...) {}

    port = 0;
    addr = {AF_INET, port};
    try {
        socket1.Send(buffer, sizeof(buffer) / sizeof(buffer[0]), &addr, sizeof(addr));
    }
    catch (...) {}

    port = 55556;
    addr = {AF_INET, };

    try {
        socket1.Send(buffer, sizeof(buffer) / sizeof(buffer[0]), &addr, sizeof(addr));
        socket3.Receive(buffer2, 30, &addr, reinterpret_cast<socklen_t *>(sizeof(addr)));
    }
    catch (...) {}
}

