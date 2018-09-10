//
//  DPointTests.cpp
//  DataPointUnit
//
//  Created by Chris Teubert on 3/6/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//

#include "Test.h"
#include "DPointTests.h"
#include "DataPoint.h"

using namespace PCOE;
using namespace PCOE::Test;

void testDPointInit()
{
    DataPoint test;

    // Test Defaults
    Assert::AreEqual(UType::Point, test.getUncertainty());  // Default
    Assert::AreEqual(0, test.getNumTimes());
    Assert::AreEqual(0, test.getNPoints());  // Only now

    //Assert::AreEqual(1, test[0].size());
    Assert::IsNaN(static_cast<double>(test[0][0]));

    try {
        test[0][1];  // Should only have one element
        Assert::Fail();  // Should have failed before this
    }
    catch (std::out_of_range a) { }
}

void testDPointUpdate()
{
    DataPoint test;

    // Test changing parameters
    test.setUncertainty(UType::MeanSD);
    Assert::AreEqual(UType::MeanSD, test.getUncertainty());
    Assert::AreEqual(UType::MeanSD, test[0].uncertainty()); // Changes for both

    test.setNumTimes(5);
    Assert::AreEqual(5 , test.getNumTimes());
    try {
        auto tmp = test[6]; // Should be out of range
        Assert::Fail();
    }
    catch (std::out_of_range a) { }
    Assert::AreEqual(UType::MeanSD, test[4].uncertainty());
    Assert::AreEqual(test[4].uncertainty(), test[4].uncertainty()); // Access
//
//    test.setNPoints(3); // Usually private
//    Assert::AreEqual(3, test[0].getNPoints());
    Assert::AreEqual(2, test[0].size());
//
    test.setUncertainty(UType::WSamples);
//    Assert::AreEqual(3, test[0].getNPoints());
    Assert::AreEqual(UType::WSamples, test[4].uncertainty());
    Assert::AreEqual(UType::WSamples, test[0].uncertainty());
}
