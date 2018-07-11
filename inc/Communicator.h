/**  Common Communicator - Header
 *   @class     Communicator Communicator.h
 *   @ingroup   GPIC++
 *   @defgroup  commCollection Collection of Communicators
 *
 *   @brief     Communicator Class- Abstract base class for all communicators
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 21, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_Communicator_H
#define PCOE_Communicator_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>

#include "ConfigMap.h"
#include "DataStore.h"
#include "ProgData.h"
#include "Thread.h"
#include "ThreadSafeLog.h"

namespace PCOE {

    /** @brief Repository of ProgData from every prognoser. (name, data) */
    using ProgDataMap = std::unordered_map<std::string, ProgData*>;

    class AllData { ///@todo(CT): Better name
    public:
        DataStore doubleDatastore;
        DataStoreString stringDataStore;
        ProgDataMap progData;

        AllData(const DataStore& doubleDatastoreIn,
                const DataStoreString& stringDataStoreIn,
                const ProgDataMap& progDataIn)
            : doubleDatastore(doubleDatastoreIn),
              stringDataStore(stringDataStoreIn),
              progData(progDataIn) {}
    };

    class Communicator : private Thread {
    public:
        using Callback = std::function<void(DataStore&)>;

        /** @brief      Constructor for Communicator- Initializes Log
         *  @see        CommunicatorFactory
         **/
        Communicator();

        virtual ~Communicator() override;

        void enqueue(const AllData&);

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
        using mutex       = std::mutex;
        using lock_guard  = std::lock_guard<mutex>;
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

#endif // PCOE_Communicator_H
