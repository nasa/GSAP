/**  @file      Matrix.h
 *
 *   @brief     An arbitrary MxN matrix
 *
 *   @author    Jason Watkins <jason-watkins@outlook.com>
 *   @author    Matthew Daigle <matthew.j.daigle@nasa.gov>
 *   @version   0.2.0
 *   @date      2016-07-25
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *              the Administrator of the National Aeronautics and Space
 *              Administration. All Rights Reserved.
 */

#ifndef GSAP_MATRIX_H
#define GSAP_MATRIX_H

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <vector>

namespace PCOE {
#undef minor
    class Matrix {
    public:
        struct ConstRowVector;
        struct RowVector;
        /***********************************************************************/
        /* Constructors, Destructor and Assignment Operator                    */
        /***********************************************************************/

        /** @brief Constructs a new empty Matrix.
         */
        Matrix();

        /** @brief Constructs a new Matrix with storage for m by n elements.
         *         Elements are value-initialized.
         *
         *  @param m The number of rows in the matrix.
         *  @param n The number of columns in the matrix.
         */
        Matrix(std::size_t m, std::size_t n);

        /** @brief Constructs a new Matrix with storage for m by n elements. Elements
         *         are initialized to @p value.
         *
         *  @param m The number of rows in the matrix.
         *  @param n The number of columns in the matrix.
         *  @param value The initial value of elements in the matrix.
         */
        Matrix(std::size_t m, std::size_t n, double value);

        /** @brief Constructs a new Matrix with storage for m by n elements.
         *         Elements are initialized sequentially in row-major order by the
         *         elements in @p l
         *
         *  @param m     The number of rows in the matrix.
         *  @param n     The number of columns in the matrix.
         *  @param value The initial value of elements in the matrix.
         */
        Matrix(std::size_t m, std::size_t n, std::initializer_list<double> l);

        /** @brief Constructs a new Matrix representing a column vector with
         *         the elements of @p v.
         *
         *  @param v The values that will make up the vector
         */
        explicit Matrix(const std::vector<double>& v);

        /** @brief Constructs a new Matrix by copying the elements of @p other.
         *
         *  @param other A matrix of the same type and size from which to copy
         *               elements.
         */
        Matrix(const Matrix& other);

        /** @brief Constructs a new Matrix by taking the elements of @p other.
        *
        *   @param other A matrix of the same type and size from which to take
        *                elements.
        */
        Matrix(Matrix&& other);

        /** @brief Assigns the elements of other to the current Matrix.
        *
        *   @param other A matrix from which to copy elements.
        *   @returns     A reference to the current Matrix.
        */
        Matrix& operator=(Matrix other);

        /** @brief Exchanges the underlying data of two Matrices.
         *
         *  @param a The first matrix to swap.
         *  @param b The second matrix to swap.
         */
        friend void swap(Matrix& a, Matrix& b);

        /** @brief Frees the underlying resources of the Matrix.
         */
        ~Matrix();

        /***********************************************************************/
        /* Equality operators                                                  */
        /***********************************************************************/
        /** @brief Determines whether the specified matrix is equal to the
         *         current matrix.
         *
         *  @param rhs The matrix to compare with the other object.
         *  @returns   true if the matrices are the same size and contain the same
         *             elements in each position; otherwise, false.
         */
        bool operator==(const Matrix& rhs) const;

        /** @brief Determines whether the specified matrix is not equal to the
         *         current matrix.
         *
         *  @param rhs The matrix to compare with the other object.
         *  @returns   true if the matrices are different sizes or contain
         *             differing elements in any position; otherwise, false.
         */
        bool operator!=(const Matrix& rhs) const;

        /** @brief Determines whether the current matrix is square.
         *
         *  @returns true if the current matrix has the same number of rows and
         *           columns; otherwise, false.
         */
        bool isSquare() const;

        /***********************************************************************/
        /* Accessors                                                           */
        /***********************************************************************/
        /** @brief Gets the number of rows in the current matrix. */
        inline std::size_t rows() const {
            return M;
        }

        /** @brief Gets the number of columns in the current matrix. */
        inline std::size_t cols() const {
            return N;
        }

        /** @brief Gets a mutable row vector that can be further indexed to get a
         *         reference to an element in the matrix.
         *
         *  @remarks The indexing operator does not perform bounds checking.
         *           attempting to access an element outside the bounds of the
         *           matrix is undefined behavior. For a version that throws on
         *           out-of-bounds access, use the @see{at} method.
         *
         *  @param m The zero-based row to get.
         */
        inline RowVector operator[](std::size_t m) {
            return RowVector(data + (m * N));
        }

