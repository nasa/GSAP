// Copyright (c) 2016-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "ConfigMap.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace ConfigMapTests {
    void configMapLoadArgs() {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwrite-strings"
#elif defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

        const int argc = 4;
        // Note (JW): Usually string literals should be `const char*`, but in
        // this case we're testing a constructor specifically designed to allow
        // setting config by passing command line arguments from the program's
        // `main`, which the C++ spec specifies as having the type char*[] for
        // argv.
        char* argv[argc] = {"-test", "-test2", "-test3", "badTest"};
        ConfigMap theMap(argc, argv);

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
    }

    void configMapUse() {
        ConfigMap theMap;
        theMap.set("test", std::vector<std::string>({"test"}));
        Assert::AreEqual(1, theMap.getVector("test").size());
        Assert::AreEqual(0, theMap.getString("test").compare("test"));

        theMap.set("test2", "blah");
        Assert::AreEqual(0, theMap.getVector("test2")[0].compare("blah"));

        Assert::IsTrue(theMap.hasKey("test"));
        Assert::IsTrue(theMap.hasKey("test2"));
        Assert::IsFalse(theMap.hasKey("test3"));

        theMap.set("testSetDouble", 2.3);
        Assert::AreEqual(2.3, theMap.getDouble("testSetDouble"), 0, "Setting double value failed.");

        theMap.set("testSetInt32", INT32_MAX);
        Assert::AreEqual(INT32_MAX, theMap.getInt32("testSetInt32"), "Setting int32 value failed.");

        theMap.set("testSetInt64", INT64_MAX);
        Assert::AreEqual(INT64_MAX, theMap.getInt64("testSetInt64"), "Setting int64 value failed.");

        theMap.set("testSetUInt64", UINT64_MAX);
        Assert::AreEqual(UINT64_MAX,
                         theMap.getUInt64("testSetUInt64"),
                         "Setting uint64 value failed.");

        theMap.set("testSetUInt32", UINT32_MAX);
        Assert::AreEqual(UINT32_MAX,
                         theMap.getUInt32("testSetUInt32"),
                         "Setting uint32 value failed.");
    }

    void configMapLoad() {
        ConfigMap theMap;
        theMap.addSearchPath("./cfg");
        theMap = ConfigMap("Test.cfg");
        Assert::AreNotEqual(0, theMap.getVector("test")[0].compare("modelBasedPrognoser"));
    }

    void configMapLoadNonexistent() {
        ConfigMap theMap;
        theMap.addSearchPath("./cfg");
        try {
            theMap = ConfigMap("Nonexistent.cfg"); // File doesn't exist
            Assert::Fail("Found file that should not exist.");
        }
        catch (std::ios_base::failure&) {
        }
    }

    void configMapAddBadSearchPath() {
        ConfigMap theMap;
        try {
            theMap.addSearchPath("./badPath");
            Assert::Fail("ConfigMap added invalid search path.");
        }
        catch (std::domain_error&) {
        }
    }

    void configMapTrim() {
        ConfigMap theMap;
        theMap.addSearchPath("./cfg");
        theMap = ConfigMap("Test.cfg");
    }

    void configMapRequireKeys() {
        ConfigMap theMap;

        // checkRequiredParams- Dont exist
        try {
            requireKeys(theMap, {"test1", "test2"});
            Assert::Fail("Found params that shouldn't exist [0]");
        }
        catch (const std::range_error&) {
        }

        // checkRequiredParams- Mix exist
        theMap.set("test1", "blah");
        try {
            requireKeys(theMap, {"test1", "test2"});
            Assert::Fail("Found params that shouldn't exist [1]");
        }
        catch (const std::range_error&) {
        }

        // checkRequiredParams- Do exist
        theMap.set("test2", "blah");
        requireKeys(theMap, {"test1", "test2"});
    }

    void registerTests(TestContext& context) {
        context.AddTest("Load Arguments", configMapLoadArgs, "Config Map");
        context.AddTest("Use", configMapUse, "Config Map");
        context.AddTest("Load", configMapLoad, "Config Map");
        context.AddTest("Load Nonexistent", configMapLoadNonexistent, "ConfigMap");
        context.AddTest("Add Bad Search Path", configMapAddBadSearchPath, "Config Map");
        context.AddTest("Trim", configMapTrim, "Config Map");
        context.AddTest("Require Keys", configMapRequireKeys, "Config Map");
    }
}
