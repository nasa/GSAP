// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <string>
#include <thread>

#include "Messages/Message.h"
#include "Messages/MessageBus.h"
#include "Messages/WaypointMessage.h"
#include "Test.h"
#include "TrajectoryService.h"
#include "TrajectoryServiceTests.h"

namespace PCOE {
    const std::string TEST_SRC = "Test";
    
    void testTrajectoryService() {
        MessageBus mb;
        TrajectoryService tc(mb, TEST_SRC);
        Test::Assert::IsTrue(tc.getSavePts().empty());
        
        mb.publish(std::shared_ptr<Message>(new EmptyMessage(MessageId::RouteStart, TEST_SRC)));
        
        auto time = MessageClock::now();
        mb.publish(std::shared_ptr<Message>(new WaypointMessage(MessageId::RouteSetWP, TEST_SRC, time, 38.00984, -122.11923, 30)));
        auto time2 = MessageClock::now();
        mb.publish(std::shared_ptr<Message>(new WaypointMessage(MessageId::RouteSetWP, TEST_SRC, time2, 38.00984, -122.11923, 30)));
        mb.publish(std::shared_ptr<Message>(new EmptyMessage(MessageId::RouteEnd, TEST_SRC)));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        auto sp = tc.getSavePts();
        Test::Assert::AreEqual(sp.size(), 2);
        auto tmp = sp.begin();
        Test::Assert::AreEqual(*tmp, time);
        Test::Assert::AreEqual(*++tmp, time2);
        
        mb.publish(std::shared_ptr<Message>(new U64Message(MessageId::RouteDeleteWP, TEST_SRC, time2.time_since_epoch().count())));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        sp = tc.getSavePts();
        Test::Assert::AreEqual(sp.size(), 1, "Checking result of delete waypoint");
        tmp = sp.begin();
        Test::Assert::AreEqual(*tmp, time);
    }
}