        /** @brief Gets an immutable row vector that can be further indexed to get
         *         an element in the matrix.
         *
         *  @remarks The indexing operator does not perform bounds checking.
         *           attempting to access an element outside the bounds of the
         *           matrix is undefined behavior. For a version that throws on
         *           out-of-bounds access, use the @see{at} method.
         *
         *  @param m The zero-based row to get.
         */
        inline ConstRowVector operator[](std::size_t m) const {
            return ConstRowVector(data + (m * N));
        }

        /** @brief Gets a reference the element at the specified location.
        *
        *  @param m The zero-based row of the element to get.
        *  @param n The zero-nased column of the element to get.
        *  @exception std::out_of_range If @p m or @p n are larger than the number
        *             of rows or columns, respectively, in the matrix.
        */
        double& at(std::size_t m, std::size_t n);

        /** @brief Gets the value of the element at the specified location.
        *
        *  @param m The zero-based row of the element to get.
        *  @param n The zero-nased column of the element to get.
        *  @exception std::out_of_range If @p m or @p n are larger than the number
        *             of rows or columns, respectively, in the matrix.
        */
        double at(std::size_t m, std::size_t n) const;

        /** @brief Retrieves the n-th column of the matrix as a column vector.
         *
         *  @param n The zero-based column of the matrix to retrive.
         *  @returns An M by 1 Matrix containing the elments of the n-th column.
         *  @exception std::out_of_range If @p m or @p n are larger than the number
         *             of rows or columns, respectively, in the matrix.
         */
        Matrix col(std::size_t n) const;

        /** @brief Sets the n-th column of the matrix to the given column vector.
         *
         *  @param n     The zero-based column of the matrix to set.
         *  @param value An M by 1 Matrix containing the elments to set the n-th
         *               column to.
         *  @exception std::out_of_range If @p n is larger than the number of
         *             columns in the matrix.
         *  @exception std::domain_error If the size of @p value is not M by 1.
         */
        void col(std::size_t n, const Matrix& value);

        /** @brief Sets the n-th column of the matrix to the given column vector.
         *
         *  @param n     The zero-based column of the matrix to set.
         *  @param value A std::vector containing the elments to set the n-th
         *               column to.
         *  @exception std::out_of_range If @p n is larger than the number of
         *             columns in the matrix.
         *  @exception std::domain_error If the size of @p value is not M.
         */
        void col(std::size_t n, const std::vector<double>& value);

        /** @brief Retrieves the m-th row of the matrix as a row vector.
         *
         *  @param m The zero-based row of the matrix to retrive.
         *  @returns An 1 by N Matrix containing the elments of the m-th row.
         */
        Matrix row(std::size_t m) const;

        /** @brief Sets the m-th row of the matrix to the given row vector.
         *
         *  @param m The zero-based row of the matrix to set.
         *  @param value A 1 by N Matrix containing the elments to set the m-th
         *               row to.
         *  @exception std::out_of_range If @p m is larger than the number of
         *             rows in the matrix.
         *  @exception std::domain_error If the size of @p value is not 1 by N.
         */
        void row(std::size_t m, const Matrix& value);

        /** @brief Sets the m-th row of the matrix to the given row vector.
         *
         *  @param m The zero-based row of the matrix to set.
         *  @param value A std::vector containing the elments to set the m-th
         *               row to.
         *  @exception std::out_of_range If @p m is larger than the number of
         *             rows in the matrix.
         *  @exception std::domain_error If the size of @p value is not N.
         */
        void row(std::size_t m, const std::vector<double>& value);

        explicit operator std::vector<double>() const;

        void resize(std::size_t m, std::size_t n);

        /***********************************************************************/
        /* Arithmetic operators                                                */
        /***********************************************************************/
        /** @brief Adds the specified matrix to the current matrix.
         *
         *  @param rhs The matrix to add.
         *  @returns   A reference to the current matrix.
         *  @exception std::domain_error If @p rhs is not the same size as the
         *             current matrix.
         */
        Matrix& operator+=(const Matrix& rhs);

        /** @brief Adds two matrices.
         *
         *  @param lhs The first matrix to add.
         *  @param rhs The second matrix to add.
         *  @returns   A new matrix containing the result.
         *  @exception std::domain_error If @p lhs and @p rhs are not the same size.
         */
        inline friend Matrix operator+(Matrix lhs, const Matrix& rhs) {
            return lhs += rhs;
        }

