//
//  StatisticalToolsTests.cpp
//  UnitTestAll
//
//  Created by Julian Vu on 2/14/17.

#include "StatisticalToolsTests.h"
#include "StatisticalTools.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

void calculateMean() {
    size_t size = 10;
    double arr[size] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    Assert::AreEqual(calculatemean(arr, size), 0.0, "Array of 0s doesn't have mean of 0");
    for (size_t i = 0; i < size; ++i) {
        arr[i] = 10;
    }
    Assert::AreEqual(calculatemean(arr, size), 10.0, "Array of all 10 doesn't have mean of 10");
    for (size_t i = 0; i < size; ++i) {
        arr[i] = i + 1;
    }
    Assert::AreEqual(calculatemean(arr, size), 5.5, "Average incorrectly calculated");
    double arr2[size];
    Assert::AreEqual(calculatemean(arr2, size), 0.0, "Empty array doesn't have mean of 0");
}
