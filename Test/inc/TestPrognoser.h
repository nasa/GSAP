//
//  TestPrognoser.h
//  Generic Infrastructure Example
//
//  Created by Chris Teubert on 4/18/16.
//  Copyright (c) 2018 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//

#ifndef GSAP_TESTPROGNOSER_H
#define GSAP_TESTPROGNOSER_H

#include "Prognoser.h"
#include "ThreadSafeLog.h"

class TestPrognoser: public PCOE::Prognoser {
 public:
    TestPrognoser(PCOE::ConfigMap & paramMap);
    Prediction step(std::map<MessageId, Datum<double> > data) override;
};
#endif
