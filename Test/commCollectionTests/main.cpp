/**  frameworkTests - Entry point
*   @file      Run unit tests for framework classes
*
*   @brief     Run unit tests for framework classes
*
*   @author    Jason Watkins <jason-watkins@outlook.com>
*   @version   0.1.0
*   @date      2016-07-08
*
*   @pre       N/A
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*              the Administrator of the National Aeronautics and Space
*              Administration. All Rights Reserved.
*/

#include <fstream>

#include "CommTests.h"
#include "Test.h"

using namespace PCOE::Test;

int main() {
    TestContext context;
    context.AddTest("RandomComm", RandomCommTest);
    context.AddTest("PlaybackComm", PlaybackCommunicatorTest);
    context.AddTest("RecorderComm", RecorderCommunicatorTest);

    int result = context.Execute();
    std::ofstream junit("testresults/commCollection.xml");
    context.WriteJUnit(junit);
    return result;
}