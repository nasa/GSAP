/**  Common Communicator - Header
 *   @class     CommonCommunicator CommonCommunicator.h
 *   @ingroup   GPIC++
 *   @defgroup  commCollection Collection of Communicators
 *
 *   @brief     CommonCommunicator Class- Abstract base class for all communicators
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 21, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_COMMONCOMMUNICATOR_H
#define PCOE_COMMONCOMMUNICATOR_H

#include <unordered_map>
#include <string>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <queue>

#include "DataStore.h"
#include "ProgData.h"
#include "ConfigMap.h"
#include "Thread.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    
    /** @brief Repository of ProgData from every prognoser. (name, data) */
    using ProgDataMap = std::unordered_map<std::string, ProgData*>;
    
    class AllData {  ///@todo(CT): Better name
    public:
        DataStore doubleDatastore;
        DataStoreString stringDataStore;
        ProgDataMap progData;
        
        AllData(const DataStore & doubleDatastoreIn,
                const DataStoreString & stringDataStoreIn,
                const ProgDataMap & progDataIn):
                    doubleDatastore(doubleDatastoreIn),
                    stringDataStore(stringDataStoreIn),
                    progData(progDataIn) {
        }
    };


    class CommonCommunicator : private Thread {
    public:
        using Callback = std::function<void(DataStore&)>;

        /** @brief      Constructor for CommonCommunicator- Initializes Log
         *  @see        CommunicatorFactory
         **/
        CommonCommunicator();

        virtual ~CommonCommunicator() override;

        void enqueue(const AllData &);

        using Thread::join;

        virtual void poll() = 0;

        void stop() final override;

        void subscribe(const Callback& fn);

    protected:

        void setRead();

        virtual DataStore read() = 0;

        virtual void write(AllData) = 0;

        using Thread::log;
    private:
        using mutex = std::mutex;
        using lock_guard = std::lock_guard<mutex>;
        using unique_lock = std::unique_lock<mutex>;

        void run() final override;

        std::vector<Callback> subscribers;
        std::queue<AllData> writeItems;
        bool readWaiting;
        mutex m;
        std::condition_variable cv;
        mutex sm;
        std::condition_variable scv;
    };
}

#endif // PCOE_COMMONCOMMUNICATOR_H
