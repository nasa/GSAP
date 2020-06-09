/**
 * A simple, robust and lightweight unit testing framework.
 *
 * @details
 * This file contains a classes to facilitate running unit test functions. Any
 * function that takes no arguments and has no return value can be used as a
 * test. Tests will fail if they throw or cause an abort. The expectation is
 * that tests will fail as the result of a failed assertion using the
 * @see{Assert} class, which throws @see{AssertFailed} exceptions. Other
 * exceptions and aborts are reported as such and considered unexpected.
 *
 * @details
 * The general usage of this framework is as follows. First, a number of unit
 * tests are written that take no arguments and return nothing. The test methods
 * are expected to use the static methods of the @see{Assert} class to evaluate
 * the expected outcomes of the test.
 *
 * @details
 * With the test cases written, a new @see{TestContext} is created, and tests
 * are added to it. Once the tests are added, calling the Execute method will
 * run all of the tests, outputting the progress of the tests to stdout or
 * another std::ostream specified by calling SetOutput. The number of failed
 * tests is also reported by the return value of Execute to allow for
 * programatic action if some tests fail.
 *
 * @example
 * #include <cstdlib>
 * #include "Test.h"
 * using namespace PCOE::Test;
 * void alway_pass() { } // Any test that doesn't throw or abort passes
 * void alway_fail() { Assert::Fail(); }
 * void abort_fail() { std::abort(); } // Fails, but doesn't crash
 * int main() {
 *     TestContext tests;
 *     tests.AddTest("Pass", always_pass);
 *     tests.AddTest("Fail", alway_fail);
 *     tests.AddTest("Abort", abort_fail);
 *     return tests.Execute();
 * }
 *
 * @copyright
 * Copyright (c) 2016-2020 United States Government as represented by
 * the Administrator of the National Aeronautics and Space
 * Administration. All Rights Reserved.
 */

#ifndef PCOE_TEST_H
#define PCOE_TEST_H

#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace {
    // Note (JW): C++11 compatability. See what we can switch to std types if
    // we ever switch to C++14 or higher
    template <bool B, class T = void>
    using EnableIfT = typename std::enable_if<B, T>::type;

    template <class T>
    using RemoveRefT = typename std::remove_reference<T>::type;
}

namespace PCOE {
    namespace Test {
        /**
         * An exception to be thrown when a unit test assertion fails.
         */
        class AssertFailed : public std::exception {
        public:
            AssertFailed() {}

            explicit AssertFailed(const std::string& what_msg) : message(what_msg) {}

            const char* what() const noexcept override {
                return message.c_str();
            }

        private:
            const std::string message;
        };

