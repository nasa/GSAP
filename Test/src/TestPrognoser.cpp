//
//  TestPrognoser.cpp
//  Generic Infrastructure Example
//
//  Created by Chris Teubert on 4/18/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//

#include <string>

#include "TestPrognoser.h"
#include "GSAPConfigMap.h"

using namespace PCOE;

const std::string MODULE_NAME   = "TestPrognoser";

TestPrognoser::TestPrognoser(PCOE::GSAPConfigMap & paramMap):
Prognoser(paramMap) {
    log.WriteLine(LOG_DEBUG, MODULE_NAME, "Creating");

    if (paramMap.hasKey("futureLoading")) {
        log.FormatLine(LOG_DEBUG, MODULE_NAME, "Received Future Loading: %s", paramMap.getVector("futureLoading")[0].c_str());
    }
}

void TestPrognoser::step() {
}

void TestPrognoser::checkResultValidity() {
    log.WriteLine(LOG_INFO, MODULE_NAME, "Checking Result Validity");
}

void TestPrognoser::checkInputValidity() {
    log.WriteLine(LOG_INFO, MODULE_NAME, "Checking Input Validity");
}
