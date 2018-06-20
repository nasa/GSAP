//
//  GaussianVariableTests.cpp
//  UnitTestAll
//
//  Created by Julian Vu on 2/28/17.

#include "GaussianVariableTests.h"
#include "GaussianVariable.h"
#include "Test.h"
#include <sstream>

using namespace PCOE;
using namespace PCOE::Test;

void ctorSpecified() {
    GaussianVariable test1(0, 0);
    GaussianVariable test2(INFINITY, INFINITY);
    GaussianVariable test3(123, 456);
}

void ctorUnspecified() {
    GaussianVariable test = GaussianVariable();
}

void generateSamplesDirect() {
    GaussianVariable test = GaussianVariable();
    test.generatesamplesdirect(0);
    test.generatesamplesdirect(200);
}

void generateSamplesICDFUR() {
    GaussianVariable test = GaussianVariable();
    test.generatesamplesicdfur(0);
    test.generatesamplesicdfur(200);
}

void setMeanStd() {
    GaussianVariable test = GaussianVariable();
    test.setmeanstd(0, 0);
    test.setmeanstd(INFINITY, INFINITY);
    test.setmeanstd(123, 456);
}

void evaluatePDF() {
    GaussianVariable test = GaussianVariable();
    test.evaluatepdf(0);
    test.evaluatepdf(1000);
}

void evaluateCDF() {
    GaussianVariable test = GaussianVariable();
    test.evaluatecdf(0);
    test.evaluatecdf(1000);
}

