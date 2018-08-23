// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Matrix.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>

namespace PCOE {
    /***********************************************************************/
    /* Constructors, Destructor and Assignment Operator                    */
    /***********************************************************************/
    Matrix::Matrix() : M(0), N(0), data(nullptr) {}

    Matrix::Matrix(std::size_t m, std::size_t n) : M(m), N(n), data(new double[m * n]()) {}

    Matrix::Matrix(std::size_t m, std::size_t n, double value)
        : M(m), N(n), data(new double[m * n]) {
        std::fill_n(data, M * N, value);
    }

    Matrix::Matrix(std::size_t m, std::size_t n, std::initializer_list<double> l) : Matrix(m, n) {
        if (l.size() != M * N) {
            throw std::domain_error("Invalid initializer list size.");
        }
        std::size_t j = 0;
        for (auto i = l.begin(); i != l.end(); ++i) {
            data[j] = *i;
            ++j;
        }
    }

    Matrix::Matrix(const std::vector<double>& v) : Matrix(v.size(), 1) {
        std::copy(v.cbegin(), v.cend(), data);
    }

    Matrix::Matrix(const Matrix& other)
        : M(other.M), N(other.N), data(new double[other.M * other.N]) {
        std::memcpy(data, other.data, sizeof(double) * M * N);
    }

    Matrix::Matrix(Matrix&& other) : Matrix() {
        swap(*this, other);
    }

    Matrix& Matrix::operator=(Matrix other) {
        swap(*this, other);
        return *this;
    }

    void swap(Matrix& a, Matrix& b) {
        using std::swap;
        swap(a.M, b.M);
        swap(a.N, b.N);
        swap(a.data, b.data);
    }

    Matrix::~Matrix() {
        delete[] data;
    }

    /***********************************************************************/
    /* Equality operators                                                  */
    /***********************************************************************/
    bool Matrix::operator==(const Matrix& rhs) const {
        if (M != rhs.M || N != rhs.N) {
            return false;
        }
        for (size_t i = 0; i < M * N; i++) {
            if (std::abs(data[i] - rhs.data[i]) > std::numeric_limits<double>::epsilon() * 10) {
                return false;
            }
        }
        return true;
    }

    bool Matrix::operator!=(const Matrix& rhs) const {
        return !(*this == rhs);
    }

    bool Matrix::isSquare() const {
        return M == N;
    }

    /***********************************************************************/
    /* Accessors                                                           */
    /***********************************************************************/

    double& Matrix::at(std::size_t m, std::size_t n) {
        if (m >= M) {
            throw std::out_of_range("m out of range.");
        }
        if (n >= N) {
            throw std::out_of_range("n out of range.");
        }
        return data[m * N + n];
    }

    double Matrix::at(std::size_t m, std::size_t n) const {
        if (m >= M) {
            throw std::out_of_range("m out of range.");
        }
        if (n >= N) {
            throw std::out_of_range("n out of range.");
        }
        return data[m * N + n];
    }

    Matrix Matrix::col(std::size_t n) const {
        if (n >= N) {
            throw std::out_of_range("n out of range.");
        }
        Matrix r(M, 1);
        for (std::size_t i = 0; i < M; ++i) {
            r[i][0] = (*this)[i][n];
        }
        return r;
    }

    void Matrix::col(std::size_t n, const Matrix& value) {
        if (n >= N) {
            throw std::out_of_range("n out of range.");
        }
        if (value.M != M) {
            throw std::domain_error("value size does not match number of rows.");
        }
        if (value.N != 1) {
            throw std::domain_error("value is nto a column vector.");
        }
        for (std::size_t i = 0; i < M; ++i) {
            (*this)[i][n] = value[i][0];
        }
    }

    void Matrix::col(std::size_t n, const std::vector<double>& value) {
        if (n >= N) {
            throw std::out_of_range("n out of range.");
        }
        if (value.size() != M) {
            throw std::domain_error("value size does not match number of rows.");
        }
        for (std::size_t i = 0; i < M; ++i) {
            (*this)[i][n] = value[i];
        }
    }

