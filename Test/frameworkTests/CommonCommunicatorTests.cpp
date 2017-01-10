/** @file CommonCommunicatorTests.cpp
 *  @brief Tests the basic functionality of CommonCommunicator
 *
 *  @author    Jason Watkins <jason-watkins@outlook.com>
 *  @version   0.2.0
 *  @date      2016-08-04
 *
 *  @copyright Copyright (c) 2013-2016 United States Government as represented by
 *             the Administrator of the National Aeronautics and Space
 *             Administration. All Rights Reserved.
 **/

#include <thread>

#include "Test.h"

#include "CommonCommunicatorTests.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace TestCommonCommunicator {
    void construct() {
        TestCommunicator tc;
        Assert::AreEqual(0, tc.readCount, "Non-zero read count");
        Assert::AreEqual(0, tc.writeCount, "Non-zero write count");
    }

    void enqueue() {
        DataStore ds;
        ds["a"] = 42;
        ProgDataMap pdm;
        pdm["x"] = nullptr;
        DataStoreString dss;

        TestCommunicator tc;
        AllData ad(ds, dss, pdm);
        tc.enqueue(ad);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        Assert::AreEqual(1, tc.writeCount, "Write count");
        Assert::AreEqual(ds, tc.writeData, "Write data");
        Assert::AreEqual(pdm, tc.writeProgData, "Write prog data");
    }

    void subscribe() {
        DataStore ds;
        TestCommunicator tc;
        tc.subscribe([&](DataStore data) { ds = data; });
        tc.readData["a"] = 7;
        tc.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        Assert::AreEqual(1, tc.readCount, "Read count");
        Assert::AreEqual(tc.readData, ds, "Read data");
    }

    void stop() {
        using clock = std::chrono::high_resolution_clock;
        using duration = clock::duration;
        using time_point = clock::time_point;

        TestCommunicator tc;
        time_point start = clock::now();
        tc.stop();
        tc.join();
        duration timeTaken = clock::now() - start;
        Assert::IsTrue(timeTaken < std::chrono::milliseconds(1), "Took too long to join");
    }
}
