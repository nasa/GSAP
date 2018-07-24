
//
//  PEventTests.cpp
//  ProgEventUnit
//
//  Created by Chris Teubert on 3/5/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National
//  Aeronautics and Space Administration.  All Rights Reserved.
//

#include "PEventTests.h"
#include "ProgEvent.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

void testPEventInit() {
    ProgEvent test;

    // Test Defaults
    Assert::AreEqual(UType::Point, test.getUncertainty());
    Assert::AreEqual(UType::Point, test.getTOE().uncertainty()); // Default
}

void testPEventUpdate() {
    ProgEvent test;

    // Test changing parameters

    test.timeOfEventUnit = TOE_Unit::S;
    Assert::AreEqual(TOE_Unit::S, test.timeOfEventUnit);

    test.timeOfEventUnit = TOE_Unit::CYCLES;
    Assert::AreEqual(TOE_Unit::CYCLES, test.timeOfEventUnit);

    test.timeOfEventUnit = "Custom";
    Assert::AreEqual("Custom", test.timeOfEventUnit);

    test.setUncertainty(UType::MeanSD);
    Assert::AreEqual(UType::MeanSD, test.getUncertainty());
    Assert::AreEqual(UType::MeanSD, test.getTOE().uncertainty()); // Changes for both

    //    test.setNPoints(3); // Usually private
    //    Assert::AreEqual(3, test.timeOfEvent.getNPoints());
    //    Assert::AreEqual(2, test.timeOfEvent.size());
    //    test.setUncertainty(WSAMPLES);
    //    Assert::AreEqual(3, test.timeOfEvent.getNPoints());
    //    Assert::AreEqual(6, test.timeOfEvent.size());
}
