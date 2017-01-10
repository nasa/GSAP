/**  PredictorTests - Header
*   @file      Unit tests for Predictor classes
*   @ingroup   GPIC++
*   @ingroup	Predictors
*
*   @brief     Unit tests for Predictor classes
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 25, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef PREDICTORTESTS_H
#define PREDICTORTESTS_H

void predictorTestInit();

// MC Battery tests
void testMonteCarloBatteryPredict();
void testMonteCarloBatteryConfig();

#endif // PREDICTORTESTS_H
