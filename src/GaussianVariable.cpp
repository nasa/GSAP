/* @copyright Copyright (c) 2018 United States Government as represented by
*            the Administrator of the National Aeronautics and Space
*            Administration. All Rights Reserved.
*/

#include <cmath>
#include <random>
#include "GaussianVariable.h"

namespace PCOE {
    GaussianVariable::GaussianVariable(double x, double y) { //If mean and standard deviation are specified
        mu = x;
        sigma = y;
    }

    GaussianVariable::GaussianVariable() { //If mean and standard deviation are not specified
        mu = 0;
        sigma = 1;
    }

    void GaussianVariable::generatesamplesdirect(int Nsamples)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> dis(mu, sigma);


        for (int i = 0; i < Nsamples; i++)
        {

            VarSamples[i] = dis(gen);
        }

    }

    void GaussianVariable::generatesamplesicdfur(int Nsamples)
    {
        double temp;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);


        for (int i = 0; i < Nsamples; i++)
        {

            temp = dis(gen);
            VarSamples[i] = invertcdfur(temp);
        }

    }


    void GaussianVariable::setmeanstd(double x, double y)
    {
        // Allow to specify mean and standard deviation on the fly
        mu = x;
        sigma = y;
    }

    double GaussianVariable::evaluatepdf(double X) {
        double pi = 3.1415926535897;  // MD: Should define this elsewhere? We should have some #define for it...
        return 1 / (sigma*sqrt(2 * pi))*exp(-(X - mu)*(X - mu) / (2 * sigma*sigma));
    }


    double GaussianVariable::evaluatecdf(double X) {
        return 0.5*(1 + erf((X - mu) / (sigma*sqrt(2))));
    }


    double GaussianVariable::invertcdfur(double U) {
        // Based on approximation of  Milton Abramowitz, Irene A. Stegun
        double c[] = { 2.515517, 0.802853, 0.010328 };
        double d[] = { 1.432788, 0.189269, 0.001308 };
        double t;
        double intermediate;

        t = sqrt(-2 * log(U));
        intermediate = t - ((c[2] * t + c[1])*t + c[0]) / (((d[2] * t + d[1])*t + d[0])*t + 1.0);

        if (U < 0.5) {
            return (-intermediate*sigma + mu);
        }
        else {
            return (intermediate*sigma + mu);
        }
    }
}





