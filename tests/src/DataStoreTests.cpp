// Copyright (c) 2016-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <chrono>

#include "Test.h"
#include "DataStore.h"
#include "Datum.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace DataStoreTests {
    void DStoreInit() {
        DataStore ds;
        Assert::AreEqual(0, ds.size(), "Non-zero size after initialization");
    }

    void DStoreUse() {
        using namespace std::chrono;
        DataStore ds;
        Datum<double>::time_point clock1 = Datum<double>::clock::now();
        ds["test"] = 1.0;
        Assert::AreEqual(1, ds.size(), "Incorrect data store size after first insert");
        Assert::AreEqual(1.0, ds["test"], 1e-12, "Incorrect data value after first insert");
        Assert::IsTrue(
            ds["test"].getTime() >= duration_cast<milliseconds>(clock1.time_since_epoch()).count(),
            "Insertion time before creation time");
        Assert::IsTrue(
            ds["test"].getTime() <= duration_cast<milliseconds>(Datum<double>::clock::now().time_since_epoch()).count(),
            "Insertion time is in the future");
        Assert::AreEqual(1.0, ds["test"].get(), 1e-12, "Incorrect value using get");
        ds["test"].set(1.2);
        Assert::AreEqual(1.2, ds["test"], 1e-12, "Incorrect data value after set");
        Datum<double> a;
        a.set(1.3);
        ds["test2"] = a;
        Assert::AreEqual(1.3, ds["test2"], 1e-12, "Incorrect value of test2");
        Assert::AreEqual(1.2, ds["test"], 1e-12, "Incorrect value of test after setting test2");

        Datum<double> b(a);
        a.set(1.5);
        Assert::AreEqual(1.3, b, 1e-12, "Incorrect value of b");
        Assert::AreEqual(1.5, a, 1e-12, "Incorrect value of a");
        Assert::AreEqual(1.3, ds["test2"], 1e-12, "Incorrect value of test2 after creating b");

        Datum<double> c(1.9);
        Assert::AreEqual(1.9, c, 1e-12, "Incorrect value of c");
        
        Datum<double> d;
        Assert::IsFalse(d.isSet(), "Datum not set");
        Datum<double> e(1.7);
        Assert::IsTrue(e.isSet(), "Datum is set");
    }

    void registerTests(TestContext& context) {
        context.AddTest("Init", DStoreInit, "DStore");
        context.AddTest("Use", DStoreUse, "DStore");
    }
}
