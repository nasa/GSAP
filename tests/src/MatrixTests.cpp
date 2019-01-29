// Copyright (c) 2016-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <iostream>
#include <random>
#include <sstream>

#include "Matrix.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace MatrixTests {
    static std::random_device rd;
    static std::mt19937 rng(rd());
    static std::uniform_real_distribution<> dist(-1000, 1000);

    const std::size_t ITERATIONS = 20;

    void construct_default() {
        try {
            Matrix m;
        }
        catch (...) {
            Assert::Fail("Constructor threw an exception");
        }
    }

    void construct_size() {
        std::uniform_int_distribution<std::size_t> sdist(0, 100);
        for (size_t i = 0; i < ITERATIONS; ++i) {
            std::size_t m = sdist(rng);
            std::size_t n = sdist(rng);
            try {
                Matrix matrix(m, n);
            }
            catch (...) {
                std::stringstream ss;
                ss << "Constructor threw an exception with size (" << m << ", " << n << ")";
                Assert::Fail(ss.str());
            }
        }
    }

    void construct_initialvalue() {
        std::uniform_int_distribution<std::size_t> sdist(0, 100);
        for (size_t i = 0; i < ITERATIONS; ++i) {
            std::size_t m = sdist(rng);
            std::size_t n = sdist(rng);
            double v = dist(rng);
            try {
                Matrix matrix(m, n, v);
            }
            catch (...) {
                std::stringstream ss;
                ss << "Constructor threw an exception with size (" << m << ", " << n << ") and IV "
                   << v;
                Assert::Fail(ss.str());
            }
        }
    }

    void construct_initializerlist() {
        try {
            std::size_t m = 4;
            std::size_t n = 4;
            Matrix matrix(4, 4, {0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6});
            for (std::size_t i = 0; i < m; i++) {
                for (std::size_t j = 0; j < n; j++) {
                    Assert::AreEqual(i + j, matrix[i][j], 1e-15, "Unexpected element value");
                }
            }
        }
        catch (...) {
            Assert::Fail("Constructor threw an exception");
        }
    }

    void construct_vector() {
        std::uniform_int_distribution<std::size_t> sdist(0, 500);
        for (size_t i = 0; i < ITERATIONS; ++i) {
            std::size_t m = sdist(rng);
            std::vector<double> v(m);
            for (std::size_t j = 0; i < m; i++) {
                v[j] = dist(rng);
            }

            try {
                Matrix matrix(v);
                Assert::AreEqual(m, matrix.rows());
                Assert::AreEqual(1, matrix.cols());
            }
            catch (...) {
                Assert::Fail("Constructor threw an exception");
            }
        }
    }

    void construct_concat() {
        const Matrix a(3,
                       2,
                       {
                           1,
                           1,
                           2,
                           2,
                           3,
                           3,
                       });
        const Matrix b(1, 2, {4, 4});
        const Matrix c(3, 1, {5, 6, 7});

        Matrix d({a, b});
        Assert::AreEqual(d.rows(), a.rows() + b.rows(), "Number of rows incorrect for row concat");
        Assert::AreEqual(d.cols(), a.cols(), "Number of columns incorrect for row concat");
        Assert::AreEqual(d.at(0, 0), a.at(0, 0), 1e-12, "Top left incorrect for row concat");
        Assert::AreEqual(d.at(3, 1), b.at(0, 1), 1e-12, "Bottom right incorrect for row concat");

        Matrix e({a, c});
        Assert::AreEqual(e.rows(), a.rows(), "Number of rows incorrect for col concat");
        Assert::AreEqual(e.cols(), a.cols() + c.cols(), "Number of cols incorrect for col concat");
        Assert::AreEqual(e.at(0, 0), a.at(0, 0), 1e-12, "Top left incorrect for col concat");
        Assert::AreEqual(e.at(2, 2), c.at(2, 0), 1e-12, "Bottom right incorrect for col concat");

        try {
            Matrix f({a, a});
            Assert::Fail("Concat constructor constructed ambiguous matrix");
        }
        catch (std::domain_error&) {
        }

        try {
            Matrix f({b, c});
            Assert::Fail("Concat constructor constructed impossible matrix");
        }
        catch (std::domain_error&) {
        }
    }

    void construct_concat_sized() {
        const Matrix a(3,
                       2,
                       {
                           1,
                           1,
                           2,
                           2,
                           3,
                           3,
                       });
        const Matrix b(3,
                       2,
                       {
                           4,
                           4,
                           5,
                           5,
                           6,
                           6,
                       });

        Matrix d(3, 4, {a, b});
        Assert::AreEqual(d.rows(), 3, "Number of rows incorrect for col concat");
        Assert::AreEqual(d.cols(), 4, "Number of cols incorrect for col concat");
        Assert::AreEqual(d.at(0, 0), a.at(0, 0), 1e-12, "Top left incorrect for col concat");
        Assert::AreEqual(d.at(2, 3), b.at(2, 1), 1e-12, "Bottom right incorrect for col concat");

        Matrix e(6, 2, {a, b});
        Assert::AreEqual(e.rows(), 6, "Number of rows incorrect for row concat");
        Assert::AreEqual(e.cols(), 2, "Number of columns incorrect for row concat");
        Assert::AreEqual(e.at(0, 0), a.at(0, 0), 1e-12, "Top left incorrect for row concat");
        Assert::AreEqual(e.at(5, 1), b.at(2, 1), 1e-12, "Bottom right incorrect for row concat");

        try {
            Matrix f(3, 3, {b});
            Assert::Fail("Concat constructor constructed impossible matrix");
        }
        catch (std::domain_error&) {
        }
    }

    void construct_copy() {
        std::size_t m = 5;
        std::size_t n = 10;
        double v = 7.0;

        Matrix m1(m, n, v);
        Matrix m2(m1);
        Assert::AreEqual(m1.rows(), m2.rows(), "Number of rows not equal");
        Assert::AreEqual(m1.cols(), m2.cols(), "Number of columns not equal");
        for (std::size_t i = 0; i < m; i++) {
            for (std::size_t j = 0; j < n; j++) {
                Assert::AreEqual(m1[i][j], m2[i][j], 1e-15, "Unexpected element value");
            }
        }
    }

    void construct_move() {
        std::size_t m = 5;
        std::size_t n = 10;
        double v = 7.0;

        Matrix m1(m, n, v);
        Matrix m2(std::move(m1));
        Assert::AreEqual(m, m2.rows(), "Number of rows not equal");
        Assert::AreEqual(n, m2.cols(), "Number of columns not equal");
        for (std::size_t i = 0; i < m; i++) {
            for (std::size_t j = 0; j < n; j++) {
                Assert::AreEqual(7.0, m2[i][j], 1e-15, "Unexpected element value");
            }
        }
    }

    void operator_assign() {
        std::size_t m = 5;
        std::size_t n = 10;
        double v = 7.0;

        Matrix m1(m, n, v);
        Matrix m2 = m1;
        Assert::AreEqual(m1.rows(), m2.rows(), "Number of rows not equal");
        Assert::AreEqual(m1.cols(), m2.cols(), "Number of columns not equal");
        for (std::size_t i = 0; i < m; i++) {
            for (std::size_t j = 0; j < n; j++) {
                Assert::AreEqual(m1[i][j], m2[i][j], 1e-15, "Unexpected element value");
            }
        }
    }

    void operator_equal() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double tmp = dist(rng);
                    m1[j][k] = tmp;
                    m2[j][k] = tmp;
                }
            }
            Assert::IsTrue(m1 == m2, "Matrices failed to compare as equal");
        }
    }

    void operator_notequal() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                    m2[j][k] = dist(rng);
                }
            }
            // Technically this might fail, but the odds of the rng producing
            // 200 consecutive doubles must be astronomically low
            Assert::IsTrue(m1 != m2, "Matrices compared as equal");
        }

        Matrix m3(m, n);
        Matrix m4(n, m);
        Assert::IsTrue(m3 != m4, "Matrices compared as equal");
    }

    void issquare() {
        std::uniform_int_distribution<std::size_t> sdist(0, 100);
        const std::size_t m = sdist(rng);
        const std::size_t n = sdist(rng);
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix matrix(m, n);
            Assert::AreEqual(m == n, matrix.isSquare());
        }
    }

    void rows() {
        std::uniform_int_distribution<std::size_t> sdist(0, 100);
        const std::size_t m = sdist(rng);
        const std::size_t n = sdist(rng);
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix matrix(m, n);
            Assert::AreEqual(m, matrix.rows(), "Unexpected number of rows");
        }
    }

    void cols() {
        std::uniform_int_distribution<std::size_t> sdist(0, 100);
        const std::size_t m = sdist(rng);
        const std::size_t n = sdist(rng);
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix matrix(m, n);
            Assert::AreEqual(n, matrix.cols(), "Unexpected number of columns");
        }
    }

    void indexer() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix matrix(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    values[j][k] = dist(rng);
                    matrix[j][k] = values[j][k];
                    Assert::AreEqual(values[j][k],
                                     matrix[j][k],
                                     1e-12,
                                     "Unexpected value in random part");
                }
            }
        }

        Matrix m2(3, 4, {1, 2, 3, 4, 5, 6, 7, 8, 4, 3, 2, 1});

        Assert::AreEqual(1, m2[0][0], 1e-12, "Unexpected value at m2[0][0]");
        Assert::AreEqual(2, m2[0][1], 1e-12, "Unexpected value at m2[0][1]");
        Assert::AreEqual(3, m2[0][2], 1e-12, "Unexpected value at m2[0][2]");
        Assert::AreEqual(4, m2[0][3], 1e-12, "Unexpected value at m2[0][3]");
        Assert::AreEqual(5, m2[1][0], 1e-12, "Unexpected value at m2[1][0]");
        Assert::AreEqual(6, m2[1][1], 1e-12, "Unexpected value at m2[1][1]");
        Assert::AreEqual(7, m2[1][2], 1e-12, "Unexpected value at m2[1][2]");
        Assert::AreEqual(8, m2[1][3], 1e-12, "Unexpected value at m2[1][3]");
        Assert::AreEqual(4, m2[2][0], 1e-12, "Unexpected value at m2[2][0]");
        Assert::AreEqual(3, m2[2][1], 1e-12, "Unexpected value at m2[2][1]");
        Assert::AreEqual(2, m2[2][2], 1e-12, "Unexpected value at m2[2][2]");
        Assert::AreEqual(1, m2[2][3], 1e-12, "Unexpected value at m2[2][3]");

        m2[0][0] = 42.0;
        m2[0][1] = 42.1;
        m2[0][2] = 42.2;
        m2[0][3] = 42.3;
        Assert::AreEqual(42.0, m2[0][0], 1e-12, "Unexpected value at m2[0][0]");
        Assert::AreEqual(42.1, m2[0][1], 1e-12, "Unexpected value at m2[0][1]");
        Assert::AreEqual(42.2, m2[0][2], 1e-12, "Unexpected value at m2[0][2]");
        Assert::AreEqual(42.3, m2[0][3], 1e-12, "Unexpected value at m2[0][3]");
    }

    void indexer_const() {
        const Matrix m2(3, 4, {1, 2, 3, 4, 5, 6, 7, 8, 4, 3, 2, 1});

        Assert::AreEqual(1, m2[0][0], 1e-12, "Unexpected value at m2[0][0]");
        Assert::AreEqual(2, m2[0][1], 1e-12, "Unexpected value at m2[0][1]");
        Assert::AreEqual(3, m2[0][2], 1e-12, "Unexpected value at m2[0][2]");
        Assert::AreEqual(4, m2[0][3], 1e-12, "Unexpected value at m2[0][3]");
        Assert::AreEqual(5, m2[1][0], 1e-12, "Unexpected value at m2[1][0]");
        Assert::AreEqual(6, m2[1][1], 1e-12, "Unexpected value at m2[1][1]");
        Assert::AreEqual(7, m2[1][2], 1e-12, "Unexpected value at m2[1][2]");
        Assert::AreEqual(8, m2[1][3], 1e-12, "Unexpected value at m2[1][3]");
        Assert::AreEqual(4, m2[2][0], 1e-12, "Unexpected value at m2[2][0]");
        Assert::AreEqual(3, m2[2][1], 1e-12, "Unexpected value at m2[2][1]");
        Assert::AreEqual(2, m2[2][2], 1e-12, "Unexpected value at m2[2][2]");
        Assert::AreEqual(1, m2[2][3], 1e-12, "Unexpected value at m2[2][3]");
    }

    void at() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix matrix(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    values[j][k] = dist(rng);
                    matrix[j][k] = values[j][k];
                    Assert::AreEqual(values[j][k],
                                     matrix.at(j, k),
                                     1e-12,
                                     "Unexpected value in random part");
                }
            }
        }

        Matrix m2(3, 4, {1, 2, 3, 4, 5, 6, 7, 8, 4, 3, 2, 1});

        Assert::AreEqual(1, m2.at(0, 0), 1e-12, "Unexpected value at m2.at(0, 0)");
        Assert::AreEqual(2, m2.at(0, 1), 1e-12, "Unexpected value at m2.at(0, 1)");
        Assert::AreEqual(3, m2.at(0, 2), 1e-12, "Unexpected value at m2.at(0, 2)");
        Assert::AreEqual(4, m2.at(0, 3), 1e-12, "Unexpected value at m2.at(0, 3)");
        Assert::AreEqual(5, m2.at(1, 0), 1e-12, "Unexpected value at m2.at(1, 0)");
        Assert::AreEqual(6, m2.at(1, 1), 1e-12, "Unexpected value at m2.at(1, 1)");
        Assert::AreEqual(7, m2.at(1, 2), 1e-12, "Unexpected value at m2.at(1, 2)");
        Assert::AreEqual(8, m2.at(1, 3), 1e-12, "Unexpected value at m2.at(1, 3)");
        Assert::AreEqual(4, m2.at(2, 0), 1e-12, "Unexpected value at m2.at(2, 0)");
        Assert::AreEqual(3, m2.at(2, 1), 1e-12, "Unexpected value at m2.at(2, 1)");
        Assert::AreEqual(2, m2.at(2, 2), 1e-12, "Unexpected value at m2.at(2, 2)");
        Assert::AreEqual(1, m2.at(2, 3), 1e-12, "Unexpected value at m2.at(2, 3)");

        m2[0][0] = 42.0;
        m2[0][1] = 42.1;
        m2[0][2] = 42.2;
        m2[0][3] = 42.3;
        Assert::AreEqual(42.0, m2.at(0, 0), 1e-12, "Unexpected value at m2.at(0, 0)");
        Assert::AreEqual(42.1, m2.at(0, 1), 1e-12, "Unexpected value at m2.at(0, 1)");
        Assert::AreEqual(42.2, m2.at(0, 2), 1e-12, "Unexpected value at m2.at(0, 2)");
        Assert::AreEqual(42.3, m2.at(0, 3), 1e-12, "Unexpected value at m2.at(0, 3)");
    }

    void const_at() {
        const Matrix m1(3, 4, {1, 2, 3, 4, 5, 6, 7, 8, 4, 3, 2, 1});

        Assert::AreEqual(1, m1.at(0, 0), 1e-12, "Unexpected value at m2.at(0, 0)");
        Assert::AreEqual(2, m1.at(0, 1), 1e-12, "Unexpected value at m2.at(0, 1)");
        Assert::AreEqual(3, m1.at(0, 2), 1e-12, "Unexpected value at m2.at(0, 2)");
        Assert::AreEqual(4, m1.at(0, 3), 1e-12, "Unexpected value at m2.at(0, 3)");
        Assert::AreEqual(5, m1.at(1, 0), 1e-12, "Unexpected value at m2.at(1, 0)");
        Assert::AreEqual(6, m1.at(1, 1), 1e-12, "Unexpected value at m2.at(1, 1)");
        Assert::AreEqual(7, m1.at(1, 2), 1e-12, "Unexpected value at m2.at(1, 2)");
        Assert::AreEqual(8, m1.at(1, 3), 1e-12, "Unexpected value at m2.at(1, 3)");
        Assert::AreEqual(4, m1.at(2, 0), 1e-12, "Unexpected value at m2.at(2, 0)");
        Assert::AreEqual(3, m1.at(2, 1), 1e-12, "Unexpected value at m2.at(2, 1)");
        Assert::AreEqual(2, m1.at(2, 2), 1e-12, "Unexpected value at m2.at(2, 2)");
        Assert::AreEqual(1, m1.at(2, 3), 1e-12, "Unexpected value at m2.at(2, 3)");
    }

    void col_get() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix matrix(m, n);
            for (std::size_t k = 0; k < n; ++k) {
                for (std::size_t j = 0; j < m; ++j) {
                    values[j][k] = dist(rng);
                    matrix[j][k] = values[j][k];
                }

                Matrix c = matrix.col(k);
                for (std::size_t j = 0; j < m; ++j) {
                    Assert::AreEqual(values[j][k],
                                     c.at(j, 0),
                                     1e-12,
                                     "Unexpected value in column vector");
                }
            }
        }

        Matrix m2(m, n);
        try {
            m2.col(n + 1);
            Assert::Fail("Got column that doesn't exist.");
        }
        catch (std::out_of_range&) {
        }
    }

    void col_setmatrix() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t k = 0; k < n; ++k) {
                Matrix c(m, 1);
                for (std::size_t j = 0; j < m; ++j) {
                    values[j][k] = dist(rng);
                    m1[j][k] = values[j][k];
                    c[j][0] = values[j][k];
                }
                m2.col(k, c);
            }
            Assert::AreEqual(m1, m2, "Matrices not equal");
        }

        Matrix m3(m, n);
        Matrix c1(m, 1);
        Matrix c2(m + 1, 1);
        Matrix c3(m - 1, 1);
        Matrix c4(m, 2);
        try {
            m3.row(m + 1, c1);
            Assert::Fail("Set column that doesn't exist.");
        }
        catch (std::out_of_range) {
        }
        try {
            m3.row(0, c2);
            m3.row(0, c3);
            Assert::Fail("Set column with invalid size.");
        }
        catch (std::domain_error) {
        }
        try {
            m3.row(0, c4);
            Assert::Fail("Set column with matrix that isn't a column vector.");
        }
        catch (std::domain_error) {
        }
    }

    void col_setvector() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t k = 0; k < n; ++k) {
                std::vector<double> c;
                for (std::size_t j = 0; j < m; ++j) {
                    values[j][k] = dist(rng);
                    m1[j][k] = values[j][k];
                    c.push_back(values[j][k]);
                }
                m2.col(k, c);
            }
            Assert::AreEqual(m1, m2, "Matrices not equal");
        }

        Matrix m3(m, n);
        std::vector<double> v1(m);
        std::vector<double> v2(m + 1);
        std::vector<double> v3(m - 1);
        try {
            m3.row(m + 1, v1);
            Assert::Fail("Set column that doesn't exist.");
        }
        catch (std::out_of_range) {
        }
        try {
            m3.row(0, v2);
            m3.row(0, v3);
            Assert::Fail("Set column with invalid size.");
        }
        catch (std::domain_error) {
        }
    }

    void row_get() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        Matrix matrix(m, n);
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    values[j][k] = dist(rng);
                    matrix[j][k] = values[j][k];
                }

                Matrix r = matrix.row(j);
                for (std::size_t k = 0; k < n; ++k) {
                    Assert::AreEqual(values[j][k],
                                     r.at(0, k),
                                     1e-12,
                                     "Unexpected value in column vector");
                }
            }
        }

        Matrix m2(m, n);
        try {
            m2.row(m + 1);
            Assert::Fail("Got row that doesn't exist.");
        }
        catch (std::out_of_range) {
        }
    }

    void row_setmatrix() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                Matrix r(1, n);
                for (std::size_t k = 0; k < n; ++k) {
                    values[j][k] = dist(rng);
                    m1[j][k] = values[j][k];
                    r[0][k] = values[j][k];
                }
                m2.row(j, r);
            }
            Assert::AreEqual(m1, m2, "Matrices not equal");
        }

        Matrix m3(m, n);
        Matrix r1(1, n);
        Matrix r2(1, n + 1);
        Matrix r3(1, n - 1);
        Matrix r4(2, n);
        try {
            m3.row(m + 1, r1);
            Assert::Fail("Set row that doesn't exist.");
        }
        catch (std::out_of_range&) {
        }
        try {
            m3.row(0, r2);
            m3.row(0, r3);
            Assert::Fail("Set row with invalid size.");
        }
        catch (std::domain_error) {
        }
        try {
            m3.row(0, r4);
            Assert::Fail("Set row with matrix that isn't a row vector.");
        }
        catch (std::domain_error) {
        }
    }

    void row_setvector() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                std::vector<double> r;
                for (std::size_t k = 0; k < n; ++k) {
                    values[j][k] = dist(rng);
                    m1[j][k] = values[j][k];
                    r.push_back(values[j][k]);
                }
                m2.row(j, r);
            }
            Assert::AreEqual(m1, m2, "Matrices not equal");
        }

        Matrix m3(m, n);
        std::vector<double> v1(n);
        std::vector<double> v2(n + 1);
        std::vector<double> v3(n - 1);
        try {
            m3.row(m + 1, v1);
            Assert::Fail("Set row that doesn't exist.");
        }
        catch (std::out_of_range) {
        }
        try {
            m3.row(0, v2);
            m3.row(0, v3);
            Assert::Fail("Set row with invalid size.");
        }
        catch (std::domain_error) {
        }
    }

    void operator_vector() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        double values[m][n];
        Matrix matrix(m, n);
        for (std::size_t i = 0; i < 10; ++i) {
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    values[j][k] = dist(rng);
                    matrix[j][k] = values[j][k];
                }

                std::vector<double> r = static_cast<std::vector<double>>(matrix.row(j));
                for (std::size_t k = 0; k < n; ++k) {
                    Assert::AreEqual(values[j][k],
                                     r.at(k),
                                     1e-12,
                                     "Unexpected value in column vector");
                }
            }

            for (std::size_t k = 0; k < n; ++k) {
                for (std::size_t j = 0; j < m; ++j) {
                    values[j][k] = dist(rng);
                    matrix[j][k] = values[j][k];
                }

                std::vector<double> c = static_cast<std::vector<double>>(matrix.col(k));
                for (std::size_t j = 0; j < m; ++j) {
                    Assert::AreEqual(values[j][k],
                                     c.at(j),
                                     1e-12,
                                     "Unexpected value in column vector");
                }
            }
        }
    }

    void resize() {
        const std::size_t m1 = 7;
        const std::size_t n1 = 5;
        const std::size_t m2 = 5;
        const std::size_t n2 = 3;
        const std::size_t m3 = 11;
        const std::size_t n3 = 13;
        Matrix matrix(m1, n1);
        Assert::AreEqual(m1, matrix.rows(), "Unexpected number of rows before resize");
        Assert::AreEqual(n1, matrix.cols(), "Unexpected number of columns before resize");
        for (std::size_t i = 0; i < m1; ++i) {
            for (std::size_t j = 0; j < n1; ++j) {
                matrix[i][j] = dist(rng);
            }
        }

        Matrix expected = matrix;
        matrix.resize(m2, n2);
        Assert::AreEqual(m2, matrix.rows(), "Unexpected number of rows shrinking");
        Assert::AreEqual(n2, matrix.cols(), "Unexpected number of columns shrinking");
        for (std::size_t i = 0; i < m2; ++i) {
            for (std::size_t j = 0; j < n2; ++j) {
                Assert::AreEqual(expected.at(i, j),
                                 matrix.at(i, j),
                                 1e-12,
                                 "Unexpected value after shrinking");
            }
        }

        expected = matrix;
        matrix.resize(m3, n3);
        Assert::AreEqual(m3, matrix.rows(), "Unexpected number of rows expansion");
        Assert::AreEqual(n3, matrix.cols(), "Unexpected number of columns expansion");
        for (std::size_t i = 0; i < m2; ++i) {
            for (std::size_t j = 0; j < n2; ++j) {
                Assert::AreEqual(expected.at(i, j),
                                 matrix.at(i, j),
                                 1e-12,
                                 "Unexpected value after expansion");
            }
        }
    }

    void negate_matrix() {
        Matrix m(2,
                 2,
                 {0,
                  1,
                  std::numeric_limits<double>::infinity(),
                  std::numeric_limits<double>::quiet_NaN()});
        Matrix m2 = -m;
        Assert::AreEqual(0, m2.at(0, 0), "Unexpected value for -0");
        Assert::AreEqual(-1, m2.at(0, 1), "Unexpected value for -1");
        Assert::AreEqual(-std::numeric_limits<double>::infinity(),
                         m2.at(1, 0),
                         "Unexpected value for -Inf");
        Assert::IsTrue(std::isnan(m2.at(1, 1)), "Unexpected value for -NaN");
    }

    void add_matrix() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                    m2[j][k] = dist(rng);
                }
            }
            Matrix result1 = m1 + m2;
            Matrix result2 = result1;
            result2 += m2;
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double e1 = m1[j][k] + m2[j][k];
                    double e2 = e1 + m2[j][k];
                    Assert::AreEqual(e1,
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after adition [+]");
                    Assert::AreEqual(e2,
                                     result2[j][k],
                                     1e-12,
                                     "Unexpected value after adition [+=]");
                }
            }
        }

        Matrix m3(m, n);
        Matrix m4(m - 1, n + 1);
        try {
            m3 + m4;
            Assert::Fail("Added matrices of different sizes");
        }
        catch (std::domain_error) {
        }
    }

    void add_scalar() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            double s = dist(rng);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                }
            }
            Matrix result1 = m1 + s;
            Matrix result2 = s + m1;
            Matrix result3 = result1;
            result3 += s;
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double e1 = m1[j][k] + s;
                    double e3 = e1 + s;
                    Assert::AreEqual(e1,
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after adition [s + m1]");
                    Assert::AreEqual(e1,
                                     result2[j][k],
                                     1e-12,
                                     "Unexpected value after adition [m1 + s]");
                    Assert::AreEqual(e3,
                                     result3[j][k],
                                     1e-12,
                                     "Unexpected value after adition [+=]");
                }
            }
        }
    }

    void subtract_matrix() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                    m2[j][k] = dist(rng);
                }
            }
            Matrix result1 = m1 - m2;
            Matrix result2 = result1;
            result2 -= m2;
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double e1 = m1[j][k] - m2[j][k];
                    double e2 = e1 - m2[j][k];
                    Assert::AreEqual(e1,
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after subtraction [-]");
                    Assert::AreEqual(e2,
                                     result2[j][k],
                                     1e-12,
                                     "Unexpected value after subtraction [-=]");
                }
            }
        }

        Matrix m3(m, n);
        Matrix m4(m - 1, n + 1);
        try {
            m3 - m4;
            Assert::Fail("Subtracted matrices of different sizes");
        }
        catch (std::domain_error) {
        }
    }

    void subtract_salar() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            double s = dist(rng);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                }
            }
            Matrix result1 = m1 - s;
            Matrix result2 = result1;
            result2 -= s;
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double e1 = m1[j][k] - s;
                    double e2 = e1 - s;
                    Assert::AreEqual(e1,
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after subtraction [-]");
                    Assert::AreEqual(e2,
                                     result2[j][k],
                                     1e-12,
                                     "Unexpected value after subtraction [-=]");
                }
            }
        }
    }

    void multiply_matrix() {
        Matrix m1(3, 5, {59, 59, 100, 94, 2, 89, 54, 31, 55, 96, 59, 36, 19, 51, 45});
        Matrix m2(5, 3, {33, 84, 31, 10, 75, 33, 87, 85, 6, 20, 100, 71, 17, 85, 92});
        Matrix e(3, 3, {13151, 27451, 11234, 8906, 27821, 17464, 5745, 18196, 10892});
        Matrix r = m1 * m2;
        for (std::size_t i = 0; i < 3; ++i) {
            for (std::size_t j = 0; j < 3; ++j) {
                Assert::AreEqual(e[i][j], r[i][j], 1e-12, "Unexpected value after multiplication");
            }
        }
    }

    void multiply_scalar() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            double s = dist(rng);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                }
            }
            Matrix result1 = m1 * s;
            Matrix result2 = s * m1;
            Matrix result3 = result1;
            result3 *= s;
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double e1 = m1[j][k] * s;
                    double e3 = e1 * s;
                    Assert::AreEqual(e1,
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after multiplication [s * m1]");
                    Assert::AreEqual(e1,
                                     result2[j][k],
                                     1e-12,
                                     "Unexpected value after multiplication [m1 * s]");
                    Assert::AreEqual(e3,
                                     result3[j][k],
                                     1e-12,
                                     "Unexpected value after multiplication [*=]");
                }
            }
        }
    }

    void divide_scalar() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            double s = dist(rng);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                }
            }
            Matrix result1 = m1 / s;
            Matrix result2 = result1;
            result2 /= s;
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double e1 = m1[j][k] / s;
                    double e2 = e1 / s;
                    Assert::AreEqual(e1,
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after division [/]");
                    Assert::AreEqual(e2,
                                     result2[j][k],
                                     1e-12,
                                     "Unexpected value after division [/=]");
                }
            }
        }
    }

    void modulo_by_scalar() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            double s = dist(rng);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                }
            }
            Matrix result1 = m1 % s;
            Matrix result2 = result1;
            result2 %= s;
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double e1 = fmod(m1[j][k], s);
                    double e2 = fmod(e1, s);
                    Assert::AreEqual(e1,
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after modulo [(mat) % (double)]");
                    Assert::AreEqual(e2,
                                     result2[j][k],
                                     1e-12,
                                     "Unexpected value after modulo [%=]");
                }
            }
        }
    }

    void modulo_scalar() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            double s = dist(rng);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                }
            }
            Matrix result1 = s % m1;
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    double e1 = fmod(s, m1[j][k]);
                    Assert::AreEqual(e1,
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after modulo [(double) % (mat)]");
                }
            }
        }
    }

    void elementwiseOperators_matrix() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix m1(m, n);
            Matrix m2(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m1[j][k] = dist(rng);
                }
            }
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    m2[j][k] = dist(rng);
                }
            }
            Matrix result1 = m1.elementwiseMultiply(m2);
            Matrix result2 = result1;
            result2 = result2.elementwiseDivide(m2);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    Assert::AreEqual(m1[j][k] * m2[j][k],
                                     result1[j][k],
                                     1e-12,
                                     "Unexpected value after elementwise multiplication");
                    Assert::AreEqual(m1[j][k],
                                     result2[j][k],
                                     2e-12,
                                     "Unexpected value after elementwise division");
                }
            }
        }
    }

    // Complex operations

    void adjoint() {
        std::size_t m = 30;
        std::size_t n = 30;
        Matrix matrix(m, n);
        for (std::size_t i = 0; i < m; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                matrix[i][j] = dist(rng);
            }
        }

        Matrix e = matrix.cofactors().transpose();
        Matrix a = matrix.adjoint();
        Assert::AreEqual(e, a, "Unexpected value");

        try {
            matrix = Matrix(2, 3);
            matrix.adjoint();
            Assert::Fail("Failed to throw on non-square matrix.");
        }
        catch (std::domain_error) {
        }
    }

    void cofactors() {
        std::size_t m = 20;
        std::size_t n = 20;
        Matrix matrix(m, n);
        for (std::size_t i = 0; i < m; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                matrix[i][j] = dist(rng);
            }
        }

        for (std::size_t i = 0; i < m; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                double e = matrix.minor(i, j) * std::pow(-1, i + j + 2);
                double a = matrix.cofactor(i, j);
                Assert::AreEqual(e, a, 1e-12, "Unexpected value");
            }
        }

        try {
            matrix = Matrix(2, 3);
            matrix.cofactors();
            Assert::Fail("Failed to throw on non-square matrix.");
        }
        catch (std::domain_error) {
        }
    }

    // Slow recursive function to compute determinants using laplace expansion.
    // Used to verify the correctness of more complex determinant algorithms.
    double laplaceDeterminant(const Matrix& m) {
        if (!m.isSquare()) {
            throw std::domain_error("Matrix must be square");
        }
        if (m.rows() == 1) {
            return m[0][0];
        }
        if (m.rows() == 2) {
            return (m[0][0] * m[1][1]) - (m[0][1] * m[1][0]);
        }

        double r = 0;
        for (size_t i = 0; i < m.rows(); i++) {
            double f = std::pow(-1.0, i) * m[i][0];
            Matrix sub = m.submatrix(i, 0);
            double d = laplaceDeterminant(sub);
            r += f * d;
        }
        return r;
    }

    void determinant() {
        Matrix m1(2, 2, {1, 0, 0, 1});
        Assert::AreEqual(1, m1.determinant(), 1e-12, "2x2 Matrix");
        Matrix m2(3, 3, {1, 0, 0, 0, 3, 1, 0, 2, 4});
        Assert::AreEqual(10, m2.determinant(), 1e-12, "3x3 Matrix");

        for (std::size_t i = 2; i < 8; ++i) {
            for (size_t j = 0; j < ITERATIONS * 20; ++j) {
                Matrix m(i, i);
                for (std::size_t a = 0; a < i; ++a) {
                    for (std::size_t b = 0; b < i; ++b) {
                        double d = dist(rng);
                        m[a][b] = d;
                    }
                }
                double det1 = m.determinant();
                double det2 = laplaceDeterminant(m);
                double e = std::abs((det1 + det2) / 2e8);
                Assert::AreEqual(det1, det2, e, "Random determinants");
            }
            for (size_t j = 0; j < ITERATIONS * 2; j++) {
                // Test some symetric matrices to excercise cholesky code
                Matrix m(i, i);
                for (std::size_t a = 0; a < i; ++a) {
                    for (std::size_t b = a; b < i; ++b) {
                        double d = dist(rng);
                        m[a][b] = d;
                    }
                    for (std::size_t b = 0; b < a; ++b) {
                        m[b][a] = m[a][b];
                    }
                }
                double det1 = m.determinant();
                double det2 = laplaceDeterminant(m);
                double e = std::abs((det1 + det2) / 2e9);
                Assert::AreEqual(det1, det2, e, "Symetric random determinants");
            }
        }
    }

    void laplaceDet() {
        Matrix m1(3, 3, {0, 0, 0, 0, 4, 5, 0, 7, 8});
        Assert::AreEqual(0, m1.determinant(), "Laplace determinant calculation wrong");
    }

    void diagonal() {
        Matrix m1(2, 2, {1, 0, 0, 1});
        Matrix sol1(2, 1, {1, 1});
        m1 = m1.diagonal();
        Assert::IsTrue(m1 == sol1, "Column matrix of diagonal not correct");

        Matrix m2(3, 3, {1, 0, 0, 0, 1, 0, 0, 0, 1});

        Matrix sol2(3, 1, {1, 1, 1});

        m2 = m2.diagonal();
        Assert::IsTrue(m2 == sol2, "Column matrix of diagonal not correct");
    }

    void inverse() {
        Matrix matrix(3, 3, {3, 5, 7, 19, 17, 13, 11, 3, 1});

        Matrix expected(3,
                        3,
                        {11.0 / 178.0,
                         -4.0 / 89.0,
                         27.0 / 178.0,
                         -31.0 / 89.0,
                         37.0 / 178.0,
                         -47.0 / 178.0,
                         65.0 / 178.0,
                         -23.0 / 178.0,
                         11.0 / 89.0});

        Matrix result = matrix.inverse();

        for (std::size_t j = 0; j < 3; ++j) {
            for (std::size_t k = 0; k < 3; ++k) {
                Assert::AreEqual(expected.at(j, k),
                                 result.at(j, k),
                                 1e-12,
                                 "Invalid inverse value");
            }
        }

        try {
            matrix = Matrix(2, 3);
            matrix.inverse();
            Assert::Fail("Failed to throw on non-square matrix.");
        }
        catch (std::domain_error) {
        }
    }

    void minors() {
        std::size_t m = 30;
        std::size_t n = 30;
        Matrix matrix(m, n);
        for (std::size_t i = 0; i < m; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                matrix[i][j] = dist(rng);
            }
        }

        for (std::size_t i = 0; i < m; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                double e = matrix.submatrix(i, j).determinant();
                double a = matrix.minor(i, j);
                Assert::AreEqual(e, a, 1e-12, "Unexpected value");
            }
        }

        try {
            matrix = Matrix(2, 3);
            matrix.minors();
            Assert::Fail("Failed to throw on non-square matrix.");
        }
        catch (std::domain_error) {
        }
    }

    void submatrix() {
        Matrix matrix(3, 3, {3, 5, 7, 19, 17, 13, 11, 3, 1});

        Matrix expected(2, 2, {3, 5, 19, 17});
        Assert::AreEqual(expected,
                         matrix.submatrix(2, 2),
                         "Unexpected submatrix removing last row/col");

        expected = Matrix(2, 2, {17, 13, 3, 1});
        Assert::AreEqual(expected,
                         matrix.submatrix(0, 0),
                         "Unexpected submatrix removing first row/col");

        expected = Matrix(2, 2, {3, 7, 11, 1});
        Assert::AreEqual(expected,
                         matrix.submatrix(1, 1),
                         "Unexpected submatrix removing middle row/col");

        try {
            matrix.submatrix(3, 1);
            Assert::Fail("Succesfully removed non-existant row.");
        }
        catch (std::out_of_range) {
        }

        try {
            matrix.submatrix(1, 3);
            Assert::Fail("Succesfully removed non-existant column.");
        }
        catch (std::out_of_range) {
        }
    }

    void transpose() {
        const std::size_t m = 20;
        const std::size_t n = 10;
        for (std::size_t i = 0; i < ITERATIONS; ++i) {
            Matrix matrix(m, n);
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    matrix[j][k] = dist(rng);
                }
            }

            Matrix result = matrix.transpose();
            Assert::AreEqual(matrix.rows(), result.cols(), "Unexpected number of columns");
            Assert::AreEqual(matrix.cols(), result.rows(), "Unexpected number of rows");
            for (std::size_t j = 0; j < m; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    Assert::AreEqual(matrix.at(j, k), result.at(k, j), 1e-12, "Unexpected value");
                }
            }
        }
    }

    void identity() {
        const std::size_t m = 2;
        const std::size_t n = 2;
        Matrix expected(m, n, {1, 0, 0, 1});
        Matrix actual(Matrix::identity(m));
        Assert::AreEqual(expected, actual, "Actual matrix not an identity");
    }

    void cholesky() {
        Matrix matrix(3, 3, {25, 15, -5, 15, 18, 0, -5, 0, 11});

        Matrix e(3, 3, {5, 0, 0, 3, 3, 0, -1, 1, 3});

        Matrix a = matrix.chol();

        Assert::AreEqual(e, a, "Unexpected value");
    }

    void weightedmean() {
        Matrix matrix(3, 2, {1, 2, 3, 4, 5, 6});
        Matrix w(2, 1, {0.2, 0.8});

        Matrix e(3, 1, {1.8, 3.8, 5.8});
        Matrix a = matrix.weightedMean(w);

        Assert::AreEqual(e, a, "Unexpected values");

        Matrix w2(3, 1, {0.2, 0.8, 0.1});
        Matrix w3(1, 1, {0.2});
        try {
            matrix.weightedMean(w2);
            matrix.weightedMean(w3);
            Assert::Fail("Calculated weighted mean with wrong number of weights");
        }
        catch (std::domain_error&) {
        }
    }

    void weightedcovariance() {
        Matrix matrix(3, 2, {1, 2, 3, 4, 5, 6});
        Matrix w(2, 1, {0.2, 0.8});

        Matrix e(3, 3, {0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16});
        Matrix a = matrix.weightedCovariance(w, 1);

        Assert::AreEqual(e, a, "Unexpected value");

        Matrix w2(3, 1, {0.2, 0.8, 0});
        Matrix w3(1, 1, {0.2});
        try {
            matrix.weightedCovariance(w2, 1);
            matrix.weightedCovariance(w3, 1);
            Assert::Fail("Calculated weighted covariance with wrong number of weights");
        }
        catch (std::domain_error) {
        }
    }

    void streamInsertionOperator() {
        Matrix m1(3, 2, {1, 2, 3, 4, 5, 6});

        Matrix m2;
        try {
            std::stringstream ss;
            ss << m1 << std::endl;
            ss << m2 << std::endl;
        }
        catch (...) {
            Assert::Fail("Exception occurred in left bitwise shift.");
        }
    }

    void registerTests(PCOE::Test::TestContext& context) {
        // Matrix Creation
        context.AddTest("Default Constructor", construct_default, "Matrix");
        context.AddTest("Sized Constructor", construct_size, "Matrix");
        context.AddTest("Inital Value Constructor", construct_initialvalue, "Matrix");
        context.AddTest("Initalizer List constructor", construct_initializerlist, "Matrix");
        context.AddTest("std::vector Constructor", construct_vector, "Matrix");
        context.AddTest("Concatenation Constructor", construct_concat, "Matrix");
        context.AddTest("Sized Concatenation Constructor", construct_concat_sized, "Matrix");
        context.AddTest("Copy Constructor", construct_copy, "Matrix");
        context.AddTest("Move Constructor", construct_move, "Matrix");
        context.AddTest("Assignment Operator", operator_assign, "Matrix");
        // Comparison operators
        context.AddTest("operator_equal", operator_equal, "Matrix");
        context.AddTest("operator_notequal", operator_notequal, "Matrix");
        context.AddTest("issquare", issquare, "Matrix");
        // Basic operations
        context.AddTest("rows", rows, "Matrix");
        context.AddTest("cols", cols, "Matrix");
        context.AddTest("indexer", indexer, "Matrix");
        context.AddTest("indexer_const", indexer_const, "Matrix");
        context.AddTest("at", at, "Matrix");
        context.AddTest("const_at", const_at, "Matrix");
        context.AddTest("col_get", col_get, "Matrix");
        context.AddTest("col_setmatrix", col_setmatrix, "Matrix");
        context.AddTest("col_setvector", col_setvector, "Matrix");
        context.AddTest("row_get", row_get, "Matrix");
        context.AddTest("row_setmatrix", row_setmatrix, "Matrix");
        context.AddTest("row_setvector", row_setvector, "Matrix");
        context.AddTest("operator_vector", operator_vector, "Matrix");
        context.AddTest("resize", resize, "Matrix");
        // Arithmetic operations
        context.AddTest("Negate Matrix", negate_matrix, "Matrix");
        context.AddTest("add_matrix", add_matrix, "Matrix");
        context.AddTest("add_scalar", add_scalar, "Matrix");
        context.AddTest("subtract_matrix", subtract_matrix, "Matrix");
        context.AddTest("subtract_salar", subtract_salar, "Matrix");
        context.AddTest("multiply_matrix", multiply_matrix, "Matrix");
        context.AddTest("multiply_scalar", multiply_scalar, "Matrix");
        context.AddTest("divide_scalar", divide_scalar, "Matrix");
        context.AddTest("modulo_by_scalar", modulo_by_scalar, "Matrix");
        context.AddTest("modulo_scalar", modulo_scalar, "Matrix");
        context.AddTest("elementwiseOperators_matrix", elementwiseOperators_matrix, "Matrix");
        // Complex operations
        context.AddTest("adjoint", adjoint, "Matrix");
        context.AddTest("cofactors", cofactors, "Matrix");
        context.AddTest("determinant", determinant, "Matrix");
        context.AddTest("laplace determinant", laplaceDet, "Matrix");
        context.AddTest("diagonal", diagonal, "Matrix");
        context.AddTest("inverse", inverse, "Matrix");
        context.AddTest("minors", minors, "Matrix");
        context.AddTest("submatrix", submatrix, "Matrix");
        context.AddTest("transpose", transpose, "Matrix");
        context.AddTest("identity", identity, "Matrix");
        // Special operations
        context.AddTest("cholesky", cholesky, "Matrix");
        context.AddTest("weightedmean", weightedmean, "Matrix");
        context.AddTest("weightedcovariance", weightedcovariance, "Matrix");
        // Stream insertion
        context.AddTest("stream insertion operator", streamInsertionOperator, "Matrix");
    }
}
