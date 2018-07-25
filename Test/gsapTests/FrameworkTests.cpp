//
//  FrameworkTests.cpp
//  UnitTestAll
//
//  Created by Chris Teubert on 4/6/16.
//  Copyright (c) 2018 United States Government as represented by the Administrator of the National
//  Aeronautics and Space Administration.  All Rights Reserved.
//

#include <string>
#include <vector>

#include "FrameworkTests.h"
#include "GSAPConfigMap.h"
#include "ProgManager.h"
#include "PrognoserFactory.h"
#include "Test.h"
#include "TestPrognoser.h"

using namespace PCOE;
using namespace PCOE::Test;

void PrognoserFactoryTest() {
    PrognoserFactory& theFactory = PrognoserFactory::instance();
    theFactory.Register<TestPrognoser>("Test");
}
