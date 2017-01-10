
//
//  PEventTests.cpp
//  ProgEventUnit
//
//  Created by Chris Teubert on 3/5/16.
//  Copyright © 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//

#include "Test.h"
#include "PEventTests.h"
#include "ProgEvent.h"

using namespace PCOE;
using namespace PCOE::Test;

void testPEventInit()
{
    ProgEvent test;

    test.setNumTimes(0);

    // Test Defaults
    Assert::AreEqual(UType::Point, test.getUncertainty());
    Assert::AreEqual(UType::Point, test.timeOfEvent.uncertainty()); // Default
    Assert::AreEqual(0, test.getNumOccurrenceSamples());
    Assert::AreEqual(0, test.getNumTimes());
    Assert::AreEqual(1, test.occurrenceMatrix.size()); // Should not change until there is a number of times (only now)
    Assert::AreEqual(0, test.occurrenceMatrix[0].size());
    Assert::AreEqual(1, test.probMatrix.size());
    Assert::IsNaN(test.probMatrix[0]);
}

void testPEventUpdate()
{
    ProgEvent test;

    // Test changing parameters

    test.timeOfEventUnit = TOE_Unit::S;
    Assert::AreEqual(TOE_Unit::S, test.timeOfEventUnit);

    test.timeOfEventUnit = TOE_Unit::CYCLES;
    Assert::AreEqual(TOE_Unit::CYCLES, test.timeOfEventUnit);

    test.timeOfEventUnit = "Custom";
    Assert::AreEqual("Custom", test.timeOfEventUnit);

    test.isEventDriven = true;
    Assert::IsTrue(test.isEventDriven);

    test.setUncertainty(UType::MeanSD);
    Assert::AreEqual(UType::MeanSD, test.getUncertainty());
    Assert::AreEqual(UType::MeanSD, test.timeOfEvent.uncertainty()); // Changes for both

    test.setNumOccurrenceSamples(10);
    Assert::AreEqual(10, test.getNumOccurrenceSamples());
    Assert::AreEqual(0, test.getNumTimes());
    Assert::AreEqual(1, test.occurrenceMatrix.size()); // Should not change until there is a number of times (only now)
    Assert::AreEqual(10, test.occurrenceMatrix[0].size());
    Assert::IsFalse(test.occurrenceMatrix[0][0]);

    test.setNumTimes(5);
    Assert::AreEqual(5, test.getNumTimes());
    Assert::AreEqual(10, test.getNumOccurrenceSamples());
    Assert::AreEqual(6, test.occurrenceMatrix.size());
    Assert::AreEqual(6, test.probMatrix.size());
    Assert::AreEqual(10, test.occurrenceMatrix[0].size());

//    test.setNPoints(3); // Usually private
//    Assert::AreEqual(3, test.timeOfEvent.getNPoints());
//    Assert::AreEqual(2, test.timeOfEvent.size());
//    test.setUncertainty(WSAMPLES);
//    Assert::AreEqual(3, test.timeOfEvent.getNPoints());
//    Assert::AreEqual(6, test.timeOfEvent.size());
}

void testPEventMeta()
{
    ProgEvent test;
    Assert::AreEqual(0, test.getName().compare("")); // Test Default
    Assert::AreEqual(0, test.getDesc().compare("")); // Test Default

    test.setMeta("Test", "Test Desc"); // Set Together
    Assert::AreEqual(0, test.getName().compare("Test"));
    Assert::AreEqual(0, test.getDesc().compare("Test Desc"));

    test.setDesc("Test Desc 2");
    Assert::AreEqual(0, test.getDesc().compare("Test Desc 2"));

    test.setName("Test 2 This is a very long name");
    Assert::AreEqual(0, test.getName().compare("Test 2 This is a very long name"));
}
