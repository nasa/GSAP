// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <string>
#include <thread>

#include "Messages/EmptyMessage.h"
#include "Messages/Message.h"
#include "Messages/MessageBus.h"
#include "Messages/ScalarMessage.h"
#include "Messages/WaypointMessage.h"
#include "Test.h"
#include "Trajectory/EventDrivenTrajectoryService.h"
#include "TrajectoryServiceTests.h"

namespace PCOE {
    const std::string TEST_SRC = "Test";

    void testTrajectoryService() {
        MessageBus mb;

        EventDrivenTrajectoryService etc(mb,
                                         std::unique_ptr<TrajectoryService>(
                                             new TrajectoryService()),
                                         TEST_SRC);
        auto& tc = etc.getTrajectoryService();

        Test::Assert::IsTrue(tc.getSavePts().empty());

        Test::Assert::AreEqual(tc.getSavePts().size(),
                               0,
                               "Savepoints should be empty before start");

        mb.publish(std::shared_ptr<Message>(
            new EmptyMessage(MessageId::RouteStart, TEST_SRC, MessageClock::now())));
        Test::Assert::AreEqual(tc.getSavePts().size(), 0, "Savepoints should be empty after start");

        auto time = MessageClock::now();
        mb.publish(std::shared_ptr<Message>(
            new WaypointMessage(MessageId::RouteSetWP, TEST_SRC, time, 38.0098, -122.119, 30)));
        auto time2 = MessageClock::now();
        mb.publish(std::shared_ptr<Message>(
            new WaypointMessage(MessageId::RouteSetWP, TEST_SRC, time2, 38.0099, -122.118, 30)));
        mb.publish(std::shared_ptr<Message>(
            new EmptyMessage(MessageId::RouteEnd, TEST_SRC, MessageClock::now())));

        mb.processAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        auto sp = tc.getSavePts();
        Test::Assert::AreEqual(sp.size(), 2, "GetSavePts size incorrect");
        auto tmp = sp.begin();
        Test::Assert::AreEqual(*tmp, time, "GetSavePts incorrect (pt 1)");
        Test::Assert::AreEqual(*++tmp, time2, "GetSavePts incorrect (pt 2)");
        auto middle_point = tc.getPoint(time + (time2 - time) / 2);
        Test::Assert::AreEqual(middle_point.getAltitude(), 30, 5e-5, "GetPt Altitude incorrect");
        Test::Assert::AreEqual(middle_point.getLatitude(),
                               38.00985,
                               5e-5,
                               "GetPt Latitude incorrect");
        Test::Assert::AreEqual(middle_point.getLongitude(),
                               -122.1185,
                               5e-5,
                               "GetPt Longitude incorrect");

        try {
            tc.getPoint(time + (time2 - time) * 2);
            Test::Assert::Fail("Did not catch high out of range request");
        }
        catch (std::out_of_range) {
        }

        try {
            tc.getPoint(time - (time2 - time) / 2);
            Test::Assert::Fail("Did not catch low out of range request");
        }
        catch (std::out_of_range) {
        }

        mb.publish(std::shared_ptr<Message>(
            new U64Message(MessageId::RouteDeleteWP, TEST_SRC, time2.time_since_epoch().count())));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        sp = tc.getSavePts();
        Test::Assert::AreEqual(sp.size(), 1, "Checking result of delete waypoint");
        tmp = sp.begin();
        Test::Assert::AreEqual(*tmp, time);

        mb.publish(std::shared_ptr<Message>(
            new EmptyMessage(MessageId::RouteClear, TEST_SRC, MessageClock::now())));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        sp = tc.getSavePts();
        Test::Assert::AreEqual(sp.size(), 0, "Checking result of clear waypoint");
    }
}
