//
//  ThreadedControlTests.cpp
//  UnitTestAll
//
//  Created by Chris Teubert on 4/6/16.
//  Copyright © 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//


#include "Test.h"
#include "ThreadTests.h"
#include "Thread.h"

using namespace PCOE;
using namespace PCOE::Test;

class TestThreadClass final : public Thread
{
public:
    TestThreadClass(): Thread() {}

    void run() final override {
        while (getState() != ThreadState::Stopped)
        {

        }
    }
};

class TestThreadExceptionClass final : public Thread
{
public:
    void run() final override {
        throw std::exception();
    }
};

void tctrltests() {
    TestThreadClass test;
    Assert::AreEqual(ThreadState::Created, test.getState());
    test.enable();
    Assert::AreEqual(ThreadState::Enabled, test.getState());
    test.start();
    Assert::AreEqual(ThreadState::Started, test.getState());
    test.pause();
    Assert::AreEqual(ThreadState::Paused, test.getState());
    test.stop();
    Assert::AreEqual(ThreadState::Stopped, test.getState());
    test.join();
    Assert::AreEqual(ThreadState::Ended, test.getState());
}

void exceptiontest() {
    TestThreadExceptionClass test;
    try {
        test.enable();
        test.join();
    }
    catch (std::exception ex) {
        Assert::Fail();
    }
}
