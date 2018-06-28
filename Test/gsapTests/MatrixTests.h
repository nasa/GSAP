/**  MatrixTests - Header
 *   @file      Unit tests for Matrix class
 *   @ingroup   GPIC++
 *
 *   @brief     Unit tests for Matrix classes
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: April 7, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef MATRIXTESTS_H
#define MATRIXTESTS_H

namespace TestMatrix {
    // Matrix creation
    void construct_default();
    void construct_size();
    void construct_initialvalue();
    void construct_initializerlist();
    void construct_vector();
    void construct_copy();
    void construct_move();
    void operator_assign();

    // Comparison operators
    void operator_equal();
    void operator_notequal();
    void issquare();

    // Basic operations
    void rows();
    void cols();
    void indexer();
    void indexer_const();
    void at();
    void const_at();
    void col_get();
    void col_setmatrix();
    void col_setvector();
    void row_get();
    void row_setmatrix();
    void row_setvector();
    void operator_vector();
    void resize();

    // Arithmetic operations
    void add_matrix();
    void add_scalar();
    void subtract_matrix();
    void subtract_salar();
    void multiply_matrix();
    void multiply_scalar();
    void divide_scalar();

    // Complex operations
    void adjoint();
    void cofactors();
    void determinant();
    void laplaceDet();
    void diagonal();
    void inverse();
    void minors();
    void submatrix();
    void transpose();
    void identity();

    // Special operations
    void cholesky();
    void weightedmean();
    void weightedcovariance();
    // Stream insertion
    void streamInsertionOperator();
}

#endif // MATRIXTESTS_H
