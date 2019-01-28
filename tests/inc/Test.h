/** @file  Test.h
 * @brief A simple, robust and lightweight unit testing framework.
 * @details
 *      This file contains a classes to facilitate running unit test
 *      functions. Any function that takes no arguments and has no return
 *      value can be used as a test. Tests will fail if they throw or cause an
 *      abort. The expectation is that tests will fail as the result of a
 *      failed assertion using the @see{Assert} class, which throws
 *      @see{AssertFailed} exceptions. Other exceptions and aborts are
 *      reported as such and considered unexpected.
 * @details
 *      The general usage of this framework is as follows. First, a number of
 *      unit tests are written that take no arguments and return nothing. The
 *      test methods are expected to use the static methods of the
 *      @see{Assert} class to evaluate the expected outcomes of the test.
 * @details
 *      With the test cases written, a new @see{TestContext} is created, and
 *      tests are added to it. Once the tests are added, calling the Execute
 *      method will run all of the tests, outputting the progress of the tests
 *      to stdout or another std::ostream specified by calling SetOutput. The
 *      number of failed tests is also reported by the return value of Execute
 *      to allow for programatic action if some tests fail.
 *
 * @example
 *      #include <cstdlib>
 *      #include "Test.h"
 *      using namespace PCOE::Test;
 *      void alway_pass() { } // Any test that doesn't throw or abort passes
 *      void alway_fail() { Assert::Fail(); }
 *      void abort_fail() { std::abort(); } // Fails, but doesn't crash
 *      int main() {
 *          TestContext tests;
 *          tests.AddTest("Pass", always_pass);
 *          tests.AddTest("Fail", alway_fail);
 *          tests.AddTest("Abort", abort_fail);
 *          return tests.Execute();
 *      }
 *
 * @copyright Copyright (c) 2018 United States Government as represented by
 *            the Administrator of the National Aeronautics and Space
 *            Administration. All Rights Reserved.
 */

#ifndef PCOE_TEST_H
#define PCOE_TEST_H

