/**  Model Unit Tests - Header
*   @ingroup   GPIC++
*   @ingroup   support
*
*   @brief     Unit tests for Model class and derived classes.
*
*   @author    Micah Ricks
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Micah Ricks (mricks1@bulldogs.aamu.edu)
*      Created: 3/7/2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef TCPTESTS_H
#define TCPTESTS_H

// Tank model tests
void testNoDelay();
void testRecieveBufferSize();
void testSendBufferSize();
void testConnect();
void testReceiveTimeout();
void testReceive();
void testSend();
void testSendAndReceive();
void testTest();
void startSever();
void error(char *msg);

#endif // TCPTESTS_H