        /**
         * Verifies conditions in unit tests using true/false propositions.
         */
        class Assert {
        public:
            /**
             * Verifies that two specified generic type data are equal by using
             * the equality operator. The assertion fails if they are not equal.
             *
             * @param expected The value that the unit test expects.
             * @param actual   The value that the unit test produced.
             * @param message  A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename TE,
                      typename TA,
                      typename = EnableIfT<std::is_convertible<TE, TA>::value>>
            static void AreEqual(const TE& expected,
                                 const TA& actual,
                                 const std::string& message = "") {
                AreEqual(expected, actual, [&]() { return message; });
            }

            /**
             * Verifies that two specified generic type data are equal by using
             * the equality operator. The assertion fails if they are not equal.
             *
             * @param expected The value that the unit test expects.
             * @param actual   The value that the unit test produced.
             * @param msg_fn   A function that produces a string to be
             *                 displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename TE,
                      typename TA,
                      typename = EnableIfT<std::is_convertible<TE, TA>::value>>
            static void AreEqual(const TE& expected,
                                 const TA& actual,
                                 const std::function<std::string()>& msg_fn) {
                if (!(actual == expected)) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that two specified generic type data are almost equal by
             * comparing the magnitude of the difference between @p{expected} and
             * @p{actual} to @p{delta}. The assertion fails if the magnitude of the
             * difference is greater than @p{delta}.
             *
             * @param expected The value that the unit test expects.
             * @param actual   The value that the unit test produced.
             * @param delta    The maximum difference in magnitude.
             * @param message  A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <
                typename TE,
                typename TA,
                typename TD,
                typename = EnableIfT<std::is_convertible<TE, TA>::value>,
                typename = EnableIfT<std::is_convertible<TD, TA>::value>,
                typename DiffType = decltype(std::declval<const TA&>() - std::declval<const TE&>()),
                typename = decltype(std::declval<const DiffType&>() <= std::declval<const TD&>())>
            static void AreEqual(const TE& expected,
                                 const TA& actual,
                                 const TD& delta,
                                 const std::string& message = "") {
                AreEqual(expected, actual, delta, [&]() { return message; });
            }

            /**
             * Verifies that two specified generic type data are almost equal by
             * comparing the magnitude of the difference between @p{expected} and
             * @p{actual} to @p{delta}. The assertion fails if the magnitude of the
             * difference is greater than @p{delta}.
             *
             * @param expected The value that the unit test expects.
             * @param actual   The value that the unit test produced.
             * @param delta    The maximum difference in magnitude.
             * @param msg_fn   A function that produces a string to be displayed if the
             *                 unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <
                typename TE,
                typename TA,
                typename TD,
                typename = EnableIfT<std::is_convertible<TE, TA>::value>,
                typename = EnableIfT<std::is_convertible<TD, TA>::value>,
                typename DiffType = decltype(std::declval<const TA&>() - std::declval<const TE&>()),
                typename = decltype(std::declval<const DiffType&>() <= std::declval<const TD&>())>
            static void AreEqual(const TE& expected,
                                 const TA& actual,
                                 const TD& delta,
                                 const std::function<std::string()>& msg_fn) {
                // Note (JW): Use !(x <= y) instead of x > y to get the right result
                // when one of the arguments is NaN.
                if (!(std::abs(actual - expected) <= delta)) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that two specified generic type data are not equal by using the
             * equality operator. The assertion fails if they are not equal.
             *
             * @param not_expected The value that the unit test expects.
             * @param actual       The value that the unit test produced.
             * @param message      A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename TE,
                      typename TA,
                      typename = EnableIfT<std::is_convertible<TE, TA>::value>>
            static void AreNotEqual(const TE& not_expected,
                                    const TA& actual,
                                    const std::string& message = "") {
                AreNotEqual(static_cast<TA>(not_expected), actual, [&]() { return message; });
            }

            /**
             * Verifies that two specified generic type data are not equal by using the
             * not equal operator. The assertion fails if they are equal.
             *
             * @param not_expected The value that the unit test expects.
             * @param actual       The value that the unit test produced.
             * @param msg_fn       A function that produces a string to be displayed if
             *                     the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename TE,
                      typename TA,
                      typename = EnableIfT<std::is_convertible<TE, TA>::value>>
            static void AreNotEqual(const TE& not_expected,
                                    const TA& actual,
                                    const std::function<std::string()>& msg_fn) {
                if (!(actual != static_cast<TA>(not_expected))) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that two specified generic type data are not almost equal by
             * comparing the magnitude of the difference between @p{expected} and
             * @p{actual} to @p{delta}. The assertion fails if the magnitude of the
             * difference is less than or equal to @p{delta}.
             *
             * @param not_expected The value that the unit test expects.
             * @param actual       The value that the unit test produced.
             * @param delta        The minimum difference in magnitude.
             * @param message      A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <
                typename TE,
                typename TA,
                typename TD,
                typename = EnableIfT<std::is_convertible<TE, TA>::value>,
                typename = EnableIfT<std::is_convertible<TD, TA>::value>,
                typename DiffType = decltype(std::declval<const TA&>() - std::declval<const TE&>()),
                typename = decltype(std::declval<const DiffType&>() > std::declval<const TD&>())>
            static void AreNotEqual(const TE& not_expected,
                                    const TA& actual,
                                    const TD& delta,
                                    const std::string& message = "") {
                AreNotEqual(not_expected, actual, delta, [&]() { return message; });
            }

            /**
             * Verifies that two specified generic type data are not almost equal by
             * comparing the magnitude of the difference between @p{expected} and
             * @p{actual} to @p{delta}. The assertion fails if the magnitude of the
             * difference is less than or equal to @p{delta}.
             *
             * @param not_expected The value that the unit test expects.
             * @param actual       The value that the unit test produced.
             * @param delta        The minimum difference in magnitude.
             * @param message      A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <
                typename TE,
                typename TA,
                typename TD,
                typename = EnableIfT<std::is_convertible<TE, TA>::value>,
                typename = EnableIfT<std::is_convertible<TD, TA>::value>,
                typename DiffType = decltype(std::declval<const TA&>() - std::declval<const TE&>()),
                typename = decltype(std::declval<const DiffType&>() > std::declval<const TD&>())>
            static void AreNotEqual(const TE& not_expected,
                                    const TA& actual,
                                    const TD& delta,
                                    const std::function<std::string()>& msg_fn) {
                // Note (JW): Use !(x > y) instead of x <= y to get the right result
                // when one of the arguments is NaN.
                if (!(std::abs(actual - not_expected) > delta)) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that two pointers refer to the same memory location. The
             * assertion fails if they refer to different memory locations.
             *
             * @param expected The value that the unit test expects.
             * @param actual   The value that the unit test produced.
             * @param message  A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename TE, typename TA>
            static void AreSame(const TE* expected,
                                const TA* actual,
                                const std::string& message = "") {
                AreSame(expected, actual, [&]() { return message; });
            }

            /**
             * Verifies that two pointers refer to the same memory location. The
             * assertion fails if they refer to different memory locations.
             *
             * @param expected The value that the unit test expects.
             * @param actual   The value that the unit test produced.
             * @param message  A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename TE, typename TA>
            static void AreSame(const TE* expected,
                                const TA* actual,
                                const std::function<std::string()>& msg_fn) {
                void* a = static_cast<void*>(expected);
                void* b = static_cast<void*>(actual);
                if (a != b) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that two pointers refer to different memory locations. The
             * assertion fails if they refer to the same memory location.
             *
             * @param expected The value that the unit test expects.
             * @param actual   The value that the unit test produced.
             * @param message  A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename TE, typename TA>
            static void AreNotSame(const TE* expected,
                                   const TA* actual,
                                   const std::string& message = "") {
                AreNotSame(expected, actual, [&]() { return message; });
            }

            /**
             * Verifies that two pointers refer to different memory locations. The
             * assertion fails if they refer to the same memory location.
             *
             * @param expected The value that the unit test expects.
             * @param actual   The value that the unit test produced.
             * @param message  A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename TE, typename TA>
            static void AreNotSame(const TE* expected,
                                   const TA* actual,
                                   const std::function<std::string()>& msg_fn) {
                void* a = static_cast<void*>(expected);
                void* b = static_cast<void*>(actual);
                if (a == b) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Fails without checking any conditions
             *
             * @param message A message to be display.
             * @expection AssertFailed Always thrown.
             */
            [[noreturn]] static void Fail(const std::string& message = "") {
                throw AssertFailed(message);
            }

