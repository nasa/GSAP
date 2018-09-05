// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef MATRIXTESTS_H
#define MATRIXTESTS_H
#include "Test.h"

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
    void negate_matrix();
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

    void registerTests(PCOE::Test::TestContext& context);
}

#endif
