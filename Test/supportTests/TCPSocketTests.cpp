//
// TCPSocketTests.cpp
// UnitTestAll
// Created by Julian Vu on 3/22/18.
//

#include "TCPSocketTests.h"
#include "Test.h"
#include "TCPSocket.h"

using namespace PCOE;
using namespace PCOE::Test;

void testTCPctor() {
    TCPSocket testSocket = TCPSocket(0);
    TCPSocket testSocket2 = TCPSocket(10);
    TCPSocket testSocket3 = TCPSocket("127.0.0.1", 10);

}
