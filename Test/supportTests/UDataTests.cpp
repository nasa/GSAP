/**  Unit Test functions for UData- Body
 *   @file      Unit Testing functions for UData
 *
 *   @brief     A set of functions to be used with UnitTester. These functions
 *              test all the aspects of UData
 *
 *   @ingroup   UData
 *
 *   @author    Chris Teubert <christopher.a.teubert@nasa.gov>
 *   @author    Jason Watkins <jason-watkins@outlook.com>
 *   @version   0.2.0
 *   @date      March 5, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#include <chrono>
#include <thread>

#include "Test.h"
#include "UData.h"
#include "UDataTests.h"

using namespace PCOE;
using namespace PCOE::Test;

static std::vector<double> getTestVec() {
    std::vector<double> a;
    a.push_back(1.3);
    a.push_back(4.4);
    a.push_back(5.5);
    a.push_back(2.1);
    a.push_back(3.3);
    a.push_back(6.2);
    return a;
}

namespace TestUData {
    void construct_default() {
        try {
            UData ud;
        }
        catch (...) {
            Assert::Fail("Constructor threw an exception");
        }
    }

    void construct_type() {
        try {
            UData ud(UType::Point);
        }
        catch (...) {
            Assert::Fail("UType::Point constructor threw an exception");
        }

        try {
            UData ud(UType::MeanSD);
        }
        catch (...) {
            Assert::Fail("UType::MeanSD constructor threw an exception");
        }

        try {
            UData ud(UType::MeanCovar);
        }
        catch (...) {
            Assert::Fail("UType::MeanCovar constructor threw an exception");
        }

        try {
            UData ud(UType::Samples);
        }
        catch (...) {
            Assert::Fail("UType::Samples constructor threw an exception");
        }

        try {
            UData ud(UType::WSamples);
        }
        catch (...) {
            Assert::Fail("WUType::Samples constructor threw an exception");
        }
    }

    void construct_copy() {
        UData ud1(UType::MeanSD);
        UData ud2(ud1);
        Assert::AreEqual(ud1.dist(), ud2.dist(), "Distributions are not equal");
        Assert::AreEqual(ud1.size(), ud2.size(), "Sizes are not equal");
        Assert::AreEqual(ud1.uncertainty(), ud2.uncertainty(), "Uncertainty types are not equal");
        Assert::AreEqual(ud1.npoints(), ud2.npoints(), "NPoints are not equal");
        Assert::AreEqual(ud1.valid(), ud2.valid(), "Validities are not equal");
        Assert::AreEqual(ud1.updated(), ud2.updated(), "Times are not equal");
    }

    void construct_move() {
        UData ud1(UType::MeanSD);
        auto dist1 = ud1.dist();
        auto size1 = ud1.size();
        auto uncertainty1 = ud1.uncertainty();
        auto npoints1 = ud1.npoints();
        auto validity1 = ud1.valid();
        auto time1 = ud1.updated();

        UData ud2 = std::move(ud1);

        Assert::AreEqual(dist1, ud2.dist(), "Distributions are not equal");
        Assert::AreEqual(size1, ud2.size(), "Sizes are not equal");
        Assert::AreEqual(uncertainty1, ud2.uncertainty(), "Uncertainty types are not equal");
        Assert::AreEqual(npoints1, ud2.npoints(), "NPoints are not equal");
        Assert::AreEqual(validity1, ud2.valid(), "Validities are not equal");
        Assert::AreEqual(time1, ud2.updated(), "Times are not equal");
    }

    void operator_assign() {
        UData ud1(UType::MeanCovar);
        auto dist1 = ud1.dist();
        auto size1 = ud1.size();
        auto uncertainty1 = ud1.uncertainty();
        auto npoints1 = ud1.npoints();
        auto validity1 = ud1.valid();
        auto time1 = ud1.updated();

        UData ud2 = ud1;

        Assert::AreEqual(dist1, ud2.dist(), "Distributions are not equal");
        Assert::AreEqual(size1, ud2.size(), "Sizes are not equal");
        Assert::AreEqual(uncertainty1, ud2.uncertainty(), "Uncertainty types are not equal");
        Assert::AreEqual(npoints1, ud2.npoints(), "NPoints are not equal");
        Assert::AreEqual(validity1, ud2.valid(), "Validities are not equal");
        Assert::AreEqual(time1, ud2.updated(), "Times are not equal");
    }

    void operator_equal() {
        UData ud1;
        UData ud2;
        UData ud3(UType::MeanSD);
        UData ud4(UType::MeanSD);
        ud3.npoints(2);

        Assert::IsTrue(ud1 == ud2, "Default constructed objects not equal");
        Assert::IsFalse(ud1 == ud3, "Objects of different types are equal");
        Assert::IsFalse(ud3 == ud4, "Objects of different sizes are equal");

        ud2.set(getTestVec());
        UData ud5 = ud2;
        ud5[0] = 42.31;
        Assert::IsFalse(ud1 == ud2, "Object with data is equal to default object");
        Assert::IsFalse(ud2 == ud5, "Objects with different data are equal");
    }

    void operator_notequal() {
        UData ud1;
        UData ud2;
        UData ud3(UType::MeanSD);
        UData ud4(UType::MeanSD);
        ud3.npoints(2);

        Assert::IsFalse(ud1 != ud2, "Default constructed objects not equal");
        Assert::IsTrue(ud1 != ud3, "Objects of different types are equal");
        Assert::IsTrue(ud3 != ud4, "Objects of different sizes are equal");

        ud2.set(getTestVec());
        UData ud5 = ud2;
        ud5[0] = 42.31;
        Assert::IsTrue(ud1 != ud2, "Object with data is equal to default object");
        Assert::IsTrue(ud2 != ud5, "Objects with different data are equal");
    }

    void dist() {
        UData ud;
        ud.dist(DIST_GAUSSIAN);
        Assert::AreEqual(DIST_GAUSSIAN, ud.dist(), "Gaussian dist does not match");
        ud.dist(DIST_UNIFORM);
        Assert::AreEqual(DIST_UNIFORM, ud.dist(), "Uniform dist does not match");
    }

    void npoints() {
        UData ud;
        ud.npoints(0);
        Assert::AreEqual(0, ud.npoints(), "npoints not equal after setting 0");
        ud.npoints(2);
        Assert::AreEqual(2, ud.npoints(), "npoints not equal after setting 0");
    }

    void uncertainty() {
        UData ud;
        ud.uncertainty(UType::Point);
        Assert::AreEqual(UType::Point, ud.uncertainty(), "UType::Point uncertainty does not match");
        ud.uncertainty(UType::MeanSD);
        Assert::AreEqual(UType::MeanSD,
                         ud.uncertainty(),
                         "UType::MeanSD uncertainty does not match");
    }

    void updated() {
        using size_type = UData::size_type;

        UData ud;
        size_type start = ud.updated();
        Assert::AreEqual(0, ud.updated(), "Default updated time not 0");

        // Need short pause between every updated() call for IDEs that run test too quickly
        std::chrono::milliseconds ms(1); // Declare and initialize one millisecond variable
        std::this_thread::sleep_for(ms); // Pause for 1 ms

        ud.set(7);
        size_type doubleTime = ud.updated();
        Assert::IsTrue(doubleTime > start, "updated not changed after setting double [0]");

        std::this_thread::sleep_for(ms); // Pause for 1 ms

        ud.set(std::make_pair(7, 11));
        size_type pairTime = ud.updated();
        Assert::IsTrue(pairTime > doubleTime, "updated not changed after setting pair [0]");

        std::this_thread::sleep_for(ms); // Pause for 1 ms

        ud.set({7, 11, 19});
        size_type vecTime = ud.updated();
        Assert::IsTrue(vecTime > pairTime, "updated not changed after setting vector [0]");

        std::this_thread::sleep_for(ms); // Pause for 1 ms

        ud[0] = 13;
        doubleTime = ud.updated();
        Assert::IsTrue(doubleTime > vecTime, "updated not changed after setting double [1]");

        std::this_thread::sleep_for(ms); // Pause for 1 ms

        ud[0] = std::make_pair(13, 17);
        pairTime = ud.updated();
        Assert::IsTrue(pairTime > doubleTime, "updated not changed after setting pair [1]");

        std::this_thread::sleep_for(ms); // Pause for 1 ms

        ud[0] = {13, 17, 23};
        vecTime = ud.updated();
        Assert::IsTrue(vecTime > pairTime, "updated not changed after setting vector [1]");
    }

    void valid() {
        UData ud;
        Assert::IsFalse(ud.valid(), "Default object is valid");
        ud.set(1);
        Assert::IsTrue(ud.valid(), "Not valid after double assignment");

        ud = UData();
        Assert::IsFalse(ud.valid(), "Default object is valid");
        ud.set(getTestVec());
        Assert::IsTrue(ud.valid(), "Not valid after vector assignment");
    }

    void size() {
        UData ud(UType::Point);
        Assert::AreEqual(1, ud.size(), "Unexpected point size");

        ud.uncertainty(UType::MeanSD);
        Assert::AreEqual(2, ud.size(), "Unexpected MeanSD size");
    }

    void operator_not() {
        UData ud;
        Assert::IsTrue(!ud, "Valid after default construction");
        ud.npoints(1);
        Assert::IsTrue(!ud, "Valid after resize only");
        ud.set(0);
        Assert::IsFalse(!ud, "Not valid after assignment");
    }

    void iterators_forward() {
        UData ud(UType::MeanCovar);
        ud.npoints(3);
        std::vector<double> values = {1.2, 3.4, 4.5, 5.6};
        ud.setVec(values);
        UData::iterator i = ud.begin();
        std::vector<double>::iterator j = values.begin();
        for (; i != ud.end() && j != values.end(); ++i, ++j) {
            Assert::AreEqual(*j, *i, 1e-12, "Unexpexted non-equal value in data");
        }

        UData::const_iterator x = ud.cbegin();
        std::vector<double>::const_iterator y = values.cbegin();
        for (; x != ud.cend() && y != values.cend(); ++x, ++y) {
            Assert::AreEqual(*y, *x, 1e-12, "Unexpexted non-equal value in data");
        }
    }

    void iterators_reverse() {
        UData ud(UType::MeanCovar);
        ud.npoints(3);
        std::vector<double> values = {1.2, 3.4, 4.5, 5.6};
        ud.setVec(values);
        UData::reverse_iterator i = ud.rbegin();
        std::vector<double>::reverse_iterator j = values.rbegin();
        for (; i != ud.rend() && j != values.rend(); ++i, ++j) {
            Assert::AreEqual(*j, *i, 1e-12, "Unexpexted non-equal value in data");
        }

        UData::const_reverse_iterator x = ud.crbegin();
        std::vector<double>::const_reverse_iterator y = values.crbegin();
        for (; x != ud.crend() && y != values.crend(); ++x, ++y) {
            Assert::AreEqual(*y, *x, 1e-12, "Unexpexted non-equal value in data");
        }
    }

    void access_double() {
        UData ud;

        Assert::IsTrue(std::isnan(ud.get()), "Default constructed object contains non-NaN value");

        ud.set(1.0);
        Assert::AreEqual(1.0, ud.get(), 1e-12, "Default getter failed [0]");
        Assert::AreEqual(1.0, ud.get(0), 1e-12, "Indexed getter failed [0]");
        Assert::AreEqual(1.0, ud[0], 1e-12, "Index operator failed [0]");

        ud.uncertainty(UType::MeanSD);
        ud.set(1, 7.0);
        Assert::AreNotEqual(7.0, ud.get(), 1e-12, "Default getter failed [1]");
        Assert::AreNotEqual(7.0, ud.get(0), 1e-12, "Indexed getter failed [1]");
        Assert::AreNotEqual(7.0, ud[0], 1e-12, "Index operator failed [1]");
        Assert::AreEqual(7.0, ud.get(1), 1e-12, "Indexed getter failed [2]");
        Assert::AreEqual(7.0, ud[1], 1e-12, "Index operator failed [2]");
    }

    void access_pair() {
        UData ud(UType::Point);

        try {
            ud.getPair();
            Assert::Fail("Pair returned when ud should only have one data element");
        }
        catch (...) {
        }

        ud.uncertainty(UType::MeanSD);
        ud.set(std::pair<double, double>({1.0, 2.0}));
        auto p = ud.getPair();
        Assert::AreEqual(1.0, p.first, 1e-12, "Unexpected first element");
        Assert::AreEqual(2.0, p.second, 1e-12, "Unexpected second element");
    }

    void access_vector() {
        UData ud;

        auto vec1 = ud.getVec();
        auto vec2 = ud.getVec(1);
        Assert::AreEqual(1, vec1.size() - vec2.size(), "Unexpected vector sizes");

        ud.uncertainty(UType::MeanSD);
        ud.set(std::vector<double>({1.0, 2.0}));
        vec1 = ud.getVec();
        Assert::AreEqual(2, vec1.size(), "Unexpected UType::MeanSD vector size");
        Assert::AreEqual(1.0, vec1[0], 1e-12, "Unexpected first element");
        Assert::AreEqual(2.0, vec1[1], 1e-12, "Unexpected second element");
    }

    void point() {
        UData ud(UType::Point);
        Assert::AreEqual(UType::Point, ud.uncertainty(), "Unexpected type");
        Assert::AreEqual(1, ud.size(), "Unexpected size");
        Assert::AreEqual(1, ud.npoints(), "Unexpected npoints");

        ud[VALUE] = 3.433;
        UData::size_type update1 = ud.updated();
        Assert::AreEqual(3.433, ud.get(VALUE), 1e-12, "Unexpected value using indexer");
        Assert::IsTrue(update1 > 0, "Time not updated on first insert");
        Assert::IsTrue(ud.valid(), "Not valid after first insert");

        ud.set(7.35);
        UData::size_type update2 = ud.updated();
        Assert::AreEqual(7.35, ud.get(VALUE), 1e-12, "Unexpected value using set");
        Assert::IsTrue(update2 - update1 > 0, "Time not updated on second insert");
        Assert::IsTrue(ud.valid(), "Not valid after second insert");

        // Setting another value should reset valid to true
        ud.invalidate();
        Assert::IsFalse(ud.valid(), "Valid after invalidation");
        ud.set(7.53);
        Assert::AreEqual(7.53, ud.get(VALUE), 1e-12, "Unexpected value after reseting");
        Assert::IsTrue(ud.valid(), "Not valid after third insertion");

        ud.set(VALUE, 9);
        Assert::AreEqual(9, ud.get(VALUE), 1e-12, "Unexpected value after indexed set");

        auto vec = ud.getVec();
        Assert::AreEqual(1, vec.size(), "Unexpected vector size for UType::Point");
        Assert::AreEqual(ud.get(VALUE), vec[0], 1e-12, "Vector value not equal");

        try {
            ud.getPair();
            Assert::Fail("Got pair from UType::Point without throwing");
        }
        catch (...) {
        }
    }

    void meanSD() {
        UData ud(UType::MeanSD);
        Assert::AreEqual(UType::MeanSD, ud.uncertainty(), "Unexpected type");
        Assert::AreEqual(2, ud.size(), "Unexpected size");
        Assert::AreEqual(1, ud.npoints(), "Unexpected npoints");

        ud[MEAN] = 3.433;
        Assert::AreEqual(3.433, ud[MEAN], 1e-12, "Unexpected value from mean indexer");

        ud.set(7);
        Assert::AreEqual(7, ud.get(), 1e-12, "Unexpected value from mean default get/set");

        ud.set(MEAN, 9);
        Assert::AreEqual(9, ud.get(MEAN), 1e-12, "Unexpected value from mean get/set");

        ud[SD] = 0;
        Assert::AreEqual(0, ud[SD], 1e-12, "Unexpected value from SD indexer");

        ud.set(SD, 9.5);
        Assert::AreEqual(9.5, ud.get(SD), 1e-12, "Unexpected value from SD get/set");

        // Pair Tests
        ud.set(std::pair<double, double>(1.3, 2.0));
        Assert::AreEqual(1.3, ud[MEAN], 1e-12, "Unexpected mean value from pair [0]");
        Assert::AreEqual(2.0, ud[SD], 1e-12, "Unexpected SD value from pair [0]");

        std::pair<double, double> pair = ud.getPair();
        Assert::AreEqual(1.3, pair.first, 1e-12, "Unexpected mean value from pair [1]");
        Assert::AreEqual(2.0, pair.second, 1e-12, "Unexpected SD value from pair [1]");

        pair = std::make_pair(7.0, 5.0);
        ud.setPair(pair);
        Assert::AreEqual(7.0, ud[MEAN], 1e-12, "Unexpected mean value from pair [2]");
        Assert::AreEqual(5.0, ud[SD], 1e-12, "Unexpected SD value from pair [2]");

        // Vector Tests
        ud.set(std::vector<double>({1.3, 2.0}));
        Assert::AreEqual(1.3, ud[MEAN], 1e-12, "Unexpected mean value from vec [0]");
        Assert::AreEqual(2.0, ud[SD], 1e-12, "Unexpected SD value from vec [0]");

        std::vector<double> vec = ud.getVec();
        Assert::AreEqual(1.3, vec[0], 1e-12, "Unexpected mean value from vec [1]");
        Assert::AreEqual(2.0, vec[1], 1e-12, "Unexpected SD value from vec [1]");

        vec = std::vector<double>({7.0, 5.0});
        ud.setVec(vec);
        Assert::AreEqual(7.0, ud[MEAN], 1e-12, "Unexpected mean value from vec [2]");
        Assert::AreEqual(5.0, ud[SD], 1e-12, "Unexpected SD value from vec [2]");
    }

    void meanCovar() {
        // Setup
        UData ud(UType::MeanCovar);
        Assert::AreEqual(UType::MeanCovar, ud.uncertainty(), "Unexpected type");
        Assert::AreEqual(2, ud.size(), "Unexpected size");
        Assert::AreEqual(1, ud.npoints(), "Unexpected npoints");

        ud.npoints(5);
        Assert::AreEqual(6, ud.size());
        Assert::AreEqual(5, ud.npoints());

        for (size_t i = 0; i < ud.npoints(); i++) {
            ud[COVAR(i)] = i * 7.0;
            Assert::AreEqual(i * 7.0, ud[COVAR(i)], 1e-12, "Unexpected value using indexer");

            ud.set(COVAR(i), i * 5.0);
            Assert::AreEqual(i * 5.0, ud.get(COVAR(i)), 1e-12, "Unexpected value using get/set");
        }

        // Pair Tests
        auto pair1 = std::make_pair(5.0, 7.0);
        auto pair2 = std::make_pair(3.0, 9.0);
        ud.setPair(pair1);
        ud.setPair(2, pair2);
        Assert::AreEqual(pair1.first, ud.get(0), 1e-12, "Unexpected value at index 0 using pair");
        Assert::AreEqual(pair1.second, ud.get(1), 1e-12, "Unexpected value at index 1 using pair");
        Assert::AreEqual(pair2.first, ud.get(2), 1e-12, "Unexpected value at index 3 using pair");
        Assert::AreEqual(pair2.second, ud.get(3), 1e-12, "Unexpected value at index 4 using pair");

        Assert::AreEqual(pair1, ud.getPair(), "Unexpected value for first pair");
        Assert::AreEqual(pair2, ud.getPair(2), "Unexpected value for second pair");

        // Vector Tests
        std::vector<double> vec = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
        ud.setVec(vec);
        for (size_t i = 0; i < ud.npoints(); i++) {
            Assert::AreEqual(vec[i], ud.get(i), 1e-12, "Unexpected value when setting vector");
        }
        Assert::AreEqual(vec, ud.getVec(), "Unexpected value when getting vector");
    }

    void percentiles() {
        UData ud;
        ud.uncertainty(UType::Percentiles);
        Assert::AreEqual(UType::Percentiles, ud.uncertainty(), "Unexpected type");
        Assert::AreEqual(2, ud.size(), "Unexpected size [0]");
        Assert::AreEqual(1, ud.npoints(), "Unexpected npoints [0]");

        ud.npoints(5);
        Assert::AreEqual(10, ud.size(), "Unexpected size [1]");
        Assert::AreEqual(5, ud.npoints(), "Unexpected npoints [1]");

        ud[PVALUE(0)] = 3.433;
        Assert::AreEqual(3.433, ud[PVALUE(0)], 1e-12);
        ud.set(7);
        Assert::AreEqual(7, ud[PVALUE(0)], 1e-12);
        ud[PERCENTILE(0)] = 0.1;
        Assert::AreEqual(0.1, ud[PERCENTILE(0)], 1e-12);
        Assert::AreEqual(7.0, ud[PVALUE(0)], 1e-12);
        ud[PVALUE(4)] = 15.0;
        Assert::AreEqual(15.0, ud[PVALUE(4)], 1e-12);
        ud.set(PVALUE(0), 9);
        Assert::AreEqual(9, ud[PVALUE(0)], 1e-12);
        Assert::AreEqual(ud.get(), ud[PVALUE(0)], 1e-12);
        ud.set(PVALUE(1), 9.5);
        Assert::AreEqual(9.5, ud[PVALUE(1)], 1e-12);
        Assert::AreEqual(ud.get(PVALUE(1)), ud[PVALUE(1)], 1e-12);
        ud.set(std::pair<double, double>(1.3, 0.5));
        Assert::AreEqual(0.5, ud[PERCENTILE(0)], 1e-12);
        Assert::AreEqual(1.3, ud[PVALUE(0)], 1e-12);
        ud.set(PAIR(3), std::pair<double, double>(1.9, 0.25));
        Assert::AreEqual(0.25, ud[PERCENTILE(3)], 1e-12);
        Assert::AreEqual(1.9, ud[PVALUE(3)], 1e-12);
        std::size_t size = ud.getVec().size();
        Assert::AreEqual(10, size);
    }

    void samples() {
        UData ud(UType::Samples);
        Assert::AreEqual(UType::Samples, ud.uncertainty(), "Unexpected type");
        Assert::AreEqual(1, ud.size(), "Unexpected size [0]");
        Assert::AreEqual(1, ud.npoints(), "Unexpected npoints [0]");

        ud.npoints(5);
        Assert::AreEqual(5, ud.size(), "Unexpected size [1]");
        Assert::AreEqual(5, ud.npoints(), "Unexpected npoints [1]");

        ud[0] = 3.433;
        Assert::AreEqual(3.433, ud[0], 1e-12);
        ud.set(7);
        Assert::AreEqual(7, ud[0], 1e-12);
        ud[1] = 0;
        Assert::AreEqual(0, ud[1], 1e-12);
        ud[4] = 15.0;
        Assert::AreEqual(15.0, ud[4], 1e-12);
        ud.set(0, 9);
        Assert::AreEqual(9, ud[0], 1e-12);
        ud.set(1, 9.5);
        Assert::AreEqual(9.5, ud[1], 1e-12);
        ud.set(std::pair<double, double>(1.3, 2.0));
        Assert::AreEqual(1.3, ud[0], 1e-12);
        Assert::AreEqual(2.0, ud[1], 1e-12);
        ud.set(1, std::pair<double, double>(1.3, 2.0));
        Assert::AreEqual(1.3, ud[0], 1e-12);
        Assert::AreEqual(1.3, ud[1], 1e-12);
        Assert::AreEqual(2.0, ud[2], 1e-12);

        ud.set(getTestVec());
        Assert::AreEqual(1.3, ud[0], 1e-12);
        Assert::AreEqual(4.4, ud[1], 1e-12);
        Assert::AreEqual(5.5, ud[2], 1e-12);
        Assert::AreEqual(2.1, ud[3], 1e-12);
        Assert::AreEqual(3.3, ud[4], 1e-12);

        std::vector<double> testVec = ud.getVec();
        Assert::AreEqual(1.3, testVec[0], 1e-12);
        Assert::AreEqual(4.4, testVec[1], 1e-12);
        Assert::AreEqual(5.5, testVec[2], 1e-12);
        Assert::AreEqual(2.1, testVec[3], 1e-12);
        Assert::AreEqual(3.3, testVec[4], 1e-12);

        ud[4] = 8.5;
        Assert::AreEqual(8.5, ud[4], 1e-12);
        ud.set(getTestVec());
        Assert::AreEqual(3.3, ud[4], 1e-12);

        testVec[4] = 8.5;
        Assert::AreEqual(3.3, ud[4], 1e-12);
        ud.setVec(testVec);
        Assert::AreEqual(8.5, ud[4], 1e-12);

        std::size_t size = ud.getVec().size();
        Assert::AreEqual(5, size);

        Assert::AreEqual(ud.get(1), ud[1], 1e-12);
        Assert::AreEqual(ud.get(), ud[0], 1e-12);
    }

    void wSamples() {
        UData ud(UType::WSamples);
        Assert::AreEqual(UType::WSamples, ud.uncertainty(), "Unexpected type");
        Assert::AreEqual(2, ud.size(), "Unexpected size [0]");
        Assert::AreEqual(1, ud.npoints(), "Unexpected npoints [0]");

        ud.npoints(5);
        Assert::AreEqual(10, ud.size(), "Unexpected size [1]");
        Assert::AreEqual(5, ud.npoints(), "Unexpected npoints [1]");

        ud[SAMPLE(0)] = 3.433;
        Assert::AreEqual(3.433, ud[SAMPLE(0)], 1e-12);
        ud.set(7);
        Assert::AreEqual(7, ud[SAMPLE(0)], 1e-12);
        ud[WEIGHT(0)] = 0.1;
        Assert::IsTrue(std::abs(ud[WEIGHT(0)] - 0.1) < 1e-12 &&
                       std::abs(ud[SAMPLE(0)] - 7) < 1e-12);
        ud[SAMPLE(4)] = 15.0;
        Assert::AreEqual(15.0, ud[SAMPLE(4)], 1e-12);
        ud.set(SAMPLE(0), 9);
        Assert::AreEqual(9, ud[SAMPLE(0)], 1e-12);
        Assert::AreEqual(ud.get(), ud[SAMPLE(0)], 1e-12);
        ud.set(SAMPLE(1), 9.5);
        Assert::AreEqual(9.5, ud[SAMPLE(1)], 1e-12);
        Assert::AreEqual(ud.get(SAMPLE(1)), ud[SAMPLE(1)], 1e-12);
        ud.set(std::pair<double, double>(1.3, 0.5));
        Assert::IsTrue(std::abs(ud[SAMPLE(0)] - 1.3) < 1e-12 &&
                       std::abs(ud[WEIGHT(0)] - 0.5) < 1e-12);
        ud.set(PAIR(3), std::pair<double, double>(1.9, 0.25));
        Assert::IsTrue(std::abs(ud[SAMPLE(3)] - 1.9) < 1e-12 &&
                       std::abs(ud[WEIGHT(3)] - 0.25) < 1e-12);
        std::size_t size = ud.getVec().size();
        Assert::AreEqual(10, size);
    }
}
