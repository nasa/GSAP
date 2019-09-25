// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "StatisticalTools.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace StatisticalToolsTests {
    void calculateMean() {
        const std::size_t size = 10;
        double arr[size] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        Assert::AreEqual(0.0,
                         calculatemean(arr, size),
                         1e-15,
                         "Array of 0s doesn't have mean of 0");
        for (std::size_t i = 0; i < size; ++i) {
            arr[i] = 10;
        }
        Assert::AreEqual(10.0,
                         calculatemean(arr, size),
                         1e-15,
                         "Array of all 10 doesn't have mean of 10");
        for (std::size_t i = 0; i < size; ++i) {
            arr[i] = i + 1;
        }
        Assert::AreEqual(5.5, calculatemean(arr, size), 1e-15, "Average incorrectly calculated");
    }

    void calculateStDv() {
        const std::size_t size = 20;
        double arr[size] = {0};
        Assert::AreEqual(0.0,
                         calculatestdv(arr, size),
                         1e-15,
                         "Standard deviation of array of 0s is not 0");

        for (std::size_t i = 0; i < size; ++i) {
            arr[i] = 10;
        }
        Assert::AreEqual(0.0,
                         calculatestdv(arr, size),
                         1e-15,
                         "Standard deviation of array of same elements is not 0");

        for (std::size_t i = 0; i < size; ++i) {
            arr[i] = i + 1;
        }
        Assert::AreEqual(5.76628,
                         calculatestdv(arr, size),
                         0.0001,
                         "Standard deviation incorrectly calculated");
    }

    void calculateCDF() {
        const std::size_t size = 10;
        double arr[size] = {0};
        Assert::AreEqual(0, calculatecdf(arr, size, 0), 1e-15, "CDF calculation incorrect");
        Assert::AreEqual(1, calculatecdf(arr, size, 10), 1e-15, "CDF calculation incorrect");
    }

    void registerTests(TestContext& context) {
        context.AddTest("Calculate Mean", calculateMean, "Statistical Tools");
        context.AddTest("Calculate Standard Deviation", calculateStDv, "Statistical Tools");
        context.AddTest("Calculate CDF", calculateCDF, "Statistical Tools");
    }
}
