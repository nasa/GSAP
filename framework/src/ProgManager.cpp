/**  @file      ProgManager.cpp     Prognostic Manager
*   @class     ProgManager         Prongostic Manager
*   @defgroup  GPIC++    Generic Prognostics Infrastructure-C++
*   @defgroup  Framework Prognostic Framework
*
*   @brief     Main class for C++ Generic Prognostic Infrastructure
*    This class creates the ProgMonitors and Communication Manager.
*
*   @author    Chris Teubert
*   @version   1.1.0
*
*   @pre       Prognostic Configuration File and Prognoster Configuration Files
*
*   @bug       Large delay for ending other threads
*
*      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
*      Created: November 11, 2015
*
*   @copyright Copyright (c) 2013-2018 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include <exception>
#include <iostream>
#include <vector>
#include <algorithm>  // For tolower
#include <string>

#include "SharedLib.h"  // for trim
#include "ProgManager.h"
#include "PrognoserFactory.h"
#include "CommManager.h"

namespace PCOE {
    /// CONFIGURABLE PARAMETERS
    const std::string PACKAGE_NAME = "C++ Generic Prognostic Infrastructure";
    const std::string VERSION = "1.1.0";
    const std::string NOTE = "If you have technical issues with the plugin, "
        "please report them by \nemailing Christopher Teubert (christopher.a.teubert@nasa.gov).";
    const std::string MODULE_NAME = "PrognosticManager";

    Cmd::Cmd() : command(NONE) {}

    class CommonCommunicator;

    static Log &logger = Log::Instance();

    ProgManager::ProgManager() : configValues(), configSet(false), theComm(CommManager::instance()) { }

    ProgManager::ProgManager(const std::string& path) :
        ProgManager(GSAPConfigMap(path)) { }

    ProgManager::ProgManager(const GSAPConfigMap& config)
        : configValues(config), configSet(true), theComm(CommManager::instance()) { }

    void ProgManager::setConfig(const std::string& path) {
        setConfig(GSAPConfigMap(path));
    }

    void ProgManager::setConfig(const GSAPConfigMap& config) {
        configValues = config;
        configSet = true;
    }

    void ProgManager::run() {
        enable();

        /// Setup Main Loop
        unsigned int counter = 0;
        Cmd ctrl;

        /// Main Loop- Handle controls for prognosers
        while (ctrl.command != STOP) {
            counter++;

            /// Handle Commands
            ctrl = control();

            if (ctrl.command == STOP) {
                stop();
            }
            else if (ctrl.command == START || ctrl.command == RESUME) {
                start();
            }
            else if (ctrl.command == PAUSE) {
                pause();
            }
        }  // End while (command != stop)w
    }
    
    void ProgManager::addPrognoser(const std::string & path) {
        PrognoserFactory &factory = PrognoserFactory::instance();
        prognosers.push_back(factory.Create(path));
    }
    
    void ProgManager::enable() {
        /// Setup Log
        logger.Initialize(PACKAGE_NAME, VERSION, NOTE);
        logger.WriteLine(LOG_INFO, MODULE_NAME, "Enabling");
        
        if (!configSet) {
            logger.WriteLine(LOG_DEBUG, MODULE_NAME, "No configuration file set - closing progManager");
            return;
        }
        
        /// SETUP PROGNOSERS
        logger.WriteLine(LOG_DEBUG, MODULE_NAME, "Setting Up Prognosers");
        if (configValues.includes("Prognosers")) {
            for (auto & itStrs : configValues.at("Prognosers")) {
                addPrognoser(itStrs);
                // @todo(CT): Add check that component was made correctly
            }
        }
        
        /// Setup COMMUNICATION
        // Note: This must be done after the prognosers
        theComm.configure(configValues);
        theComm.start();
        
        logger.WriteLine(LOG_DEBUG, MODULE_NAME, "Enabled");
    }
    
    void ProgManager::start() {
        logger.WriteLine(LOG_INFO, MODULE_NAME, "Starting");
        /// START PROGNOSERS
        for (auto & prognoser : prognosers) {
            prognoser->start();
        }
        
        logger.WriteLine(LOG_DEBUG, MODULE_NAME, "Started");
    }
    
    void ProgManager::pause() {
        logger.WriteLine(LOG_INFO, MODULE_NAME, "Pausing");
        /// PAUSE PROGNOSERS
        for (auto & prognoser : prognosers) {
            prognoser->pause();
        }
        
        logger.WriteLine(LOG_DEBUG, MODULE_NAME, "Paused");
    }
    
    void ProgManager::stop() {
        logger.WriteLine(LOG_INFO, MODULE_NAME, "Stopping");
        /// STOP PROGNOSERS
        for (auto & prognoser : prognosers) {
            prognoser->stop();
        }
        
        logger.WriteLine(LOG_DEBUG, MODULE_NAME, "Cleanup");
        
        /// CLEANUP ACTIVITIES
        // End each Prognoser
        for (auto & prognoser : prognosers) {
            logger.WriteLine(LOG_DEBUG, MODULE_NAME, "Waiting for Prognoser thread to stop");
            prognoser->join();// Wait for thread to end
        }
        
        // Stop Communication Manager
        // NOTE: This has to be done after the other threads that used it are stopped
        theComm.stop();
        logger.WriteLine(LOG_DEBUG, MODULE_NAME, "Waiting for Comm thread to stop");
        theComm.join();
        
        // Stop Log, exit thread
        logger.WriteLine(LOG_INFO, MODULE_NAME, "Stopped");
        logger.Close();
    }

    Cmd ProgManager::control() {
        logger.WriteLine(LOG_TRACE, MODULE_NAME, "Waiting for Control Command");

        std::string input;
        Cmd c;

        std::cout << "prg $ ";
        std::cin >> input;  // Receive input
        logger.FormatLine(LOG_TRACE, MODULE_NAME, "Control Command received- %s", input.c_str());
        trim(input);

        if (input.length() == 0) {
            c.command = NONE;
            return c;
        }

        const auto marker = input.find_first_of(" \t");
        std::string command = (input.substr(0, marker));
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

        // Fill out Command Structure
        if (command.compare("start") == 0) {
            c.command = START;
            logger.WriteLine(LOG_TRACE, MODULE_NAME, "Start command received");
        }
        else if (command.compare("pause") == 0) {
            c.command = PAUSE;
            logger.WriteLine(LOG_TRACE, MODULE_NAME, "Pause command received");
        }
        else if (command.compare("resume") == 0) {
            c.command = RESUME;
            logger.WriteLine(LOG_TRACE, MODULE_NAME, "Resume command received");
        }
        else if (command.compare("stop") == 0) {
            c.command = STOP;
            logger.WriteLine(LOG_TRACE, MODULE_NAME, "Stop command received");
        }
        else {
            c.command = NONE;
            logger.FormatLine(LOG_WARN, MODULE_NAME, "Command not recognized: %s", command.c_str());
        }

        return c;
    }
}
