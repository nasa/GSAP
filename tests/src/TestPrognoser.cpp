// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <string>

#include "TestPrognoser.h"

using namespace PCOE;

const std::string MODULE_NAME = "TestPrognoser";

TestPrognoser::TestPrognoser(PCOE::ConfigMap& paramMap) {
    log.WriteLine(LOG_DEBUG, MODULE_NAME, "Creating");

    if (paramMap.hasKey("futureLoading")) {
        log.FormatLine(LOG_DEBUG,
                       MODULE_NAME,
                       "Received Future Loading: %s",
                       paramMap.getVector("futureLoading")[0].c_str());
    }
}

Prediction TestPrognoser::step(std::map<MessageId, Datum<double>>) {
    return Prediction::EmptyPrediction();
}
