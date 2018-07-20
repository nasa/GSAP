//
//  FrameworkTests.cpp
//  UnitTestAll
//
//  Created by Chris Teubert on 4/6/16.
//  Copyright (c) 2018 United States Government as represented by the Administrator of the National
//  Aeronautics and Space Administration.  All Rights Reserved.
//

#include <string>
#include <vector>

#include "CommManager.h"
#include "FrameworkTests.h"
#include "GSAPConfigMap.h"
#include "ProgManager.h"
#include "PrognoserFactory.h"
#include "Test.h"
#include "TestPrognoser.h"

using namespace PCOE;
using namespace PCOE::Test;

void PrognoserFactoryTest() {
    PrognoserFactory& theFactory = PrognoserFactory::instance();
    theFactory.Register<TestPrognoser>("Test");
}

void PCOE::CommManagerTest() {
    CommManager& comm = CommManager::instance();
    GSAPConfigMap theMap;
    theMap["Communicators"] = std::vector<std::string>();
    comm.configure(theMap);
    auto threadid = comm.getID();
    theMap.set("commmanger.step_size", "1000");

    comm.configure(theMap);
    Assert::AreEqual(1000, comm.stepSize);
    Assert::AreEqual(threadid, comm.getID());

    auto nLookup = comm.lookup.size();
    comm.registerKey("Test_CommManagerTest");
    Assert::AreEqual(nLookup + 1, comm.lookup.size());
    try {
        comm.lookup["Test_CommManagerTest"];
    }
    catch (int) {
        Assert::Fail();
    }

    comm.lookup["Test_CommManagerTest"] = 1.0;
    Assert::AreEqual(1.0, comm.lookup["Test_CommManagerTest"], 1e-12);
    Assert::AreEqual(1.0, comm.getValue("Test_CommManagerTest"), 1e-12);
    try {
        comm.getValue("Test_CommManagerTest2"); // Shouldn't exist
        Assert::Fail();
    }
    catch (...) {
    }

    comm.stop();
    comm.join();
}
