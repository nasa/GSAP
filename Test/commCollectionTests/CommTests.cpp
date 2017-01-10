//
//  CommTests.cpp
//  UnitTestAll
//
//  Created by Chris Teubert on 4/6/16.
//  Copyright (c) 2016 NASA Diagnostics and Prognostics Group. All rights reserved.
//

#include <mutex>

#include "Test.h"
#include "CommTests.h"
#include "ConfigMap.h"
#include "RandomCommunicator.h"
#include "RecorderCommunicator.h"
#include "PlaybackCommunicator.h"
#include "CommunicatorFactory.h"
#include "DataStore.h"

using namespace PCOE;
using namespace PCOE::Test;

void RandomCommTest()
{
    std::mutex m;
    std::unique_lock<std::mutex> lock(m);
    // Default
    ConfigMap theMap;
    DataStore a;
    RandomCommunicator theComm(theMap);
    theComm.subscribe([&](DataStore ds) {
        std::lock_guard<std::mutex> guard(m);
        a = ds;
    });

    a["Test1"] = -1.0;

    theComm.enqueue( AllData(a, DataStoreString(), ProgDataMap()) );
    theComm.poll();
    lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    lock.lock();

    Assert::IsTrue(a["Test1"] >= 0.0, "Test1 not set");

    // Setting max
    theMap.set("max", "100");
    RandomCommunicator theComm2(theMap);
    theComm2.subscribe([&](DataStore ds) {
        std::lock_guard<std::mutex> guard(m);
        a = ds;
    });
    theComm2.enqueue( AllData(a, DataStoreString(), ProgDataMap()) );

    for (int i = 0; i < 100; i++)
    {
        theComm2.poll();
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        lock.lock();
        Assert::AreEqual(50.0, a["Test1"], 50.0, "Random value out of range [1]");
    }


    // Setting step
    theMap.set("step", "0.2");
    RandomCommunicator theComm3(theMap);
    theComm3.subscribe([&](DataStore ds) {
        std::lock_guard<std::mutex> guard(m);
        a = ds;
    });
	theComm3.enqueue(AllData(a, DataStoreString(), ProgDataMap()));

    for (auto i=0; i<100; i++)
    {
        theComm3.poll();
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        lock.lock();
        Assert::AreEqual(50.0, a["Test1"], 50.0, "Random value out of range [2]");
        bool x = a["Test1"] * 5.0 - round(a["Test1"] * 5.0) < 0.05;
        bool y = a["Test1"] * 5.0 - round(a["Test1"] * 5.0) > 0.05;
        Assert::IsTrue(x || y, "I have no idea what this is testing");
    }

    GSAPConfigMap theMap3;
    CommunicatorFactory & theFactory = CommunicatorFactory::instance();
    theFactory.Register("random", theFactory.Create<RandomCommunicator>);
    CommunicatorFactory::unique_ptr theComm4 = theFactory.Create("random", theMap3);
    Assert::IsNotNull(theComm4, "Factory failed to create communicator");
}

void RecorderCommunicatorTest()
{
    ConfigMap theMap;
    RecorderCommunicator theComm(theMap);
    DataStore a;
    a["Test1"] = -1.0;

    theMap.set("saveFile", "TestRecorderFile.txt");
    RecorderCommunicator theComm2(theMap);
}

void PlaybackCommunicatorTest()
{
    ConfigMap theMap;
//    PlaybackCommunicator theComm(theMap);
//
//    theMap.set("file", "TestRecorderFile.txt");
//    PlaybackCommunicator theComm2(theMap);
}
