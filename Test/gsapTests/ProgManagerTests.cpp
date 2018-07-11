/** @file
 *  @brief A set of functions to test the basic functionality of ProgManager
 *
 *  @author    Jason Watkins <jason-watkins@outlook.com>
 *  @version   0.2.0
 *  @date      2016-08-08
 *
 *  @copyright Copyright (c) 2018 United States Government as represented by
 *             the Administrator of the National Aeronautics and Space
 *             Administration. All Rights Reserved.
 **/

#include "ProgManager.h"

#include "Test.h"
#include "ProgManagerTests.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace TestProgManager {
    void construct_default() {
        ProgManager pm;
    }

    void construct_path() {
        std::string path = "../example/cfg/prog.cfg";
        ProgManager pm(path);
    }

    void construct_config() {
        std::string path = "../example/cfg/prog.cfg";
        GSAPConfigMap config(path);
        ProgManager pm(config);
    }

    void setConfig_path() {
        std::string path = "../example/cfg/prog.cfg";
        ProgManager pm;
        pm.setConfig(path);
    }

    void setConfig_config() {
        std::string path = "../example/cfg/prog.cfg";
        GSAPConfigMap config(path);
        ProgManager pm;
        pm.setConfig(config);
    }
}
