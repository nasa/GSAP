/* @copyright Copyright (c) 2018 United States Government as represented by
 *            the Administrator of the National Aeronautics and Space
 *            Administration. All Rights Reserved.
 */

#ifndef PCOE_STATISTICALTOOLS_H
#define PCOE_STATISTICALTOOLS_H

namespace PCOE {
    double calculatemean(double X[], int N);
    double calculatestdv(double X[], int N);
    double calculatecdf(double X[], int N, double Xcritical);
}

#endif // PCOE_STATISTICALTOOLS_H
