//
//  progMain.cpp
//  Example
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 5/4/16.
//  Copyright © 2018 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//

#include "ConfigMap.h"
#include "ThreadSafeLog.h"

using namespace PCOE;

int main() {
    // Specify configuration file directories
    ConfigMap::addSearchPath("../example/cfg/");
    //Log::SetVerbosity(LOG_VERBOSITY::LOG_TRACE); // Change Verbosity


//    ProgManager PM = ProgManager("prog.cfg");
//    PM.run();
    return 0;
}