            /**
             * Verifies that two specified condition is true. The assertion fails if the
             * condition is false.
             *
             * @param condition The condition to verify.
             * @param message   A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            static void IsTrue(bool condition, const std::string& message = "") {
                return IsTrue(condition, [&]() { return message; });
            }

            /**
             * Verifies that two specified condition is true. The assertion fails if the
             * condition is false.
             *
             * @param condition The condition to verify.
             * @param message   A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            static void IsTrue(bool condition, const std::function<std::string()>& msg_fn) {
                if (!condition) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that two specified condition is false. The assertion fails if
             * the condition is true.
             *
             * @param condition The condition to verify.
             * @param message   A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            static void IsFalse(bool condition, const std::string& message = "") {
                return IsFalse(condition, [&]() { return message; });
            }

            /**
             * Verifies that two specified condition is false. The assertion fails if
             * the condition is true.
             *
             * @param condition The condition to verify.
             * @param message   A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            static void IsFalse(bool condition, const std::function<std::string()>& msg_fn) {
                if (condition) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that the specified value is not a number using
             * @code{std::isnan}. The assertion fails if @p{value} is a valid number.
             * Note that this function differs from @code{std::isnan} in that it only
             * accepts floating point types. It will not cast integral types to double.
             *
             * @param value    The value to check.
             * @param message  A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename T, typename = EnableIfT<std::is_floating_point<T>::value>>
            static void IsNaN(T value, const std::string& message = "") {
                IsNan(value, [&]() { return message; });
            }

            /**
             * Verifies that the specified value is not a number using
             * @code{std::isnan}. The assertion fails if @p{value} is a valid number.
             * Note that this function differs from @code{std::isnan} in that it only
             * accepts floating point types. It will not cast integral types to double.
             *
             * @param value  The value to check.
             * @param msg_fn A function that produces a string to be displayed if the
             *               unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename T, typename = EnableIfT<std::is_floating_point<T>::value>>
            static void IsNaN(T value, const std::function<std::string()>& msg_fn) {
                if (!std::isnan(value)) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that the specified value is not a number using
             * @code{std::isnan}. The assertion fails if @p{value} is a valid number.
             * Note that this function differs from @code{std::isnan} in that it only
             * accepts floating point types. It will not cast integral types to double.
             *
             * @param value   The value to check.
             * @param message A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename T, typename = EnableIfT<std::is_floating_point<T>::value>>
            static void IsNotNaN(T value, const std::string& message = "") {
                IsNotNaN(value, [&]() { return message; });
            }

            /**
             * Verifies that the specified value is not a number using
             * @code{std::isnan}. The assertion fails if @p{value} is a valid number.
             * Note that this function differs from @code{std::isnan} in that it only
             * accepts floating point types. It will not cast integral types to double.
             *
             * @param value  The value to check.
             * @param msg_fn A function that produces a string to be displayed if the
             *               unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename T, typename = EnableIfT<std::is_floating_point<T>::value>>
            static void IsNotNaN(T value, const std::function<std::string()>& msg_fn) {
                if (std::isnan(value)) {
                    throw AssertFailed(msg_fn());
                }
            }

            /**
             * Verifies that the specified pointer is null. The assertion fails if it is
             * not null.
             *
             * @param value   The value to check.
             * @param message A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename T>
            static void IsNull(const RemoveRefT<T>* value, const std::string& message = "") {
                IsNull(value, [&]() { return message; });
            }

            /**
             * Verifies that the specified value is not valid. The assertion fails if it
             * coerces to true.
             *
             * @param value   The value to check.
             * @param message A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename T>
            static void IsNull(const RemoveRefT<T>& value, const std::string& message = "") {
                if (value) {
                    throw AssertFailed(message);
                }
            }

            /**
             * Verifies that the specified pointer is not null. The assertion fails if
             * it is null.
             *
             * @param value   The value to check.
             * @param message A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename T>
            static void IsNotNull(const RemoveRefT<T>* value, const std::string& message = "") {
                if (!value) {
                    throw AssertFailed(message);
                }
            }

            /**
             * Verifies that the specified value is valid. The assertion fails if it
             * coerces to false.
             *
             * @param value   The value to check.
             * @param message A message to be displayed if the unit test fails.
             * @expection AssertFailed If the assertion fails.
             */
            template <typename T>
            static void IsNotNull(const RemoveRefT<T>& value, const std::string& message = "") {
                if (!value) {
                    throw AssertFailed(message);
                }
            }
        };

