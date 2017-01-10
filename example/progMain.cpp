/**  Start point for the Generic Architecture
 *   @ingroup   GPIC++
 *   @ingroup   Framework
 *
 *   @brief     Example starting point for the generic architecture. Specifies prognosers, communicators, starts ProgManager
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 17, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include "ProgManager.h"
#include "ThreadSafeLog.h"

const std::string MODULE_NAME = "Verification Test-Mac";

int main(int argc, const char * argv[]) {
    std::string configFile("/Users/cteubert/Documents/GSAP/GSAP_Code/cpp/Test/Verification Tests/cfg/prog_mac.cfg");
    if (argc >= 2) {
        configFile = argv[1];
    }
    
    const Log & log(Log::Instance());
    //log.setVerbosity(LOG_TRACE);
    
    ProgManager PM = ProgManager(configFile);
    
    PM.run();
    
    log.WriteLine(LOG_INFO, MODULE_NAME, "Finished verificaiton test");
    
    return 0;
}
