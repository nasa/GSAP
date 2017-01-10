/* @copyright Copyright (c) 2016 United States Government as represented by
*            the Administrator of the National Aeronautics and Space
*            Administration. All Rights Reserved.
*/

#include <cmath>
#include "StatisticalTools.h"

namespace PCOE {
    double calculatemean(double X[], int N)
    {
        double sum = 0;
        double mu = 0;
        for (int i = 0; i < N; i++) {
            sum = sum + X[i];
        }

        mu = sum / N;
        return mu;
    }


    double calculatestdv(double X[], int N)
    {
        double mu, sum, var, std;
        sum = 0;
        mu = calculatemean(X, N);

        for (int i = 0; i < N; i++)
        {
            sum = sum + (X[i] - mu)*(X[i] - mu);
        }
        var = sum / N;
        std = sqrt(var);

        return std;
    }



    double calculatecdf(double X[], int N, double Xcritical) //Evaluate Prob(X<Xcritical)
    {
        double sum = 0;

        for (int i = 0; i < N; i++)
        {
            if (X[i] < Xcritical)
            {
                sum = sum + 1;
            }
        }
        return sum / N;
    }
}