        struct TestCase {
            std::string name = "";
            bool passed = false;
            std::string failureMessage = "";
            std::chrono::microseconds time = std::chrono::microseconds(0);
        };

        struct TestSuite {
            std::string name = "";
            std::size_t failed = 0;
            std::chrono::microseconds time = std::chrono::microseconds(0);
            std::vector<TestCase> results;
        };

        /**
         * Represents a harness for running unit tests.
         */
        class TestContext {
            using clock = std::chrono::high_resolution_clock;
            using time_point = clock::time_point;
            using duration = clock::duration;

        public:
            using TestFunction = std::function<void(void)>;

            /** Initializes a new TestContext. */
            TestContext() : out(&std::cout) {}

            /**
             * Adds a function that contains code that must be run after all of the
             * tests in the specified category have run and to cleanup resources that
             * are used by the tests.
             *
             * @param category The name of the category associated with the initializer.
             * @param clean    A function to run.
             */
            void AddCategoryCleanup(const std::string& category, const TestFunction& clean) {
                cleanup[category] = clean;
            }

            /**
             * Adds a function that contains code that must be run before any of the
             * tests in the specified category have run and to allocate resources to be
             * used by the tests.
             *
             * @param category The name of the category associated with the initializer.
             * @param init     A function to run.
             */
            void AddCategoryInitializer(const std::string& category, const TestFunction& init) {
                initializers[category] = init;
            }

            /**
             * Adds a unit test to the specified category. If no category is specified,
             * the test is added to the empty category.
             *
             * @param name     The name of the test. Names are printed along with
             *                 pass/fail status when tests are executed.
             * @param test     A unit test function.
             * @param category The group of the unit test. Unit tests results are
             *                 printed based on their group.
             */
            void AddTest(const std::string& name,
                         const TestFunction& test,
                         const std::string& category = "") {
                auto it = category_indexes.find(category);
                if (it == category_indexes.end()) {
                    TestList tl;
                    tl.emplace_back(name, test);
                    test_categories.emplace_back(category, tl);
                    category_indexes.emplace(category, test_categories.size() - 1);
                }
                else {
                    std::size_t cat_index = it->second;
                    TestList& cat_tests = test_categories.at(cat_index).second;
                    cat_tests.emplace_back(name, test);
                }
            }

