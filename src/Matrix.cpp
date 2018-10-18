// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Matrix.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <numeric>
#include <stdexcept>

#include "Contracts.h"
#include "Eigen/Cholesky"

namespace PCOE {
    /***********************************************************************/
    /* Constructors, Destructor and Assignment Operator                    */
    /***********************************************************************/
    Matrix::Matrix() : matrix(0, 0) {}

    // Note (JW): Note the parens at the end of the array allocation. This
    // value-initializes the array.
    Matrix::Matrix(std::size_t m, std::size_t n) : matrix(Eigen::MatrixXd::Zero(m, n)) {}

    Matrix::Matrix(std::size_t m, std::size_t n, double value)
        : matrix(Eigen::MatrixXd::Constant(m, n, value)) {}

    Matrix::Matrix(std::size_t m, std::size_t n, std::initializer_list<double> l) : Matrix(m, n) {
        Expect(l.size() == m * n, "Invalid initializer list size.");

        auto it = l.begin();
        for (std::size_t i = 0; i < m; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                matrix(i, j) = *it;
                ++it;
            }
        }
    }

    Matrix::Matrix(std::initializer_list<std::reference_wrapper<const Matrix>> l) {
        Expect(l.size() > 0, "Concat constructor called with empty list");
        std::size_t m = l.begin()->get().rows();
        std::size_t n = l.begin()->get().cols();
        bool allSameSize = std::all_of(l.begin(), l.end(), [=](const Matrix& i) {
            return i.rows() == m && i.cols() == n;
        });
        if (allSameSize) {
            throw std::domain_error(
                "Ambiguous matrix construction. Please specify the desired matrix dimensions");
        }

        // First, try concatenating by columns
        bool rowsMatch =
            std::none_of(l.begin(), l.end(), [=](const Matrix& i) { return i.rows() != m; });
        if (rowsMatch) {
            std::size_t rows = m;
            std::size_t cols =
                std::accumulate(l.begin(), l.end(), 0u, [](std::size_t i, const Matrix& j) {
                    return i + j.cols();
                });
            matrix.resize(rows, cols);

            // Everything matches up to concatenate by column
            // TODO (JW): Optimize me
            std::size_t i = 0;
            for (const Matrix& matrix : l) {
                for (std::size_t j = 0; j < matrix.cols(); ++j) {
                    this->col(i, matrix.col(j));
                    ++i;
                }
            }
            return;
        }

        // Concatenating by column didn't work, try concatenating by row
        bool colsMatch =
            std::none_of(l.begin(), l.end(), [=](const Matrix& i) { return i.cols() != n; });
        if (colsMatch) {
            std::size_t rows =
                std::accumulate(l.begin(), l.end(), 0u, [](std::size_t i, const Matrix& j) {
                    return i + j.rows();
                });
            std::size_t cols = n;
            matrix.resize(rows, cols);

            // Everything matches up to concatenate by column
            // TODO (JW): Optimize me
            std::size_t i = 0;
            for (const Matrix& matrix : l) {
                for (std::size_t j = 0; j < matrix.rows(); ++j) {
                    this->row(i, matrix.row(j));
                    ++i;
                }
            }
            return;
        }

        // Neither concatenation worked, so fail.
        throw std::domain_error("The provided matrices do not fit in the specified dimensions");
    }

    Matrix::Matrix(std::size_t m,
                   std::size_t n,
                   std::initializer_list<std::reference_wrapper<const Matrix>> l)
        : Matrix(m, n) {
        Expect(l.size() > 0, "Sized concat constructor called with empty list");
        // First, try concatenating by columns
        bool rowsMatch =
            std::none_of(l.begin(), l.end(), [=](const Matrix& i) { return i.rows() != m; });
        std::size_t columnCount =
            std::accumulate(l.begin(), l.end(), 0u, [](std::size_t i, const Matrix& j) {
                return i + j.cols();
            });
        if (rowsMatch && columnCount == n) {
            // Everything matches up to concatenate by column
            // TODO (JW): Optimize me
            std::size_t i = 0;
            for (const Matrix& matrix : l) {
                for (std::size_t j = 0; j < matrix.cols(); ++j) {
                    this->col(i, matrix.col(j));
                    ++i;
                }
            }
            return;
        }

        // Concatenating by column didn't work, try concatenating by row
        bool colsMatch =
            std::none_of(l.begin(), l.end(), [=](const Matrix& i) { return i.cols() != n; });
        std::size_t rowCount =
            std::accumulate(l.begin(), l.end(), 0u, [](std::size_t i, const Matrix& j) {
                return i + j.rows();
            });
        if (colsMatch && rowCount == m) {
            // Everything matches up to concatenate by column
            // TODO (JW): Optimize me
            std::size_t i = 0;
            for (const Matrix& matrix : l) {
                for (std::size_t j = 0; j < matrix.rows(); ++j) {
                    this->row(i, matrix.row(j));
                    ++i;
                }
            }
            return;
        }

        // Neither concatenation worked, so fail.
        throw std::domain_error("The provided matrices do not fit in the specified dimensions");
    }

    Matrix::Matrix(const std::vector<double>& v) : Matrix(v.size(), 1) {
        auto it = v.begin();
        for (std::size_t i = 0; i < v.size(); ++i) {
            matrix(i, 0) = *it;
            ++it;
        }
    }

    Matrix::Matrix(const Matrix& other) : matrix(other.matrix) {}

    Matrix::Matrix(Matrix&& other) : Matrix() {
        if (*this == other) {
            return;
        }
        swap(*this, other);
    }

    Matrix& Matrix::operator=(Matrix other) {
        swap(*this, other);
        return *this;
    }

    void swap(Matrix& a, Matrix& b) {
        using std::swap;
        swap(a.matrix, b.matrix);
    }

    Matrix::~Matrix() {}

    /***********************************************************************/
    /* Equality operators                                                  */
    /***********************************************************************/
    bool Matrix::operator==(const Matrix& rhs) const {
        if (rows() != rhs.rows() || cols() != rhs.cols()) {
            return false;
        }
        return matrix.isApprox(rhs.matrix, std::numeric_limits<double>::epsilon() * 10);
    }

    bool Matrix::operator!=(const Matrix& rhs) const {
        return !(*this == rhs);
    }

    bool Matrix::isSquare() const {
        return matrix.rows() == matrix.cols();
    }

    /***********************************************************************/
    /* Accessors                                                           */
    /***********************************************************************/

    double& Matrix::at(std::size_t m, std::size_t n) {
        Expect(m < matrix.rows(), "m out of range");
        Expect(n < matrix.cols(), "n out of range");
        return matrix(m, n);
    }

    double Matrix::at(std::size_t m, std::size_t n) const {
        Expect(m < matrix.rows(), "m out of range");
        Expect(n < matrix.cols(), "n out of range");
        return matrix(m, n);
    }

    Matrix Matrix::col(std::size_t n) const {
        Expect(n < matrix.cols(), "n out of range");
        return matrix.col(n);
    }

    void Matrix::col(std::size_t n, const Matrix& value) {
        Expect(n < matrix.cols(), "n out of range");
        Expect(value.rows() == matrix.rows(), "value rows does not match number of rows.");
        Expect(value.cols() == 1, "value is not a column vector");
        matrix.col(n) = value.matrix;
    }

    void Matrix::col(std::size_t n, const std::vector<double>& value) {
        Expect(n < matrix.cols(), "n out of range.");
        Expect(value.size() == matrix.rows(), "value size does not match number of rows.");
        for (std::size_t i = 0; i < matrix.rows(); ++i) {
            matrix(i, n) = value[i];
        }
    }

    Matrix Matrix::row(std::size_t m) const {
        Expect(m < matrix.rows(), "m out of range");
        return matrix.row(m);
    }

    void Matrix::row(std::size_t m, const Matrix& value) {
        Expect(m < matrix.rows(), "m out of range");
        Expect(value.rows() == 1, "value is not a row vector");
        Expect(value.cols() == matrix.cols(), "value cols does not match number of cols.");
        matrix.row(m) = value.matrix;
    }

    void Matrix::row(std::size_t m, const std::vector<double>& value) {
        Expect(m < matrix.rows(), "m out of range.");
        Expect(value.size() == matrix.cols(), "value size does not match number of cols.");
        for (std::size_t i = 0; i < matrix.cols(); ++i) {
            matrix(m, i) = value[i];
        }
    }

    Matrix::operator std::vector<double>() const {
        Expect(matrix.rows() == 1 || matrix.cols() == 1, "Matrix is not a vector.");
        std::vector<double> result;
        result.reserve(matrix.size());
        for (std::size_t i = 0; i < matrix.size(); i++) {
            result.push_back(matrix.coeff(i));
        }
        return result;
    }

    void Matrix::resize(std::size_t m, std::size_t n) {
        matrix.conservativeResize(m, n);
    }

    /***********************************************************************/
    /* Arithmetic operators                                                */
    /***********************************************************************/
    Matrix Matrix::operator-() {
        return -matrix;
    }

    Matrix& Matrix::operator+=(const Matrix& rhs) {
        Expect(rows() == rhs.rows() && cols() == rhs.cols(), "Size mismatch");
        matrix += rhs.matrix;
        return *this;
    }

    Matrix& Matrix::operator+=(double rhs) {
        matrix.array() += rhs;
        return *this;
    }

    Matrix& Matrix::operator-=(const Matrix& rhs) {
        Expect(rows() == rhs.rows() && cols() == rhs.cols(), "Size mismatch");
        matrix -= rhs.matrix;
        return *this;
    }

    Matrix& Matrix::operator-=(double rhs) {
        matrix.array() -= rhs;
        return *this;
    }

    Matrix Matrix::operator*(const Matrix& rhs) const {
        return matrix * rhs.matrix;
    }

    Matrix& Matrix::operator*=(double rhs) {
        matrix *= rhs;
        return *this;
    }

    Matrix& Matrix::operator/=(double rhs) {
        matrix /= rhs;
        return *this;
    }

    Matrix& Matrix::operator%=(double rhs) {
        for (std::size_t i = 0; i < rows(); ++i) {
            for (std::size_t j = 0; j < cols(); ++j) {
                matrix(i, j) = std::fmod(matrix(i, j), rhs);
            }
        }
        return *this;
    }

    Matrix Matrix::elementwiseMultiply(const Matrix& mat) const {
        Expect(rows() == mat.rows() && cols() == mat.cols(), "Size mismatch");
        return matrix.array() * mat.matrix.array();
    }

    Matrix Matrix::elementwiseDivide(const Matrix& mat) const {
        Expect(rows() == mat.rows() && cols() == mat.cols(), "Size mismatch");
        return matrix.array() / mat.matrix.array();
    }

    /***********************************************************************/
    /* Operations                                                          */
    /***********************************************************************/
    Matrix Matrix::adjoint() const {
        return matrix.adjoint();
    }

    Matrix Matrix::chol() const {
        return matrix.llt().matrixL();
    }

    double Matrix::cofactor(std::size_t i, std::size_t j) const {
        return minor(i, j) * std::pow(-1, i + j + 2);
    }

    Matrix Matrix::cofactors() const {
        Matrix r(matrix.rows(), matrix.cols());
        for (std::size_t i = 0; i < matrix.rows(); ++i) {
            for (std::size_t j = 0; j < matrix.cols(); ++j) {
                r.at(i, j) = cofactor(i, j);
            }
        }
        return r;
    }

    double Matrix::determinant() const {
        Expect(rows() == cols(), "Non-square matrix");
        return matrix.determinant();
    }

    Matrix Matrix::diagonal() const {
        return matrix.diagonal();
    }

    Matrix Matrix::inverse() const {
        Expect(rows() == cols(), "Non-square matrix");
        return matrix.inverse();
    }

    double Matrix::minor(std::size_t m, std::size_t n) const {
        return submatrix(m, n).determinant();
    }

    Matrix Matrix::minors() const {
        Matrix r(matrix.rows(), matrix.cols());
        for (std::size_t i = 0; i < matrix.rows(); ++i) {
            for (std::size_t j = 0; j < matrix.cols(); ++j) {
                r.at(i, j) = this->minor(i, j);
            }
        }
        return r;
    }

    Matrix Matrix::submatrix(std::size_t m, std::size_t n) const {
        Expect(m < matrix.rows(), "m is too big.");
        Expect(n < matrix.cols(), "n is too big.");
        Matrix r(matrix.rows() - 1, matrix.cols() - 1);
        std::size_t i_off = 0;
        for (std::size_t i = 0; i < matrix.rows(); ++i) {
            if (i == m) {
                i_off = 1;
                continue;
            }
            std::size_t j_off = 0;
            for (std::size_t j = 0; j < matrix.cols(); ++j) {
                if (j == n) {
                    j_off = 1;
                    continue;
                }
                r.at(i - i_off, j - j_off) = (*this).at(i, j);
            }
        }
        return r;
    }

    Matrix Matrix::transpose() const {
        return matrix.transpose();
    }

    // Weighted covariance. Alpha is a scaling factor and defaults to 1.
    // Beta is also a scaling parameter and defaults to 0. For default values
    // there will be no scaling.
    // Note that weighted covariance function differs for sigma
    // points vs other samples, and this one is assuming sigma points.
    Matrix Matrix::weightedCovariance(const Matrix& w,
                                      const double alpha,
                                      const double beta) const {
        if (matrix.cols() != w.matrix.rows() || w.matrix.cols() != 1) {
            throw std::domain_error("w is not a column vector with matrix.rows() rows.");
        }

        Matrix result(matrix.rows(), matrix.rows());

        Matrix wmean = weightedMean(w);

        // Compute covariance
        for (std::size_t i = 0; i < matrix.cols(); ++i) {
            Matrix column = col(i);
            Matrix diff = column - wmean;
            Matrix diffT = diff.transpose();
            Matrix temp = diff * diffT;
            result = result + temp * w.at(i, 0);

            // Offset with alpha term
            column = col(0);
            diff = column - wmean;
            diffT = diff.transpose();
            temp = diff * diffT;
            result += temp * (1 - alpha * alpha + beta);
        }

        return result;
    }

    Matrix Matrix::weightedMean(const Matrix& w) const {
        if (w.matrix.rows() != matrix.cols() || w.matrix.cols() != 1) {
            throw std::domain_error("w is not a column vector with matrix.rows() rows.");
        }
        return (*this) * w;
    }

    /***********************************************************************/
    /* Static Operations                                                   */
    /***********************************************************************/
    Matrix Matrix::identity(std::size_t m) {
        return Eigen::MatrixXd::Identity(m, m);
    }

    /***********************************************************************/
    /* Steam Insertion                                                     */
    /***********************************************************************/
    std::ostream& operator<<(std::ostream& os, const Matrix& obj) {
        for (std::size_t i = 0; i < obj.matrix.rows(); ++i) {
            for (std::size_t j = 0; j < obj.matrix.cols(); ++j) {
                os << obj.at(i, j) << " ";
            }
            os << std::endl;
        }
        return os;
    }
}
