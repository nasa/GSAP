// Copyright (c) 2017-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "GaussianVariable.h"
#include "Test.h"
#include <sstream>

using namespace PCOE;
using namespace PCOE::Test;

namespace GaussianVariableTests {
    void ctorSpecified() {
        GaussianVariable test1(0, 0);
        GaussianVariable test2(INFINITY, INFINITY);
        GaussianVariable test3(123, 456);
    }

    void ctorUnspecified() {
        try {
            GaussianVariable test;
            (void)test;
        }
        catch (...) {
            Assert::Fail("Failed to contruct Gaussian Variable");
        }
    }

    void generateSamplesDirect() {
        GaussianVariable test;
        test.generatesamplesdirect(0);
        test.generatesamplesdirect(200);
    }

    void generateSamplesICDFUR() {
        GaussianVariable test;
        test.generatesamplesicdfur(0);
        test.generatesamplesicdfur(200);
    }

    void setMeanStd() {
        GaussianVariable test;
        test.setmeanstd(0, 0);
        test.setmeanstd(INFINITY, INFINITY);
        test.setmeanstd(123, 456);
    }

    void evaluatePDF() {
        GaussianVariable test;
        test.evaluatepdf(0);
        test.evaluatepdf(1000);
    }

    void evaluateCDF() {
        GaussianVariable test;
        test.evaluatecdf(0);
        test.evaluatecdf(1000);
    }

    void registerTests(TestContext& context) {
        context.AddTest("Constructor Specified", ctorSpecified, "Gaussian Variable");
        context.AddTest("Constructor Unspecified", ctorUnspecified, "Gaussian Variable");
        context.AddTest("Generate Samples Direct", generateSamplesDirect, "Gaussian Variable");
        context.AddTest("Generate Samples ICDUR", generateSamplesICDFUR, "Gaussian Variable");
        context.AddTest("Set Mean Std", setMeanStd, "Gaussian Variable");
        context.AddTest("Evaluate PDF", evaluatePDF, "Gaussian Variable");
        context.AddTest("Evaluate CDF", evaluateCDF, "Gaussian Variable");
    }
}
