//
// UDPSocketTests.cpp
// UnitTestAll
// Created by Julian Vu on 3/23/18.
//

#include "UDPSocketTests.h"
#include "Net/UDPSocket.h"
#include "Test.h"
#include <cstring>
#include <sstream>
#include <thread>

using namespace PCOE;
using namespace PCOE::Test;

void testUDPCtor() {
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

    try {
        UDPSocket socket8(AF_INET6, static_cast<unsigned short>(55559));
    }
    catch (std::invalid_argument& ex) {
        std::stringstream ecValueAsString;
        ecValueAsString << ex.what();
        int ecValueAsInt;
        ecValueAsString >> ecValueAsInt;
        if (ecValueAsInt == EAFNOSUPPORT) {
            std::cout << ecValueAsInt << std::endl;
            std::cout << EAFNOSUPPORT << std::endl;
            Assert::Fail("Ctor using AF_INET6 failed.");
        }
    }

    try {
        UDPSocket socket9 = UDPSocket(AF_UNIX, 55560);
        Assert::Fail("Socket created with unsupported address family.");
    }
    catch (std::invalid_argument&) {
        // Note (JW): This is the correct exception for this call, but for some
        // reason it doesn't get triggered on Linux.
    }
    catch (std::system_error&) {
        // Note (JW): This exception is thrown on Linux (and macOS?) instead.
    }

    try {
        UDPSocket socket10 = UDPSocket(65000, 55561);
        Assert::Fail("Socket created with unsupported address family.");
    }
    catch (std::invalid_argument&) {
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
}

void testExceptionHandling() {
    UDPSocket socket1 = UDPSocket(AF_INET, 55555);
    try {
        UDPSocket socket2 = UDPSocket(AF_INET, 55555);
        Assert::Fail("Socket created using taken port.");
    }
    catch (std::system_error&) {
    }

    try {
        UDPSocket socket3 = UDPSocket(AF_INET6, 55556);

        try {
            UDPSocket socket4 = UDPSocket(AF_INET6, 55556);
            socket4.Close();
            Assert::Fail("Socket created using taken port.");
        }
        catch (std::system_error&) {
            socket3.Close();
        }
    }
    catch (std::invalid_argument& ex) {
        std::stringstream ecValueAsString;
        ecValueAsString << ex.what();
        int ecValueAsInt;
        ecValueAsString >> ecValueAsInt;
        if (ecValueAsInt == EAFNOSUPPORT) {
            std::cout << ecValueAsInt << std::endl;
            std::cout << EAFNOSUPPORT << std::endl;
            Assert::Fail("Ctor using AF_INET6 failed.");
        }
    }

    try {
        UDPSocket socket5 = UDPSocket(-1, 55557);
        Assert::Fail("Socket created with unsupported address family.");
    }
    catch (std::invalid_argument&) {
    }

    unsigned short port = 55555;
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    try {
        UDPSocket socket6 = UDPSocket((struct sockaddr*)&addr, sizeof(addr));
        Assert::Fail("Socket created using taken port.");
    }
    catch (std::system_error&) {
    }

    UDPSocket socketToReceive = UDPSocket(AF_INET, 60000);
    char buffer[] = "Hello, this is a test message.";
    const std::size_t BUF_LEN = sizeof(buffer);
    socket1.Send(buffer, sizeof(buffer) / sizeof(buffer[0]), "127.0.0.1", 60000);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    char buffer2[128];
    UDPSocket::size_type result = socketToReceive.Available();
#if !defined(_WIN32) && !defined(__APPLE__)
    Assert::AreEqual(BUF_LEN, result, "Bytes available to read is not same as bytes sent.");
#elif defined(_WIN32)
    Assert::AreEqual(BUF_LEN, result, "Bytes available to read is not same as bytes sent.");
#else
    std::cout << "Bytes available: " << result << std::endl;
    Assert::AreEqual(47, result, "Bytes available to read is not same as bytes sent.");
#endif
    socketToReceive.Receive(buffer2, sizeof(buffer) / sizeof(buffer[0]));
    result = socketToReceive.Available();
    Assert::AreEqual(0,
                     result,
                     "Available() returns bytes even though no more bytes are being sent.");

    port = 55558;
    addr.sin_family = AF_UNIX;
    addr.sin_port = htons(port);
    try {
        socket1.Connect((struct sockaddr*)&addr, sizeof(addr));
        Assert::Fail("Connected socket to socket with unsupported address family");
    }
    catch (std::system_error&) {
    }

    try {
        socket1.Close();
        socket1.Send(buffer,
                     sizeof(buffer) / sizeof(buffer[0]),
                     (struct sockaddr*)&addr,
                     sizeof(addr));
        Assert::Fail("Invalid socket sent data.");
    }
    catch (std::system_error&) {
    }

    // Note (JW): Google DNS server. Convenient address because it is the same
    // in either byte order.
    struct in_addr inaddr = {0x08080808};
    port = 55556;
    addr.sin_family = AF_INET;
    addr.sin_addr = inaddr;
    addr.sin_port = htons(port);
    socket1 = UDPSocket(AF_INET);
    socket1.Connect((struct sockaddr*)&addr, sizeof(addr));
    try {
        socketToReceive.Close();
        socket1.Send(buffer,
                     sizeof(buffer) / sizeof(buffer[0]),
                     (struct sockaddr*)&addr,
                     sizeof(addr));
        socketToReceive.Receive(buffer2,
                                30,
                                (struct sockaddr*)&addr,
                                reinterpret_cast<socklen_t*>(sizeof(addr)));
        Assert::Fail("Invalid socket received data.");
    }
    catch (std::system_error&) {
    }

    try {
        char messageToSend[] = "Hello, this is a test message, not meant to be sent.";
        UDPSocket failSocket(AF_INET, 55561);
        UDPSocket failSocket2(AF_INET, 55562);
        failSocket.Connect("127.0.0.1", 55562);
        failSocket.Close();
        failSocket.Send(messageToSend, strlen(messageToSend));
        Assert::Fail("Invalid socket sent data.");
    }
    catch (std::system_error&) {
    }

    try {
        char messageToSend[] = "Hello, this is a test message, not meant to be sent.";
        UDPSocket failSocket(AF_INET, 55561);
        UDPSocket failSocket2(AF_INET, 55562);
        failSocket.Close();
        failSocket.Send(messageToSend, strlen(messageToSend), "127.0.0.1", 55562);
        Assert::Fail("Invalid socket sent data.");
    }
    catch (std::system_error&) {
    }

    try {
        UDPSocket failSocket(AF_INET, 55561);
        UDPSocket failSocket2(AF_INET, 55562);
        failSocket.Close();
        failSocket.Connect("127.0.0.1", 55562);
        Assert::Fail("Socket connected after closing.");
    }
    catch (std::system_error&) {
    }

    try {
        UDPSocket failSocket(AF_INET, 55561);
        UDPSocket failSocket2(AF_INET, 55562);
        failSocket.Close();
        failSocket.Available();
        Assert::Fail("Socket has bytes to read after closing.");
    }
    catch (std::system_error&) {
    }
}
