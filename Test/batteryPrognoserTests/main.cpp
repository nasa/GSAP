//
//  runBatteryPrognoserTests.cpp
//  Unit tests for BatteryPrognoser
//
//  Created by Matthew Daigle on 3/8/2016.
//  Copyright (c) 2016 NASA Diagnostics and Prognostics Group. All rights reserved.
//

#include <fstream>
#include <iostream>

#include "Test.h"

// Unit test headers
#include "BatteryPrognoserTests.h"

using namespace PCOE;
using namespace PCOE::Test;

int main() {
    TestContext context;
    context.AddCategoryInitializer("Battery Prognoser", batteryPrognoserInit);
    context.AddTest("Construction", testBatteryPrognoserConstruction, "Battery Prognoser");
    context.AddTest("Step", testBatteryPrognoserStep, "Battery Prognoser");

    int result = context.Execute();
    std::ofstream junit("testresults/batteryPrognoser.xml");
    context.WriteJUnit(junit);
    return result;
}