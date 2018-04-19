//
//  ThreadedControlTests.cpp
//  UnitTestAll
//
//  Created by Chris Teubert on 4/6/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
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
        test.start();
        test.join();
    }
    catch (std::exception ex) {
        Assert::Fail();
    }

    test = TestThreadExceptionClass();
    {   // Exceptions associated with enabling
        test.enable();
        try {
            test.enable();
            Assert::Fail("Thread was enabled twice.");
        }
        catch (...) {}

        test.start();
        try {
            test.enable();
            Assert::Fail("Thread was enabled after starting.");
        }
        catch (...) {}

        test.pause();
        try {
            test.enable();
            Assert::Fail("Thread was enabled after pausing.");
        }
        catch (...) {}

        test.stop();
        try {
            test.enable();
            Assert::Fail("Thread was enabled after stopping.");
        }
        catch (...) {}

        test.join();
        try {
            test.enable();
            Assert::Fail("Thread was enabled after joining.");
        }
        catch (...) {}
    }

    test = TestThreadExceptionClass();
    {   // Exceptions associated with starting
        test.enable();
        test.start();
        try {
            test.start();
            Assert::Fail("Thread started twice.");
        }
        catch (...) {}

        test.stop();
        try {
            test.start();
            Assert::Fail("Thread started after stopping.");
        }
        catch (...) {}

        test.join();
        try {
            test.start();
            Assert::Fail("Thread started after joining.");
        }
        catch (...) {}
    }

    test = TestThreadExceptionClass();
    {   // Exceptions associated with pausing
        test.enable();
        test.start();
        test.pause();
        try {
            test.pause();
            Assert::Fail("Thread paused twice.");
        }
        catch (...) {}

        test.stop();
        try {
            test.pause();
            Assert::Fail("Thread paused after stopping.");
        }
        catch (...) {}

        test.join();
        try {
            test.pause();
            Assert::Fail("Thread paused after joining.");
        }
        catch (...)  {}
    }

    test = TestThreadExceptionClass();
    {   // Exceptions associated with stopping
        test.enable();
        test.start();
        test.stop();
        try {
            test.stop();
            Assert::Fail("Thread stopped twice.");
        }
        catch (...) {}

        test.join();
        try {
            test.stop();
            Assert::Fail("Thread stopped after joining.");
        }
        catch (...) {}
    }

    test = TestThreadExceptionClass();
    {   // Exceptions associated with joining
        test.enable();
        test.start();
        test.stop();
        test.join();
        try {
            test.join();
            Assert::Fail("Thread joined  twice.");
        }
        catch (...) {}
    }
}

void moveCtor() {

    TestThreadClass test;

    TestThreadClass test2(std::move(test));

    Assert::AreEqual(ThreadState::Created, test.getState());
    Assert::AreEqual(ThreadState::Created, test2.getState());
}

void assignmentOperator() {
    TestThreadClass test, test2;
    test2 = std::move(test);
}

void testGetID() {
    TestThreadClass test;
    test.enable();
    test.start();
    test.stop();
    Assert::IsFalse(test.getID() == std::thread::id());
    test.join();
}
