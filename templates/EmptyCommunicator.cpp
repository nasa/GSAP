/**  Empty Communicator - Body
 *   @class     EmptyCommunicator EmptyCommunicator.h
 *   @ingroup   GPIC++
 *   @ingroup   commCollection
 *
 *   @brief     Communicator Template
 *
 *   @see        CommonCommunicator
 *
 *   @author    Chris Teubert
 *   @version   1.0.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 25, 2016
 *
 *   @copyright Copyright (c) 2013-2017 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <string>

#include "EmptyCommunicator.h"

namespace PCOE {
    const std::string MODULE_NAME = "EMPTYCOMM"; // Replace with your module name for log

    EmptyCommunicator::EmptyCommunicator(const ConfigMap & configMap) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Configuring");
        ///------------------------------------
        /// HERE IS WHERE YOU CONFIGURE THE Communicator.
        /// Read the configuration map
        ///     Example: std::string SomeParam = configMap.at("SomeParam")[0];
        ///     Example2: std::vector<std::string> paramList = configMap.at("otherParam");
        ///
        /// ADD COMMUNICATOR CONFIGURATION CODE BELOW:
        ///------------------------------------
        (void)configMap; // Supress unused variable warning. Remove when implementing your communicator

        ///------------------------------------
    }

    inline void EmptyCommunicator::poll() {
        // setRead(); // <-Call this if there is data to be read
          // Not calling setRead() in this function will mean that the read()
          // function will not be called
    }

    void EmptyCommunicator::write(AllData data) {
        ///------------------------------------
        /// HERE IS WHERE YOU SEND DATA
        ///     Send any of the data in the DataStore ds or progData pData
        ///
        ///     The DataStore ds contains the latest received sensor data.
        ///     Access it by key, for example:
        ///         double value = ds["SomeKey"];
        ///         ms_rep timeValeWasSet = ds["SomeKey"].getTime();
        ///     The DataStoreString dsString contains the latest received sensor strings
        ///     Access it by key, for example:
        ///         std::string value = ds["SomeKey"];
        ///         ms_rep timeValeWasSet = ds["SomeKey"].getTime();
        ///
        ///     The ProgDataMap pData contains the results of all prognosers.
        ///     Access it by prognoser name, for example:
        ///         ProgData & pdBatt = pData["Battery1"];

        DataStore & ds              = data.doubleDatastore;
        DataStoreString & dsString  = data.stringDataStore;
        ProgDataMap & pData         = data.progData;

        /// ADD COMMUNICATOR PUBLISHER CODE BELOW:
        ///------------------------------------

        ///------------------------------------
        (void) ds; // Supress unused variable warning. Remove when implementing your communicator
        (void) dsString;
        (void) pData;
    }

    DataStore EmptyCommunicator::read() {
        ///------------------------------------
        /// HERE IS WHERE YOU RECEIVE DATA
        ///     Receive data and fill in the DataStore 'ds'
        ///     Example: ds["someParam"] = 1.0;
        ///
        /// ADD COMMUNICATOR SUBSCRIBER CODE BELOW:
        ///------------------------------------
        DataStore ds;

        ///------------------------------------
        return ds;
    }
}
