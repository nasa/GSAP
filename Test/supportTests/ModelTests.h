/**  Model Unit Tests - Header
*   @ingroup   GPIC++
*   @ingroup   support
*
*   @brief     Unit tests for Model class and derived classes.
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: 3/7/2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef MODELTESTS_H
#define MODELTESTS_H

// Tank model tests
void testTankInitialize();
void testTankStateEqn();
void testTankOutputEqn();

// Battery model tests
void testBatterySetParameters();
void testBatteryInitialization();
void testBatteryStateEqn();
void testBatteryOutputEqn();
void testBatteryThresholdEqn();
void testBatteryInputEqn();
void testBatteryPredictedOutputEqn();

#endif // MODELTESTS_H