#include <chrono>
#include <cmath>
#include <csetjmp>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace PCOE {
    namespace Test {
        /** @brief An exception to be thrown when a unit test assertion fails.
         *
         * @author  Jason Watkins <jason-watkins@outlook.com>
         * @version 1.0.0
         * @date    2016-07-13
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

        /** @brief Verifies conditions in unit tests using true/false propositions.
         *
         * @author  Jason Watkins <jason-watkins@outlook.com>
         * @version 1.0.0
         * @date    2016-07-13
         */
        class Assert {
        public:
            /** @brief Verifies that two specified generic type data are equal
             *        by using the equality operator. The assertion fails if
             *        they are not equal.
             *
             * @param expected The value that the unit test expects.
             * @param acutal   The value that the unit test produced.
             * @param message  A message to be displayed if the unit test
             *                   fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename T>
            static void AreEqual(const T& expected,
                                 const T& actual,
                                 const std::string& message = "") {
                if (!(actual == expected)) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified generic type data are equal
             *        by using the equality operator. The assertion fails if
             *        they are not equal.
             *
             * @param expected The value that the unit test expects.
             * @param acutal   The value that the unit test produced.
             * @param message  A message to be displayed if the unit test
             *                   fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename TExpected, typename TActual>
            static void AreEqual(const TExpected& expected,
                                 const TActual& actual,
                                 const std::string& message = "") {
                TActual tmp = static_cast<TActual>(expected);
                if (!(actual == tmp)) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified generic type data are almost
             *        equal by comparing the magnitude of the difference
             *        between @p{expected} and @p{actual} to @p{delta}.
             *        The assertion fails if the magnitude of the difference
             *        is greater than @p{delta}.
             *
             * @param expected The value that the unit test expects.
             * @param acutal   The value that the unit test produced.
             * @param delta    The maximum difference in magnitude.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename T>
            static void AreEqual(const T& expected,
                                 const T& actual,
                                 const T& delta,
                                 const std::string& message = "") {
                if (std::abs(actual - expected) > delta) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified doubles are almost
             *        equal by comparing the magnitude of the difference
             *        between @p{expected} and @p{actual} to @p{delta}.
             *        The assertion fails if the magnitude of the difference
             *        is greater than @p{delta}.
             *
             * @param expected The value that the unit test expects.
             * @param acutal   The value that the unit test produced.
             * @param delta    The maximum difference in magnitude.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            static void AreEqual(double expected,
                                 double actual,
                                 double delta,
                                 const std::string& message = "") {
                if (std::abs(actual - expected) > delta) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified generic type data are almost
             *        equal by comparing the magnitude of the difference
             *        between @p{expected} and @p{actual} to @p{delta}.
             *        The assertion fails if the magnitude of the difference
             *        is greater than @p{delta}.
             *
             * @param expected The value that the unit test expects.
             * @param acutal   The value that the unit test produced.
             * @param delta    The maximum difference in magnitude.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename TExpected, typename TActual>
            static void AreEqual(const TExpected& expected,
                                 const TActual& actual,
                                 const TActual& delta,
                                 const std::string& message = "") {
                TActual tmp = static_cast<TActual>(expected);
                if (std::abs(actual - tmp) > delta) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified generic type data are not
             *          equal by using the not equal operator. The assertion
             *          fails if they are equal.
             *
             * @param notExpected The value that the unit test expects.
             * @param acutal      The value that the unit test produced.
             * @param message     A message to be displayed if the unit test
             *                    fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename T>
            static void AreNotEqual(const T& notExpected,
                                    const T& actual,
                                    const std::string& message = "") {
                if (!(actual != notExpected)) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified generic type data are not
             *        equal by using the equality operator. The assertion
             *        fails if they are not equal.
             *
             * @param notExpected The value that the unit test expects.
             * @param acutal      The value that the unit test produced.
             * @param message     A message to be displayed if the unit test
             *                    fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename TExpected, typename TActual>
            static void AreNotEqual(const TExpected& notExpected,
                                    const TActual& actual,
                                    const std::string& message = "") {
                TActual tmp = static_cast<TActual>(notExpected);
                if (!(actual == tmp)) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified generic type data are not
             *        almost equal by comparing the magnitude of the
             *        difference between @p{expected} and @p{actual} to
             *        @p{delta}. The assertion fails if the magnitude of
             *        the difference is less than or equal to @p{delta}.
             *
             * @param notExpected The value that the unit test expects.
             * @param acutal      The value that the unit test produced.
             * @param delta       The minimum difference in magnitude.
             * @param message      A message to be displayed if the unit test
             *                     fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename T>
            static void AreNotEqual(const T& notExpected,
                                    const T& actual,
                                    const T& delta,
                                    const std::string& message = "") {
                if (std::abs(actual - notExpected) <= delta) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified doubles are almost
             *        equal by comparing the magnitude of the difference
             *        between @p{expected} and @p{actual} to @p{delta}.
             *        The assertion fails if the magnitude of the difference
             *        is greater than @p{delta}.
             *
             * @param notExpected The value that the unit test expects.
             * @param acutal      The value that the unit test produced.
             * @param delta       The maximum difference in magnitude.
             * @param message     A message to be displayed if the unit test
             *                    fails.
             * @expection AssertionFailed If the assertion fails.
             */
            static void AreNotEqual(double notExpected,
                                    double actual,
                                    double delta,
                                    const std::string& message = "") {
                if (std::abs(actual - notExpected) <= delta) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified generic type data are not
             *        almost equal by comparing the magnitude of the
             *        difference between @p{expected} and @p{actual} to
             *        @p{delta}. The assertion fails if the magnitude of
             *        the difference is less than or equal to @p{delta}.
             *
             * @param notExpected The value that the unit test expects.
             * @param acutal      The value that the unit test produced.
             * @param delta       The minimum difference in magnitude.
             * @param message     A message to be displayed if the unit test
             *                    fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename TExpected, typename TActual>
            static void AreNotEqual(const TExpected& notExpected,
                                    const TActual& actual,
                                    const TActual& delta,
                                    const std::string& message = "") {
                TActual tmp = static_cast<TActual>(notExpected);
                if (std::abs(actual - tmp) <= delta) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two pointers refer to the same memory
             *        location. The assertion fails if they refer to different
             *        memory locations.
             *
             * @param expected The value that the unit test expects.
             * @param acutal   The value that the unit test produced.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename TExpected, typename TActual>
            static void AreSame(const TExpected* expected,
                                const TActual* actual,
                                const std::string& message = "") {
                void* a = static_cast<void*>(expected);
                void* b = static_cast<void*>(actual);
                if (a != b) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two pointers refer to different memory
             *        locations. The assertion fails if they refer to the
             *        same memory location.
             *
             * @param notExpected The value that the unit test expects.
             * @param acutal      The value that the unit test produced.
             * @param message     A message to be displayed if the unit test
             *                    fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename TExpected, typename TActual>
            static void AreNotSame(const TExpected* expected,
                                   const TActual* actual,
                                   const std::string& message = "") {
                void* a = static_cast<void*>(expected);
                void* b = static_cast<void*>(actual);
                if (a == b) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Fails without checking any conditions
             *
             * @param message  A message to be display.
             * @expection AssertionFailed Always thrown.
             */
            [[noreturn]] static void Fail(const std::string& message = "") {
                throw AssertFailed(message);
            }

            /** @brief Verifies that two specified condition is false. The
             *        assertion fails if the condition is true.
             *
             * @param condition The condition to verify.
             * @param message   A message to be displayed if the unit test
             *                  fails.
             * @expection AssertionFailed If the assertion fails.
             */
            static void IsFalse(bool condition, const std::string& message = "") {
                if (condition) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that the specified float value is not a
             *        number using std::isnan. The assertion fails if
             *        @p{value} is a valid number.
             *
             * @param value    The value to check.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            static void IsNaN(float value, const std::string& message = "") {
                if (!std::isnan(value)) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that the specified double value is not a
             *        number using std::isnan. The assertion fails if
             *        @p{value} is a valid number.
             *
             * @param value    The value to check.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            static void IsNaN(double value, const std::string& message = "") {
                if (!std::isnan(value)) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that the specified long double value is not a
             *        number using std::isnan. The assertion fails if
             *        @p{value} is a valid number.
             *
             * @param value    The value to check.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            static void IsNaN(long double value, const std::string& message = "") {
                if (!std::isnan(value)) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that the specified pointer is null. The
             *        assertion fails if it is not null.
             *
             * @param value    The value to check.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename T>
            static void IsNull(const T* value, const std::string& message = "") {
                if (value) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that the specified value is not valid. The
             *        assertion fails if it coerces to true.
             *
             * @param value    The value to check.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename T>
            static void IsNull(const T& value, const std::string& message = "") {
                if (value) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that the specified pointer is not null. The
             *        assertion fails if it is null.
             *
             * @param value    The value to check.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename T>
            static void IsNotNull(const T* value, const std::string& message = "") {
                if (!value) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that the specified value is valid. The
             *          assertion fails if it coerces to false.
             *
             * @param value    The value to check.
             * @param message  A message to be displayed if the unit test
             *                 fails.
             * @expection AssertionFailed If the assertion fails.
             */
            template <typename T>
            static void IsNotNull(const T& value, const std::string& message = "") {
                if (!value) {
                    throw AssertFailed(message);
                }
            }

            /** @brief Verifies that two specified condition is true. The
             *        assertion fails if the condition is false.
             *
             * @param condition The condition to verify.
             * @param message   A message to be displayed if the unit test
             *                  fails.
             * @expection AssertionFailed If the assertion fails.
             */
            static void IsTrue(bool condition, const std::string& message = "") {
                if (!condition) {
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

        /** @brief Represents a harness for running unit tests.
         *
         *  @author  Jason Watkins <jason-watkins@outlook.com>
         *  @version 1.0.0
         *  @date    2016-07-13
         */
        class TestContext {
            using clock = std::chrono::high_resolution_clock;
            using time_point = clock::time_point;
            using duration = clock::duration;

        public:
            using TestFunction = std::function<void(void)>;

            /** @brief Initializes a new TestContext. */
            TestContext() : out(&std::cout) {}

            /** @brief Adds a function that contains code that must be run
             *        after all of the tests in the specified category have
             *        run and to cleanup resources that are used by the tests.
             *
             * @param category The name of the category associated with the
             *                 initializer.
             * @param clean    A function to run.
             */
            void AddCategoryCleanup(const std::string& category, const TestFunction& clean) {
                cleanup[category] = clean;
            }

            /** @brief Adds a function that contains code that must be run
             *        before any of the tests in the specified category have
             *        run and to allocate resources to be used by the tests.
             *
             * @param category The name of the category associated with the
             *                 initializer.
             * @param init     A function to run.
             */
            void AddCategoryInitializer(const std::string& category, const TestFunction& init) {
                initializers[category] = init;
            }

            /** @brief Adds a unit test to the specified category. If no
             *        category is specified, the test is added to the empty
             *        category.
             *
             * @param name     The name of the test. Names are printed along
             *                 with pass/fail status when tests are executed.
             * @param test     A unit test function.
             * @param category The group of the unit test. Unit tests results
             *                 are printed based on their group.
             */
            void AddTest(const std::string& name,
                         const TestFunction& test,
                         const std::string& category = "") {
                NameTestPair ntp = std::make_pair(name, test);
                tests[category].push_back(ntp);
            }

            /** @brief Executes the added unit tests.
             *
             * @returns The number of tests that failed.
             */
            int Execute() {
                std::ostream& rout = *out;
                time_point startTime = clock::now();
                for (const TestCategory& cat : tests) {
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
                        std::size_t s = cat.second.size();
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

                    if (ts.failed == 0) {
                        rout << "All passed." << std::endl;
                    }
                    else {
                        rout << std::endl;
                        for (const auto& tc : ts.results) {
                            rout << "    " << tc.name << " -- ";
                            if (tc.passed) {
                                rout << "PASSED" << std::endl;
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
                    duration tsTime = clock::now() - tsStart;
                    ts.time = std::chrono::duration_cast<std::chrono::microseconds>(tsTime);
                    results.push_back(ts);
                }
                duration contextTime = clock::now() - startTime;
                time = std::chrono::duration_cast<std::chrono::microseconds>(contextTime);
                return failed;
            }

            /** @brief Sets the stream to which results are printed.
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
            using CategoryMap = std::unordered_map<std::string, TestList>;

            CategoryMap tests;
            std::unordered_map<std::string, TestFunction> cleanup;
            std::unordered_map<std::string, TestFunction> initializers;
            std::ostream* out;
            std::size_t failed = 0;
            std::chrono::microseconds time = std::chrono::microseconds(0);
            std::vector<TestSuite> results;
        };
    }
}

#endif // PCOE_TEST_H
