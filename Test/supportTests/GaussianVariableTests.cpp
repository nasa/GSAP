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
    try {
        GaussianVariable test1(0, 0);
    }
    catch (...) {
        std::stringstream ss;
        ss << "Constructor threw an exception with (" <<
           0 << ", " << 0 << ")";
        Assert::Fail(ss.str());
    }

    try {
        GaussianVariable test2(INFINITY, INFINITY);
    }
    catch (...) {
        std::stringstream ss;
        ss << "Constructor threw an exception with (" <<
           INFINITY << ", " << INFINITY << ")";
        Assert::Fail(ss.str());
    }

    try {
        GaussianVariable test3(123, 456);
    }
    catch (...) {
        std::stringstream ss;
        ss << "Constructor threw an exception with (" <<
           123 << ", " << 456 << ")";
        Assert::Fail(ss.str());
    }
}

void ctorUnspecified() {
    try {
        GaussianVariable test = GaussianVariable();
    }
    catch (...) {
        std::stringstream ss;
        ss << "Default constructor threw exception.";
        Assert::Fail(ss.str());
    }
}

void generateSamplesDirect() {
    GaussianVariable test = GaussianVariable();
    try {
        test.generatesamplesdirect(0);
    }
    catch (...) {
        std::stringstream ss;
        ss << "generatesamplesdirect() threw exception with 0";
        Assert::Fail(ss.str());
    }

    try {
        test.generatesamplesdirect(200);
    }
    catch (...) {
        std::stringstream ss;
        ss << "generatesamplesdirect() threw exception with 200";
        Assert::Fail(ss.str());
    }
}

void generateSamplesICDFUR() {
    GaussianVariable test = GaussianVariable();
    try {
        test.generatesamplesicdfur(0);
    }
    catch (...) {
        std::stringstream ss;
        ss << "generatesamplesicdfur() threw exception with 0";
        Assert::Fail(ss.str());
    }

    try {
        test.generatesamplesicdfur(200);
    }
    catch (...) {
        std::stringstream ss;
        ss << "generatesamplesicdfur() threw exception with 200";
        Assert::Fail(ss.str());
    }
}

void setMeanStd() {
    GaussianVariable test = GaussianVariable();
    try {
        test.setmeanstd(0, 0);
    }
    catch (...) {
        std::stringstream ss;
        ss << "setmeanstd() threw exception with (0, 0)";
        Assert::Fail(ss.str());
    }

    try {
        test.setmeanstd(INFINITY, INFINITY);
    }
    catch (...) {
        std::stringstream ss;
        ss << "setmeanstd() threw exception with (INFINITY, INFINITY)";
        Assert::Fail(ss.str());
    }

    try {
        test.setmeanstd(123, 456);
    }
    catch (...) {
        std::stringstream ss;
        ss << "setmeanstd() threw exception with (123, 456)";
        Assert::Fail(ss.str());
    }
}

void evaluatePDF() {
    GaussianVariable test = GaussianVariable();
    try {
        test.evaluatepdf(0);
    }
    catch (...) {
        std::stringstream ss;
        ss << "evaluatepdf() threw exception with mu = 0, x = 0";
        Assert::Fail(ss.str());
    }

    try {
        test.evaluatepdf(1000);
    }
    catch (...) {
        std::stringstream ss;
        ss << "evaluatepdf() threw exception with mu = 0, x = 1000";
        Assert::Fail(ss.str());
    }
}

void evaluateCDF() {
    GaussianVariable test = GaussianVariable();
    try {
        test.evaluatecdf(0);
    }
    catch (...) {
        std::stringstream ss;
        ss << "evaluatecdf() threw exception with mu = 0, x = 0";
        Assert::Fail(ss.str());
    }

    try {
        test.evaluatecdf(1000);
    }
    catch (...) {
        std::stringstream ss;
        ss << "evaluatecdf() threw exception with mu = 0, x = 1000";
        Assert::Fail(ss.str());
    }
}