        /** @brief Adds a scalar to the current matrix.
         *
         *  @param rhs The scalar to add.
         *  @returns   A reference to the current matrix.
         */
        Matrix& operator+=(double rhs);

        /** @brief Adds a scalar to a matrix.
         *
         *  @param lhs The matrix to add to
         *  @param rhs The scalar to add.
         *  @returns   A new matrix containing the result.
         */
        inline friend Matrix operator+(Matrix lhs, double rhs) {
            return lhs += rhs;
        }

        /** @brief Adds a scalar to a matrix.
          *
         *  @param lhs The scalar to add.
         *  @param rhs The matrix to add to
         *  @returns   A new matrix containing the result.
         */
        inline friend Matrix operator+(double lhs, Matrix rhs) {
            return rhs += lhs;
        }

        /** @brief Subtracks the specified matrix to the current matrix.
         *
         *  @param rhs The matrix to subtract.
         *  @returns   A reference to the current matrix.
         *  @exception std::domain_error If @p rhs is not the same size as the
         *             current matrix.
         */
        Matrix& operator-=(const Matrix& rhs);

        /** @brief Subtracts two matrices.
         *
         *  @param lhs The first matrix to subtract.
         *  @param rhs The second matrix to subtract.
         *  @returns   A new matrix containing the result.
         *  @exception std::domain_error If @p lhs and @p rhs are not the same size.
         */
        inline friend Matrix operator-(Matrix lhs, const Matrix& rhs) {
            return lhs -= rhs;
        }

        /** @brief Subtracts a scalar from the current matrix.
         *
         *  @param rhs The scalar to subtract.
         *  @returns   A reference to the current matrix.
         */
        Matrix& operator-=(double other);

        /** @brief Subtracts a scalar from a matrix.
         *
         *  @param lhs The matrix to subtract from
         *  @param rhs The scalar to subtract.
         *  @returns   A new matrix containing the result.
         */
        inline friend Matrix operator-(Matrix lhs, double rhs) {
            return lhs -= rhs;
        }

        /** @brief Multiplies the current matrix by another matrix.
         *
         *  @param rhs The matrix to multiply by.
         *  @returns   A new matrix containing the result.
         *  @exception std::domain_error If the number of rows in the argument does
         *             not match the number of columns in the current matrix.
         */
        Matrix operator*(const Matrix& rhs) const;

        /** @brief Multiplies the current matrix by a scalar.
        *
        *  @param rhs The scalar to multiply by.
        *  @returns   A reference to the current matrix.
        */
        Matrix& operator*=(double rhs);

        /** @brief Mulitiplies a matrix by a scalar.
        *
        *  @param lhs The matrix to multiply.
        *  @param rhs The scalar to multiply by.
        *  @returns   A new matrix containing the result.
        */
        inline friend Matrix operator*(Matrix lhs, double rhs) {
            return lhs *= rhs;
        }

        /** @brief Mulitiplies a matrix by a scalar.
        *
        *  @param lhs The scalar to multiply by.
        *  @param rhs The matrix to multiply.
        *  @returns   A new matrix containing the result.
        */
        inline friend Matrix operator*(double lhs, Matrix rhs) {
            return rhs *= lhs;
        }

        /** @brief Divides the current matrix by a scalar.
        *
        *  @param rhs The scalar to divide by.
        *  @returns   A reference to the current matrix.
        */
        Matrix& operator/=(double rhs);

        /** @brief Divides a matrix by a scalar.
        *
        *  @param lhs The matrix to divide.
        *  @param rhs The scalar to divide by.
        *  @returns   A new matrix containing the result.
        */
        inline friend Matrix operator/(Matrix lhs, double rhs) {
            return lhs /= rhs;
        }

        /***********************************************************************/
        /* Operations                                                          */
        /***********************************************************************/
        /** @brief Calculates the adjoint of the matrix
         *
         *  @returns A matrix containing the adjoint of the current matrix.
         *  @exception std::domain_error If the matrix is not square.
         */
        Matrix adjoint() const;

        /** @brief Calculates the Cholesky decomposition of the matrix.
         *
         *  @remarks The matrix must be symetric and positive definite for the
         *  Cholesky decomposition to exist.
         */
        Matrix chol() const;

