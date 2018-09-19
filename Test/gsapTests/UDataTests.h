/**  Unit Test functions for UData- Header
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
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#ifndef UDATATESTS_H
#define UDATATESTS_H

namespace TestUData {
    void construct_default();
    void construct_type();
    void construct_scalar();
    void construct_copy();
    void construct_move();
    void operator_assign();
    void operator_equal();
    void operator_notequal();
    void operator_double();
    void dist();
    void npoints();
    void size();
    void uncertainty();
    void updated();
    void valid();
    void operator_not();
    void iterators_forward();
    void iterators_reverse();
    void access_double();
    void access_pair();
    void access_vector();
    void point();
    void meanSD();
    void meanCovar();
    void percentiles();
    void samples();
    void wSamples();
}

#endif // UDATATESTS_H
