//
//  ProgDataTests.cpp
//  ProgDataUnit
//
//  Created by Chris Teubert on 3/7/16.
//  Copyright © 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//

#include "Test.h"
#include "ProgDataTests.h"
#include "ProgData.h"

using namespace PCOE;
using namespace PCOE::Test;

void progDataCopyTest()
{
    ProgData test;
    ProgData test2(test);
}

void progDataTest()
{
    ProgData test;
    // Check defaults
    Assert::IsTrue(test.getComponentName().empty());
    Assert::IsTrue(test.getPrognoserName().empty());
    Assert::IsTrue(test.getUniqueId().empty());
    Assert::AreEqual(0, test.getEventNames().size()); // No events yet
    Assert::AreEqual(0, test.getInternalNames().size()); // No internals yet
    Assert::AreEqual(0, test.getSystemTrajectoryNames().size()); // No SystemTrajectories yet
    Assert::AreEqual(0, test.getTimes().size()); // No Times
    Assert::AreEqual(UType::Point, test.events.getUncertainty());
    Assert::AreEqual(UType::Point, test.sysTrajectories.getUncertainty());

    ProgData test2("Example","Example1","5w19fs");
    Assert::AreEqual(0, test2.getComponentName().compare("Example1"));
    Assert::AreEqual(0, test2.getPrognoserName().compare("Example"));
    Assert::AreEqual(0, test2.getUniqueId().compare("5w19fs"));
    Assert::AreEqual(0, test2.getEventNames().size()); // No events yet
    Assert::AreEqual(0, test2.getInternalNames().size()); // No internals yet
    Assert::AreEqual(0, test2.getSystemTrajectoryNames().size()); // No SystemTrajectories yet
    Assert::AreEqual(0, test2.getTimes().size()); // No Times
    Assert::AreEqual(UType::Point, test2.events.getUncertainty());
    Assert::AreEqual(UType::Point, test2.sysTrajectories.getUncertainty());

    // Set names
    test.setComponentName("Test");
    Assert::AreEqual("Test", test.getComponentName());
    Assert::AreEqual("", test.getUniqueId());
    Assert::AreEqual("", test.getPrognoserName());
    test.setPrognoserName("Test2");
    Assert::AreEqual("Test2", test.getPrognoserName());
    Assert::AreEqual("Test", test.getComponentName());
    Assert::AreEqual("", test.getUniqueId());
    test.setUniqueId("Test3");
    Assert::AreEqual("Test3", test.getUniqueId());
    Assert::AreEqual("Test2", test.getPrognoserName());
    Assert::AreEqual("Test", test.getComponentName());

    // Set Uncertainty for all
    test.setUncertainty(UType::MeanCovar);
    Assert::AreEqual(UType::MeanCovar, test.events.getUncertainty());
    Assert::AreEqual(UType::MeanCovar, test.sysTrajectories.getUncertainty());

    // Adding events
    test.addEvent("Test Event");
    Assert::AreEqual(1, test.getEventNames().size()); // 1 Event
    test.addEvent("Test Event 2", "Description of said event");
    Assert::AreEqual(2, test.getEventNames().size()); // 2 Events
    std::vector<std::string> extraEvents;
    extraEvents.push_back("Test Event 3");
    extraEvents.push_back("Test Event 4");
    test.addEvents(extraEvents);
    Assert::AreEqual(4, test.getEventNames().size()); // 4 Events

    // Adding SystemTrajectories
    test.addSystemTrajectory("Test SystemTrajectory");
    Assert::AreEqual(1, test.getSystemTrajectoryNames().size()); // 1 SystemTrajectory
    test.addSystemTrajectory("Test SystemTrajectory 2", "Description of said SystemTrajectory");
    Assert::AreEqual(2, test.getSystemTrajectoryNames().size()); // 2 SystemTrajectories
    std::vector<std::string> extraSystemTrajectories;
    extraSystemTrajectories.push_back("Test SystemTrajectory 3");
    extraSystemTrajectories.push_back("Test SystemTrajectory 4");
    test.addSystemTrajectories(extraSystemTrajectories);
    Assert::AreEqual(4, test.getSystemTrajectoryNames().size()); // 4 SystemTrajectories

    // Adding Internals
    test.addInternal("Test Internal");
    Assert::AreEqual(1, test.getInternalNames().size()); // 1 Internal

    std::vector<std::string> extraInternals;
    extraInternals.push_back("Test Internal 3");
    extraInternals.push_back("Test Internal 4");
    test.addInternals(extraInternals);
    Assert::AreEqual(3, test.getInternalNames().size()); // 3 Internals

    // Setup Occurrence Matrix
    test.setupOccurrence(1000);
    Assert::AreEqual(1000, test.events["Test Event"].occurrenceMatrix[0].size());

    // Setup Future Times
    test.setPredictions(10, 5); // 5 intervals, 10s apart
    Assert::AreEqual(5, test.events.getNTimes());
    Assert::AreEqual(5, test.sysTrajectories.getNTimes());
    Assert::AreEqual(6, test.events["Test Event"].occurrenceMatrix.size());
    Assert::AreEqual(50, test.getTimes()[5], 1e-12);

    std::vector<double> t;
    t.push_back(1.7);
    t.push_back(2.1);
    test.setPredictions(t);
    Assert::AreEqual(2, test.events.getNTimes());
    Assert::AreEqual(2, test.sysTrajectories.getNTimes());
    Assert::AreEqual(2.1, test.getTimes()[2], 1e-12);
}
