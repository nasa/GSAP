//
//  ConfigMapTests.cpp
//  UnitTestAll
//
//  Created by Chris Teubert on 4/5/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//

#include "Test.h"
#include "ConfigMap.h"
#include "GSAPConfigMap.h"

#include "ConfigMapTests.h"

using namespace PCOE;
using namespace PCOE::Test;

void configMapInit()
{
    ConfigMap theMap;
    Assert::AreEqual(0, theMap.size());
}

void configMapLoadArgs() {
    const int argc = 4;
    char *argv[argc] = {"-test", "-test2", "-test3" , "badTest"};
    ConfigMap theMap(argc, argv);
}

void configMapUse()
{
    ConfigMap theMap;
    theMap["test"] = std::vector<std::string>({"test"});
    Assert::AreEqual(1, theMap["test"].size());
    Assert::AreEqual(0, theMap["test"][0].compare("test"));

    theMap.set("test2","blah");
    Assert::AreEqual(0, theMap["test2"][0].compare("blah"));

    /* Added test cases for incldues() - Julian */
    Assert::IsTrue(theMap.includes({"test"}));
    Assert::IsTrue(theMap.includes({"test2"}));
    Assert::IsFalse(theMap.includes({"test3"}));

    //std::string exampleLine("test3:a,b,dslfjs,d");
    //theMap.add(exampleLine);
    //assert(theMap["test3"].size() == 4);
    //assert(theMap["test3"][0].compare("a")==0);
    //assert(theMap["test3"][2].compare("dslfjs")==0);

    // The error case- improperline
    //ConfigMap theMap2;
    //theMap2.add("test4");
    //assert(theMap2.size() == 0);
}

void configMapLoad()
{
    /* New test here - Julian */
    ConfigMap theMap;
    try {
        theMap.addSearchPath("../Test/supportTests");
        theMap = ConfigMap("Test.cfg");
        Assert::AreNotEqual(0, theMap["test"][0].compare("modelBasedPrognoser"));
    }
    catch(...) {}
}

void configMapLoadNonexistent()
{
    /* New test here - Julian */
    // Add search path with / character
    ConfigMap theMap;
    theMap.addSearchPath("../");
    try {
        theMap = ConfigMap("Nonexistent.cfg"); // File doesn't exist
        Assert::Fail("Found file that should not exist.");
    }
    catch (...) {}
}

void configMapAddBadSearchPath()
{
    /* New test here - Julian */
    ConfigMap theMap;
    try {
        theMap.addSearchPath("../badPath");
    }
    catch (...) {}
}

void configMapTrim()
{
    /* New test here - Julian */
    ConfigMap theMap;
    theMap.addSearchPath("../Test/supportTests");
    theMap = ConfigMap("Test.cfg");
}

void gsapConfigMapInit()
{
    GSAPConfigMap theMap;
}

void gsapConfigMapUse()
{
    GSAPConfigMap theMap;

    // checkRequiredParams- Dont exist
    try {
        theMap.checkRequiredParams({"test1","test2"});
        Assert::Fail("Found params that shouldn't exist [0]");
    }
    catch (std::runtime_error e) {}

    // checkRequiredParams- Mix exist
    theMap.set("test1", "blah");
    try {
        theMap.checkRequiredParams({"test1","test2"});
        Assert::Fail("Found params that shouldn't exist [1]");
    }
    catch (std::runtime_error e) {}

    // checkRequiredParams- Do exist
    theMap.set("test2", "blah");
    theMap.checkRequiredParams({ "test1","test2" });
}
