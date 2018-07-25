/**  Common Prognoser - Header
 *   @class     Prognoser Prognoser.h
 *   @ingroup   GPIC++
 *   @ingroup   ProgLib
 *
 *   @brief     Common Prognoser Class- Inherited by Component Prognosers
 *
 *   The purpose of this class is to:
 *      1. handle all the common operations (getting/sending data, saving results,
 *         handling prognoser history file, etc.) and,
 *      2. setup the structure of the component prognoser with virtual functions.
 *      3. contain the main prognostics loop
 *   As a design choice- we try to offload as much of the process to the common
 *   prognoser as possible to simplify integrating additional prognosers.
 *
 *   @note      THIS SOFTWARE IS DESIGNED TO WORK WITH OS X 10.11, and Windows V #####
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_Prognoser_H
#define PCOE_Prognoser_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "DataStore.h"
#include "Thread.h" // For Start, Stop, pause, ... etc.

namespace PCOE {
    class GSAPConfigMap;

    // This class is needed to bind getValue. Bind requires const copy constructor, which
    // cannot be provided for CommManager (is deleted).
    // The wrapper has a copy constructor to allow this. The getValue method calls
    // getvalue for the wrapped CommManager

    class Prognoser : public Thread {
    public:
        /** @brief      Common Prognoser Constructor
         *  @param      config Map of configuration parameters in the prognoser configuration
         *              file
         *
         *  The common prognoser constructor is called from the component prognoser
         *  constructor. It is passed in any configuration parameters in the prognoser
         *  configuration file and does the following:
         *      1. configures those parameters
         *      2. initializes the communications manager and logger, which both the
         *         common and component prognosers use.
         */
        Prognoser(GSAPConfigMap& config);

        /**  @brief       Main Prognostics Thread
         *
         *   Directs the main prognostics loop- runs until monitor->stop()
         */
        void run() override;

        /// Save the current state to the prognostic history file
        void saveState() const;

        /// Load the last state from the prognostic history file
        void loadHistory();

        /// Reset the prognostic history file (incase of maintanance, etc.)
        void resetHistory() const;

        //*-------------------------------------------------------*
        //|     Virtual methods - Used by Component Prognoser     |
        //*-------------------------------------------------------*

        /** @brief     check the validity of any input (sensor) data.
         *
         *             This could be as simple as bound checks or a complicated
         *             analysis. By default this is not done- making this step
         *             optional in the component prognoser implementation
         */
        virtual void checkInputValidity() {}

        /** @brief     check if there is enough new data to preform prognosis
         *  @return    if there is enough data
         *
         *             Check if the data exists and is new enough to be used for
         *             prognosis. If false is returned prognostics steps will be
         *             skipped. By default this returns true- making this step
         *             optional in the component prognoser implementation
         */
        virtual bool isEnoughData() const {
            return true;
        }

        /** @brief     Prognostic Step
         *
         *             Preform model updates. This is done every step where there is
         *             enough data. This is a required method in any component
         *             prognoser
         */
        virtual void step() = 0;

        /** @brief     check the validity of any prognostics results.
         *
         *             This could be as simple as bound checks or a complicated
         *             analysis. By default this a simple bounds test on timeToEvent
         *             - making this step optional in the component prognoser
         *             implementation
         *             Default implemented in Prognoser
         */
        virtual void checkResultValidity();

    protected:
        Datum<double> getValue(const std::string& key);

    private:
        std::string histFileName; ///< Name of history file
        std::vector<std::string> histStr; ///< Current contents of history file

        unsigned int loopInterval; ///< Time between prognostic loops (ms)
        unsigned int saveInterval; ///< Loops between saves
        bool saveEnabled;

        std::map<std::string, std::function<Datum<double>(void)>> lookup;
    };
}

#endif // PCOE_Prognoser_H
