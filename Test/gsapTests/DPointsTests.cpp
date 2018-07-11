//
//  DPointsTests.cpp
//  DataPointsUnit
//
//  Created by Chris Teubert on 3/6/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National
//  Aeronautics and Space Administration.  All Rights Reserved.
//

#include "DPointsTests.h"
#include "DataPoint.h"
#include "DataPoints.h"
#include "PContainer.h"
#include "ProgEvent.h"
#include "ProgEvents.h"
#include "Test.h"
using namespace PCOE;
using namespace PCOE::Test;

void testPEventsInit() {
    ProgEvents test;

    // Check Defaults
    Assert::AreEqual(UType::Point, test.getUncertainty());
    Assert::AreEqual(0, test.getNTimes());
    Assert::AreEqual(1, test.getNSamples());
    Assert::AreEqual(0, test.size()); // No Events yet

    // Add New
    test.addNew("Test", "Test Desc");
    Assert::AreEqual(1, test.size()); // Just added an event
    Assert::AreEqual(UType::Point, test["Test"].getUncertainty()); // Should be point type
    Assert::AreEqual(1, test["Test"].getTOE().npoints());

    // Set Uncertainty
    test.setUncertainty(UType::MeanCovar);
    Assert::AreEqual(UType::MeanCovar, test.getUncertainty());
    Assert::AreEqual(UType::MeanCovar, test["Test"].getUncertainty()); // Should have changed type

    test.addNew("TestNew", "Test Desc");
    Assert::AreEqual(UType::MeanCovar,
                     test["TestNew"].getUncertainty()); // Should have updated type- not old one
    Assert::AreEqual(2, test["Test"].getTOE().npoints());
    Assert::AreEqual(2, test["TestNew"].getTOE().npoints());
    test.setNSamples(100);
    Assert::AreEqual(2, test["TestNew"].getTOE().npoints());

    test.setUncertainty(UType::Samples);
    Assert::AreEqual(100, test["TestNew"].getTOE().npoints());

    // Set Num Times
    test.setNTimes(5);
    Assert::AreEqual(5, test.getNTimes());
}

void testDPointsInit() {
    DataPoints test;

    // Check Defaults
    Assert::AreEqual(UType::Point, test.getUncertainty());
    Assert::AreEqual(0, test.getNTimes());
    Assert::AreEqual(0, test.size()); // No Events yet

    // Add New
    test.addNew("Test", "Test Desc");
    Assert::AreEqual(1, test.size()); // Just added an event
    Assert::AreEqual(UType::Point, test["Test"].getUncertainty()); // Should be point type
    Assert::AreEqual(1, test["Test"].getNPoints());

    // Set Uncertainty
    test.setUncertainty(UType::MeanCovar);
    Assert::AreEqual(UType::MeanCovar, test.getUncertainty());
    Assert::AreEqual(UType::MeanCovar, test["Test"].getUncertainty()); // Should have changed type

    test.addNew("TestNew", "Test Desc");
    Assert::AreEqual(UType::MeanCovar,
                     test["TestNew"].getUncertainty()); // Should have updated type- not old one
    Assert::AreEqual(2, test["Test"].getNPoints());
    Assert::AreEqual(2, test["TestNew"].getNPoints());

    // Set Num Times
    test.setNTimes(5);
    Assert::AreEqual(5, test.getNTimes());
    Assert::AreEqual(5, test["Test"].getNumTimes());
    test.addNew("TestNew2", "Test Desc");
    Assert::AreEqual(5, test["TestNew2"].getNumTimes());

    // Set N Samples
    test.setNSamples(100);
    Assert::AreEqual(5, test.getNTimes());
    Assert::AreEqual(3, test["Test"].getNPoints());
    test.setUncertainty(UType::WSamples);
    Assert::AreEqual(100, test["Test"].getNPoints()); // Uses NSamples
    test.addNew("TestNew3", "Test Desc");
    Assert::AreEqual(100, test["TestNew3"].getNPoints()); // Uses NSamples
}

void testPEventsUpdate() {
    ProgEvents test;
    test.setUncertainty(UType::WSamples);

    test.addNew("Test1", "Blah");
    test.addNew("Test2", "Blah2");
    test.setNSamples(1000);

    Assert::IsNaN(static_cast<double>(test["Test1"].getTOE()[0]));
    test["Test1"].getTOE().set(PAIR(1), std::pair<double, double>(2.0, 0.1));
    Assert::AreEqual(2.0, test["Test1"].getTOE()[SAMPLE(1)], 1e-12);
}

void testDPointsUpdate() {
    DataPoints test;
    test.setUncertainty(UType::WSamples);

    test.addNew("Test1", "Blah");
    test.addNew("Test2", "Blah2");
    test.setNSamples(1000);

    Assert::IsNaN(test["Test1"][0].get());
}

void testPEventsIncludes() {
    ProgEvents test;
    test.addNew("Test", "Test Desc");
    Assert::IsTrue(test.includes("Test"));
    Assert::IsFalse(test.includes("Test2"));
}

void testDPointsIncludes() {
    DataPoints test;
    test.addNew("Test", "Test Desc");
    Assert::IsTrue(test.includes("Test"));
    Assert::IsFalse(test.includes("Test2"));
}
