/**  Communication Manager - Header
 *   @class     CommManager CommManager.h
 *   @ingroup   GPIC++
 *   @ingroup   Framework
 *
 *   @brief     Communication Manager Singleton Class
 *   Directs sending and getting of data. Handes set/get requests from Prognosers.
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *   @bug       Sometimes experiences race condition progDataStruct, etc.
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_COMMMANAGER_H
#define PCOE_COMMMANAGER_H

#include <string>
#include <vector>
#include <mutex>

#include "Thread.h"  // For start, stop, pause, ...
#include "CommonCommunicator.h"
#include "GSAPConfigMap.h"
#include "Singleton.h"
#include "Datum.h"

namespace PCOE {
    class Log;
    class ProgData;
    class DataStore;

    class CommManager : public Thread, public Singleton<CommManager> {
        friend void CommManagerTest();
        friend class Singleton<CommManager>;  // Needed for singleton
    public:
        void configure(const GSAPConfigMap & params);

        /** @brief      Register a key with the Comm Manager

         *
         *  This method allows users of the CommManager to register keys they expect
         *  to use. If the key has not yet been registered by another user the key
         *  will be added to the datamap.
         */
        void registerKey(const std::string & key);

        bool registerProgData(const std::string & key, ProgData * pData);

        /** @brief      Get the value associated with a key
         *  @param[in]  key Key for which the value is requested
         *
         *  @return     Pointer to datum containing the result of the query
         *              Will be null if key does not exist
         */
        Datum<double> getValue(const std::string & key) const;
        
        Datum<std::string> getString(const std::string & key) const;

        /**  @brief     Main Communications Thread
         *
         *   Directs the main communications loop- runs until comm->stop()
         */
        void run();

    private:
        using mutex = std::recursive_mutex;
        using lock_guard = std::lock_guard<mutex>;

        /// Communications Manager Constructor
        CommManager();

        void updateLookup(DataStore & ds);

        ProgDataMap progData;

        DataStore lookup;
        DataStoreString stringLookup;
    
        bool threadStarted;

        std::vector<std::unique_ptr<CommonCommunicator> > comms;
        unsigned long stepSize;
        mutable mutex progDataMutex;
        mutable mutex lookupMutex;
    };
}

#endif // PCOE_COMMMANAGER_H