    Matrix Matrix::row(std::size_t m) const {
        if (m >= M) {
            throw std::out_of_range("m out of range.");
        }
        Matrix r(1, N);
        for (std::size_t i = 0; i < N; ++i) {
            r[0][i] = (*this)[m][i];
        }
        return r;
    }

    void Matrix::row(std::size_t m, const Matrix& value) {
        if (m >= M) {
            throw std::out_of_range("m out of range.");
        }
        if (value.N != N) {
            throw std::domain_error("value size does not match number of columns.");
        }
        if (value.M != 1) {
            throw std::domain_error("value is not a row vector.");
        }
        for (std::size_t i = 0; i < N; ++i) {
            (*this)[m][i] = value[0][i];
        }
    }

    void Matrix::row(std::size_t m, const std::vector<double>& value) {
        if (m >= M) {
            throw std::out_of_range("m out of range.");
        }
        if (value.size() != N) {
            throw std::domain_error("value size does not match number of columns.");
        }
        for (std::size_t i = 0; i < N; ++i) {
            (*this)[m][i] = value[i];
        }
    }

    Matrix::operator std::vector<double>() const {
        if (M != 1 && N != 1) {
            throw std::domain_error("Matrix is not a vector.");
        }
        return std::vector<double>(data, data + M * N);
    }

    void Matrix::resize(std::size_t m, std::size_t n) {
        double* newData = new double[m * n];
        std::size_t mm = std::min(M, m);
        std::size_t nn = std::min(N, n);
        for (std::size_t i = 0; i < mm; ++i) {
            for (std::size_t j = 0; j < nn; ++j) {
                newData[i * n + j] = data[i * N + j];
            }
        }
        delete[] data;
        data = newData;
        M = m;
        N = n;
    }

    /***********************************************************************/
    /* Arithmetic operators                                                */
    /***********************************************************************/
    Matrix Matrix::operator-() {
        Matrix result = *this;
        for (size_t i = 0; i < M * N; i++) {
            result.data[i] = -result.data[i];
        }
        return result;
    }

    Matrix& Matrix::operator+=(const Matrix& rhs) {
        if (M != rhs.M || N != rhs.N) {
            throw std::domain_error("Matrices are different sizes.");
        }
        for (size_t i = 0; i < M * N; i++) {
            data[i] += rhs.data[i];
        }
        return *this;
    }

    Matrix& Matrix::operator+=(double rhs) {
        for (size_t i = 0; i < M * N; i++) {
            data[i] += rhs;
        }
        return *this;
    }

    Matrix& Matrix::operator-=(const Matrix& rhs) {
        if (M != rhs.M || N != rhs.N) {
            throw std::domain_error("Matrices are different sizes.");
        }
        for (size_t i = 0; i < M * N; i++) {
            data[i] -= rhs.data[i];
        }
        return *this;
    }

    Matrix& Matrix::operator-=(double other) {
        for (size_t i = 0; i < M * N; i++) {
            data[i] -= other;
        }
        return *this;
    }

