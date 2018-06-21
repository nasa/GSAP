/**  frameworkTests - Entry point
*   @file      Run unit tests for framework classes
*
*   @brief     Run unit tests for framework classes
*
*   @author    Jason Watkins <jason-watkins@outlook.com>
*   @version   1.1.0
*   @date      2016-07-08
*
*   @pre       N/A
*
*   @copyright Copyright (c) 2018 United States Government as represented by
*              the Administrator of the National Aeronautics and Space
*              Administration. All Rights Reserved.
*/

#include <fstream>

#include "CommunicatorTests.h"
#include "FrameworkTests.h"
#include "ProgManagerTests.h"
#include "Test.h"

using namespace PCOE::Test;

int main() {
    TestContext context;
    context.AddTest("Prognoser Factory", PrognoserFactoryTest);
    context.AddTest("CommManagerTest", PCOE::CommManagerTest);

    // ProgManager
    context.AddTest("construct_default", TestProgManager::construct_default, "ProgManager");
    context.AddTest("construct_path", TestProgManager::construct_path, "ProgManager");
    context.AddTest("construct_config", TestProgManager::construct_config, "ProgManager");
    context.AddTest("setConfig_path", TestProgManager::setConfig_path, "ProgManager");
    context.AddTest("setConfig_config", TestProgManager::setConfig_config, "ProgManager");

    context.AddTest("construct", TestCommunicator::construct, "Common Communicator");
    context.AddTest("enqueue", TestCommunicator::enqueue, "Common Communicator");
    context.AddTest("subscribe", TestCommunicator::subscribe, "Common Communicator");
    context.AddTest("stop", TestCommunicator::stop, "Common Communicator");

    int result = context.Execute();
    std::ofstream junit("testresults/framework.xml");
    context.WriteJUnit(junit);
    return result;
}
