//
//  BatteryPrognoserTests.h
//
//  Created by Matthew Daigle on 3/17/2016.
//  Copyright (c) 2016 NASA Diagnostics and Prognostics Group. All rights reserved.
//

#ifndef BATTERYPROGNOSERTESTS_H
#define BATTERYPROGNOSERTESTS_H

#include "ModelBasedPrognoser.h"

void batteryPrognoserInit();

// BatteryPrognoser tests
PCOE::ModelBasedPrognoser* createBatteryPrognoser();
void testBatteryPrognoserConstruction();
void testBatteryPrognoserStep();

#endif