            /**
             * Executes the added unit tests.
             *
             * @returns The number of tests that failed.
             */
            std::size_t Execute() {
                std::ostream& rout = *out;
                time_point startTime = clock::now();
                for (const TestCategory& cat : test_categories) {
                    TestSuite ts;
                    ts.name = cat.first;

                    rout << "Running " << cat.first << ": ";
                    time_point tsStart = clock::now();
                    try {
                        TestFunction& initializer = initializers[cat.first];
                        if (initializer) {
                            initializer();
                        }
                    }
                    catch (...) {
                        int s = static_cast<int>(cat.second.size());
                        rout << "Initializer failed. Skipping " << s << " tests.\n";
                        failed += s;
                        continue;
                    }

                    for (const NameTestPair& ntp : cat.second) {
                        TestCase tc;
                        tc.name = ntp.first;
                        time_point tcStart = clock::now();
                        try {
                            ntp.second();
                            tc.passed = true;
                        }
                        catch (const AssertFailed& ex) {
                            ++failed;
                            ++ts.failed;
                            tc.passed = false;
                            tc.failureMessage = std::string("Assert Failed: ") + ex.what();
                        }
                        catch (const std::exception& ex) {
                            ++failed;
                            ++ts.failed;
                            tc.passed = false;
                            tc.failureMessage = std::string("Exception: ") + ex.what();
                        }
                        catch (...) {
                            ++failed;
                            ++ts.failed;
                            tc.passed = false;
                            tc.failureMessage = "Unknown exception";
                        }
                        duration tcTime = clock::now() - tcStart;
                        tc.time = std::chrono::duration_cast<std::chrono::microseconds>(tcTime);
                        ts.results.push_back(tc);
                    }

                    duration tsTime = clock::now() - tsStart;
                    ts.time = std::chrono::duration_cast<std::chrono::microseconds>(tsTime);
                    results.push_back(ts);
                    if (ts.failed == 0) {
                        rout << "All passed. (" << ts.time.count() / 1000 << "ms)" << std::endl;
                    }
                    else {
                        rout << std::endl;
                        for (const auto& tc : ts.results) {
                            rout << "    " << tc.name << " -- ";
                            if (tc.passed) {
                                rout << "PASSED (" << tc.time.count() / 1000 << "ms)" << std::endl;
                            }
                            else {
                                rout << "FAILED: " << tc.failureMessage << std::endl;
                            }
                        }
                    }

                    try {
                        TestFunction& clean = cleanup[cat.first];
                        if (clean) {
                            clean();
                        }
                    }
                    catch (...) {
                        rout << "    Cleanup failed.\n";
                    }
                }
                duration contextTime = clock::now() - startTime;
                time = std::chrono::duration_cast<std::chrono::microseconds>(contextTime);
                return failed;
            }

            /**
             * Sets the stream to which results are printed.
             *
             * @param stream The stream to output to.
             */
            void SetOutput(std::ostream& stream) {
                out = &stream;
            }

            void WriteJUnit(std::ostream& stream) {
                stream << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
                stream << "<testsuites name=\"PCOE\" failures=\"" << failed << "\" time=\""
                       << usTos(time) << "\">\n";
                for (const auto& cat : results) {
                    stream << "    <testsuite name=\"" << cat.name << "\" failures=\"" << cat.failed
                           << "\" time=\"" << usTos(cat.time) << "\">\n";
                    for (const auto& test : cat.results) {
                        stream << "        <testcase name=\"" << test.name << "\" time=\""
                               << usTos(test.time) << "\"";
                        if (!test.failureMessage.empty()) {
                            stream << ">\n";
                            stream << "            <failure message=\"" << test.failureMessage
                                   << "\"/>\n";
                            stream << "        </testcase>\n";
                        }
                        else {
                            stream << "/>\n";
                        }
                    }
                    stream << "    </testsuite>\n";
                }
                stream << "</testsuites>\n";
            }

        private:
            inline double usTos(std::chrono::microseconds t) {
                return static_cast<double>(t.count()) / 1000000.0;
            }

            using NameTestPair = std::pair<std::string, TestFunction>;
            using TestList = std::vector<NameTestPair>;
            using TestCategory = std::pair<std::string, TestList>;

            std::vector<TestCategory> test_categories;
            std::unordered_map<std::string, std::size_t> category_indexes;
            std::unordered_map<std::string, TestFunction> cleanup;
            std::unordered_map<std::string, TestFunction> initializers;
            std::ostream* out;
            std::size_t failed = 0;
            std::chrono::microseconds time = std::chrono::microseconds(0);
            std::vector<TestSuite> results;
        };
    }
}

#endif
