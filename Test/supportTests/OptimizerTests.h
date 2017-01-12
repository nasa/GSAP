/**  OptimizerTests - Header
*   @file      Unit tests for Optimizer classes
*   @ingroup   GPIC++
*   @ingroup   Optimizers
*
*   @brief     Unit tests for Optimizer classes
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: August 16, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef OptimizerTests_H
#define OptimizerTests_H

#include "Battery.h"

void optimizerTestsInit();

double evaluateBattery(std::vector<std::vector<double>> & data, std::vector<double> x);
void testBatteryParamEst();

double evaluateBatteryEOL(std::vector<double> x, std::vector<double> & qMobiles, std::vector<double> & Ros, std::vector<double> & tDiffusions, std::vector<double> & currents, std::vector<double> & dts);
void testBatteryAgingRateParamEst();

#endif // OptimizerTests_H
