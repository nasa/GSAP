/**  @file      ProgManager.cpp     Prognostic Manager
 *   @class     ProgManager         Prongostic Manager
 *   @ingroup   GPIC++
 *   @ingroup   Framework
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

#ifndef PCOE_PROGMANAGER_H
#define PCOE_PROGMANAGER_H

#include <string>
#include <vector>

#include "CommonPrognoser.h" // Note: This is needed for UniquePointer
#include "GSAPConfigMap.h"
 
namespace PCOE {
    enum COMMAND {
        NONE,
        START,
        STOP,
        RESUME,
        PAUSE
    };

    /** @brief      Structure for holding received commands */
    struct Cmd {
        Cmd();            /**< Initialize command to none */
        COMMAND command;  /**< Set to value from Enum COMMAND- Defined in ThreadControl */
        std::string body; /**< Body of the message (to be used in future iterations) (ex: Start thisPrognoser) */
    };

    class Log;
    class CommManager;

    /**
     *  @class      ProgManager
     *  @brief      Main class for C++ Generic Prognostic Infrastructure
     *    This class creates the ProgMonitors and Communication Manager.
     **/
    class ProgManager {
    public:
        /** @brief Default constructor. Constructs a new ProgManager with no
         *         associated configuration. Configuration must be set by
         *         calling @see setConfig before running the progManager.
         */
        ProgManager();
        
        /** @brief Constructs a new ProgManager configured by opening the
         *         configuration file at the given path.
         *
         *  @param path The path to a configuration file.
         */
        ProgManager(const std::string& path);

        /** @brief Constructs a new ProgManager with the given configuration.
         *
         *  @param config The configuration used by the progManager
         */
        ProgManager(const GSAPConfigMap& config);

        /** @brief Sets the configuration of the current progManager by
         *         opening the configuration file at the given path.
         *
         *  @param path The path to a configuration file.
         */
        void setConfig(const std::string & path);
        
        /** @brief Sets the configuration of the current progManager.
         *
         *  @param config The configuration used by the progManager
         */
        void setConfig(const GSAPConfigMap& config);
        
        /** @brief Add a new prognoser
         *
         *  @param  path  The path to a configuration file
         **/
        void addPrognoser(const std::string & path);

        /** @brief starts the progManager. */
        void run();
        
        void enable();
        
        void start();
        
        void pause();
        
        inline void resume() {start();}
        
        void stop();

    private:
        GSAPConfigMap configValues;
        bool configSet;

        /// @function   control
        /// @brief      Function to receive control commands from terminal
        /// @return     Received Command
        Cmd control();
        
        std::vector<std::unique_ptr<CommonPrognoser> > prognosers;
        
        CommManager &theComm;
    };
}

#endif // PCOE_PROGMANAGER_H
