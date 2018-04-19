/* @author    Shankar Sankararaman
 *
 * @copyright Copyright (c) 2018 United States Government as represented by
 *            the Administrator of the National Aeronautics and Space
 *            Administration. All Rights Reserved.
 */

#ifndef PCOE_GAUSSIANVARIABLE_H
#define PCOE_GAUSSIANVARIABLE_H

namespace PCOE {
    class GaussianVariable    // MD: Derive from some abstract class? Common functions would be evaluatepdf/cdf, invertcdf, setparameters, generatesample(s)
    {
    private:
        double mu, sigma; /* mean, standard deviation, variable instance, uniform random variable*/

    public:
        //double X, U;
        double VarSamples[10000];    // MD: Change to vector, don't hardcode size
        GaussianVariable();  // Constructor 1
        GaussianVariable(double, double);  // Constructor 2 - set mean and standard deviation

        double evaluatepdf(double); /* variable instance, mean, standard deviation */
        double evaluatecdf(double); /* variable instance, mean, standard deviation */
        double invertcdfur(double);  /* uniform random variable, mean, standard deviation */
        void setmeanstd(double, double); /* to store the values of mean and standard deviation*/
        void generatesamplesdirect(int); /*generate a particular number of samples using diret c++ random generator*/
        void generatesamplesicdfur(int); /*generate a particular number of samples using cdf inversion*/
    };
}

#endif // PCOE_GAUSSIANVARIABLE_H








