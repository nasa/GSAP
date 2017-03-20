/**  ObserverTests - Header
*   @file      Unit tests for Observer classes
*   @ingroup   GPIC++
*   @ingroup   Observers
*
*   @brief     Unit tests for Observer classes
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 17, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef OBSERVERTESTS_H
#define OBSERVERTESTS_H

void observerTestsInit();

// UKF Tank tests
void testUKFTankInitialize();
void testUKFTankStep();
void testUKFTankGetInputs();

// UKF Battery tests
void testUKFBatteryFromConfig();
void testUKFBatteryInitialize();
void testUKFBatteryStep();

// PF Battery tests
void testPFBatteryFromConfig();
void testPFBatteryInitialize();
void testPFBatteryStep();

#endif // OBSERVERTESTS_H