    Matrix Matrix::operator*(const Matrix& rhs) const {
        if (N != rhs.M) {
            throw std::domain_error("Matrices are compatible.");
        }
        Matrix r(M, rhs.N);
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < rhs.N; j++) {
                double e = 0;
                for (size_t k = 0; k < N; k++) {
                    e += ((*this)[i][k] * rhs[k][j]);
                }
                r[i][j] = e;
            }
        }
        return r;
    }

    Matrix& Matrix::operator*=(double rhs) {
        for (size_t i = 0; i < M * N; i++) {
            data[i] *= rhs;
        }
        return *this;
    }

    Matrix& Matrix::operator/=(double rhs) {
        for (size_t i = 0; i < M * N; i++) {
            data[i] /= rhs;
        }
        return *this;
    }

    /***********************************************************************/
    /* Operations                                                          */
    /***********************************************************************/
    Matrix Matrix::adjoint() const {
        return cofactors().transpose();
    }

    Matrix Matrix::chol() const {
        if (M != N) {
            throw std::domain_error("Matrix must be square");
        }

        Matrix r(M, N);
        if (!cholInternal(r)) {
            throw std::domain_error("Matrix is not positive definite");
        }
        return r;
    }

    double Matrix::cofactor(std::size_t i, std::size_t j) const {
        return minor(i, j) * std::pow(-1, i + j + 2);
    }

    Matrix Matrix::cofactors() const {
        Matrix r(M, N);
        for (std::size_t i = 0; i < M; ++i) {
            for (std::size_t j = 0; j < N; ++j) {
                r[i][j] = cofactor(i, j);
            }
        }
        return r;
    }

    double Matrix::determinant() const {
        if (M != N) {
            throw std::domain_error("Matrix must be square");
        }

        // Try to do a crout decomposition. If the decomposition isn't possible,
        // fall back to laplace expansion to calculate the determinant.
        double r = 1;
        Matrix A(M, N);
        // if (cholInternal(A)) {
        //	for (std::size_t i = 0; i < N; i++) {
        //		r *= A(i, i) * A[i][i];
        //	}
        //	return r;
        //}
        if (croutInternal(A, r)) {
            for (std::size_t i = 0; i < N; i++) {
                r *= A[i][i];
            }
            return r;
        }
        return laplaceDet();
    }

    Matrix Matrix::diagonal() const {
        std::size_t len = std::min(M, N);
        Matrix r(len, 1);
        for (size_t i = 0; i < len; i++) {
            r[i][0] = (*this)[i][i];
        }
        return r;
    }

    Matrix Matrix::inverse() const {
        if (M != N) {
            throw std::domain_error("Matrix must be square");
        }

        double d = determinant();
        if (std::abs(d) < 1e-15) {
            throw std::domain_error("Matrix is singular.");
        }

        return adjoint() / d;
    }

    double Matrix::minor(std::size_t m, std::size_t n) const {
        return submatrix(m, n).determinant();
    }

    Matrix Matrix::minors() const {
        Matrix r(M, N);
        for (std::size_t i = 0; i < M; ++i) {
            for (std::size_t j = 0; j < N; ++j) {
                r[i][j] = this->minor(i, j);
            }
        }
        return r;
    }

    Matrix Matrix::submatrix(std::size_t m, std::size_t n) const {
        if (m >= M) {
            throw std::out_of_range("m is too big.");
        }
        if (n >= N) {
            throw std::out_of_range("n is too big.");
        }
        Matrix r(M - 1, N - 1);
        std::size_t i_off = 0;
        for (std::size_t i = 0; i < M; ++i) {
            if (i == m) {
                i_off = 1;
                continue;
            }
            std::size_t j_off = 0;
            for (std::size_t j = 0; j < N; ++j) {
                if (j == n) {
                    j_off = 1;
                    continue;
                }
                r[i - i_off][j - j_off] = (*this)[i][j];
            }
        }
        return r;
    }

    Matrix Matrix::transpose() const {
        Matrix r(N, M);
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                r[j][i] = (*this)[i][j];
            }
        }
        return r;
    }

    // Weighted covariance. Alpha is a scaling factor and defaults to 1.
    // Beta is also a scaling parameter and defaults to 0. For default values
    // there will be no scaling.
    // Note that weighted covariance function differs for sigma
    // points vs other samples, and this one is assuming sigma points.
    Matrix Matrix::weightedCovariance(const Matrix& w,
                                      const double alpha,
                                      const double beta) const {
        if (N != w.M || w.N != 1) {
            throw std::domain_error("w is not a column vector with M rows.");
        }

        Matrix result(M, M);

        Matrix wmean = weightedMean(w);

        // Compute covariance
        for (std::size_t i = 0; i < N; ++i) {
            Matrix column = col(i);
            Matrix diff = column - wmean;
            Matrix diffT = diff.transpose();
            Matrix temp = diff * diffT;
            result = result + temp * w[i][0];

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
        if (w.M != N || w.N != 1) {
            throw std::domain_error("w is not a column vector with M rows.");
        }
        return (*this) * w;
    }

    /***********************************************************************/
    /* Static Operations                                                   */
    /***********************************************************************/
    Matrix Matrix::identity(std::size_t m) {
        Matrix r(m, m);
        for (size_t i = 0; i < m; i++) {
            r[i][i] = 1;
        }
        return r;
    }

    /***********************************************************************/
    /* Steam Insertion                                                     */
    /***********************************************************************/
    std::ostream& operator<<(std::ostream& os, const Matrix& obj) {
        for (size_t i = 0; i < obj.M; ++i) {
            for (size_t j = 0; j < obj.N; ++j) {
                os << obj[i][j] << " ";
            }
            os << std::endl;
        }
        return os;
    }

    double Matrix::laplaceDet() const {
        if (M != N) {
            throw std::domain_error("Matrix must be square");
        }
        if (rows() == 1) {
            return *data;
        }
        if (rows() == 2) {
            return (data[0] * data[3] - data[1] * data[2]);
        }

        double r = 0;
        for (size_t i = 0; i < rows(); i++) {
            double f = std::pow(-1.0, i) * (*this)[i][0];
            Matrix sub = submatrix(i, 0);
            double d = sub.determinant();
            r += f * d;
        }
        return r;
    }

    bool Matrix::cholInternal(Matrix& r) const {
        for (std::size_t i = 1; i < M; ++i) {
            for (std::size_t j = 0; j < i; ++j) {
                if (std::abs((*this)[i][j] - (*this)[j][i]) > 1e-15) {
                    return false;
                }
            }
        }

        for (std::size_t k = 0; k < M; ++k) {
            double sum = 0;
            for (std::size_t p = 0; p < k; ++p) {
                sum += r[k][p] * r[k][p];
            }
            r[k][k] = std::sqrt((*this)[k][k] - sum);
            for (std::size_t i = k + 1; i < M; ++i) {
                double s = 0;
                for (std::size_t p = 0; p < k; ++p) {
                    s += r[i][p] * r[k][p];
                }
                r[i][k] = ((*this)[i][k] - s) / r[k][k];
            }
        }

        for (size_t i = 0; i < M * N; i++) {
            if (std::isnan(r.data[i]) || std::isinf(r.data[i])) {
                return false;
            }
        }
        return true;
    }

    bool Matrix::croutInternal(Matrix& r, double& mult) const {
        // If the first element of the matrix is 0, we have to interchange two
        // rows or columns so that the first element is non-zero, or the LU
        // decomposition algorithm will fail.
        // Each interchange flips the sign of the detrminant, so we start with a
        // result of 1, and change the result to -1 if we need to perform an
        // interchange. This result is later multiplied with the element along the
        // diagonal of the decomposed matrix.
        mult = 1;
        r = (*this);
        if (std::abs((*this)[0][0]) < 1e-15) {
            std::size_t i;
            for (i = 0; i < N; ++i) {
                if (std::abs(r[i][0]) > 1e-15) {
                    Matrix tmp = r.row(0);
                    r.row(0, r.row(i));
                    r.row(i, tmp);
                    mult = -1;
                    break;
                }
            }
            if (i == N) { // Failed to find a row with non-zero in first column
                std::size_t j;
                for (j = 0; j < M; ++j) {
                    if (std::abs(r[0][j]) > 1e-15) {
                        Matrix tmp = r.col(0);
                        r.col(0, r.col(j));
                        r.col(j, tmp);
                        mult = -1;
                        break;
                    }
                }
                if (j == M) { // Failed to find a column with non-zero first row
                    return false;
                }
            }
        }

        // Crout decomposition. Both L and U can be placed in the original copy
        // without overwriting and values from the original matrix that are still
        // needed by the algorithm.
        for (std::size_t k = 0; k < N; ++k) {
            for (std::size_t i = k; i < N; ++i) {
                double sum = 0;
                for (std::size_t p = 0; p < k; ++p) {
                    sum += r[i][p] * r[p][k];
                }
                r[i][k] = r[i][k] - sum;
            }
            for (std::size_t j = k + 1; j < N; ++j) {
                double sum = 0;
                for (std::size_t p = 0; p < k; ++p) {
                    sum += r[k][p] * r[p][j];
                }
                r[k][j] = (r[k][j] - sum) / r[k][k];
            }
        }

        return true;
    }
}