        /** @brief Calculates the i,j-th cofactor of the matrix.
         *
         *  @param m The row of the elment to calculate the cofactor for.
         *  @param n The column of the elment to calculate the cofactor for.
         *  @returns The cofactor of the given element.
         *  @exception std::domain_error If the matrix is not square.
         */
        double cofactor(std::size_t i, std::size_t j) const;

        /** @brief Calculates the cofactors of all elements in the matrix.
         *
         *  @returns A matrix containing all of the cofactors of the original matrix.
         *  @exception std::domain_error If the matrix is not square.
         */
        Matrix cofactors() const;

        /** @brief Calculates the determinant of the matrix.
         *
         *  @remarks The current implementation uses Cholesky decomposition if the
         *           Cholesky decomposition is defined, since the Cholesky
         *           decomposition is more numerically stable than Crout. If the
         *           Cholesky decomposition is not defined, then Crout decomposition
         *           is used. Finally, if the Crout decomposition is not possible,
         *           Laplace expansion is used.
         *
         *  @returns The determinant of the matrix.
         *  @exception std::domain_error If the matrix is not square.
         */
        double determinant() const;

        /** @brief Gets the diagonal valus of the matrix.
         *
         *  @returns A column vector representing the diagonal entries in the matrix.
         */
        Matrix diagonal() const;

        /** @brief Computes the inverse of a square matrix.
         *
         *  @returns A Matrix containing the inverse of the current matrix.
         *  @exception std::domain_error If the matrix is not square.
         */
        Matrix inverse() const;

        /** @brief Calculates the minor of the specified elemnt in the Matrix.
         *  @details The minor of a given elmement in a matrix is the determinant
         *           of the matrix obtained by removing the rown and column of the
         *           element from the original matrix.
         *
         *  @param m The row of the elment to calculate the minor for.
         *  @param n The column of the elment to calculate the minor for.
         *  @returns The value of the minor at the given point in the matrix.
         *  @exception std::domain_error If the matrix is not square.
         */
        double minor(std::size_t m, std::size_t n) const;

        /** @brief   Calculates the minors of all elements in the matrix.
         *  @details The minor of a given elmement in a matrix is the determinant
         *           of the matrix obtained by removing the rown and column of the
         *           element from the original matrix.
         *
         *  @returns A matrix containing all of the minors of the original matrix.
         *  @exception std::domain_error If the matrix is not square.
         */
        Matrix minors() const;

        /** @brief Generates a new matrix with the specified row and column removed.
         *
         *  @param m The row to remove.
         *  @param n The column to remove.
         *  @returns A new matrix with identical elements to the current matrix
         *           except for the removed elements.
         *
         *  @exception std::out_of_range If either argument is larger than its
         *             dimension.
         */
        Matrix submatrix(std::size_t m, std::size_t n) const;

        /** @brief Returns the transpose of the current matrix.
         */
        Matrix transpose() const;

        Matrix weightedCovariance(const Matrix& w, const double alpha=1, const double beta=0) const;

        Matrix weightedMean(const Matrix& w) const;

        /***********************************************************************/
        /* Static Operations                                                   */
        /***********************************************************************/
        /** @brief Returns the identity matrix of the given size.
         *
         *  @param m The size of the matrix.
         */
        static Matrix identity(std::size_t m);

        /***********************************************************************/
        /* Steam Insertion                                                     */
        /***********************************************************************/
        friend std::ostream& operator<<(std::ostream& os, const Matrix& obj);

        /***********************************************************************/
        /* Indexer proxies                                                     */
        /***********************************************************************/
        struct ConstRowVector {
        public:
            ConstRowVector(const double* d) : data(d) { }

            inline double operator[](std::size_t n) const {
                return data[n];
            }

        protected:
            const double* data;
        };

        struct RowVector : public ConstRowVector {
        public:
            RowVector(double* d) : ConstRowVector(d), mutData(d) { }

            inline double& operator[](std::size_t n) {
                return mutData[n];
            }

        protected:
            double* mutData;
        };

    private:
        /***********************************************************************/
        /* Internal implementation helpers                                     */
        /* !!!WARNING: These function do not do any error checking!!!          */
        /***********************************************************************/
        double laplaceDet() const;

        bool cholInternal(Matrix& r) const;
        bool croutInternal(Matrix& r, double& mult) const;

        /***********************************************************************/
        /* Class data members                                                  */
        /***********************************************************************/
        std::size_t M;
        std::size_t N;
        double* data;
    };
}

#endif
