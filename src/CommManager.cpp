/**  Communication Manager - Body
 *   @class     CommManager CommManager.h
 *   @ingroup   GPIC++
 *   @ingroup   Framework
 *
 *   @brief     Communication Manager Singleton Class
 *   Directs sending and getting of data. Handes set/get requests from Prognosers.
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *   @bug       Sometimes experiences race condition progDataStruct, etc.
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <sstream>
#include <string>
#include <thread>

#include "CommManager.h"
#include "CommunicatorFactory.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    /// Consts
    const unsigned long DEFAULT_STEP_SIZE = 50;

    /// Parameter Map Keys
    const std::string STEP_SIZE_KEY = "commmanger.step_size";
    const std::string COMM_KEY = "Communicators";

    CommManager::CommManager() : Thread(), threadStarted(false), stepSize(DEFAULT_STEP_SIZE) {
        moduleName = "CommManager";
        log.WriteLine(LOG_INFO, moduleName, "Enabling");
    }

    void CommManager::configure(const GSAPConfigMap& params) {
        CommunicatorFactory& Factory = CommunicatorFactory::instance();
        for (auto& it : params.at(COMM_KEY)) {
            GSAPConfigMap config(it);
            auto& name = config.at("type")[0];
            comms.push_back(Factory.Create(name, config));
            comms.back()->subscribe([this](DataStore& ds) { this->updateLookup(ds); });
        }

        if (params.includes(STEP_SIZE_KEY)) {
            stepSize = std::stoul(params.at(STEP_SIZE_KEY)[0]);
        }

        enable();
    }

    void CommManager::run() {
        static std::chrono::high_resolution_clock::time_point nextTime;
        while (getState() != ThreadState::Stopped) {
            nextTime =
                std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(stepSize);
            log.WriteLine(LOG_TRACE, moduleName, "Updating Lookup Table");

            for (auto& it : comms) {
                // Poll each communicator. For some communicators, this
                // triggers a read, for others it is a no-op.
                it->poll();
            }

            if (getState() == ThreadState::Stopped) {
                break;
            }

            {
                std::lock(lookupMutex, progDataMutex);
                lock_guard lookuplock(lookupMutex, std::adopt_lock);
                lock_guard proglock(progDataMutex, std::adopt_lock);

                AllData data(lookup, stringLookup);
                for (auto& it : comms) {
                    it->enqueue(data);
                }
            }

            // Second check so it will stop quicker (publisher may take some time)
            if (getState() == ThreadState::Stopped) {
                break;
            }
            std::this_thread::sleep_until(nextTime);
        }
    }

    void CommManager::registerKey(const std::string& tagName) {
        lock_guard lock(lookupMutex);
        std::stringstream a;

        if (lookup.find(tagName) == lookup.end()) {
            // TagName doesn't exist
            // if New Tagname - create
            log.FormatLine(LOG_DEBUG, moduleName, "Registering new tag: %s", tagName.c_str());
            lookup[tagName] = Datum<double>();
        }
        else {
            log.FormatLine(LOG_DEBUG,
                           moduleName,
                           "Tag already registered, skipping: %s",
                           tagName.c_str());
        }
    }

    Datum<double> CommManager::getValue(const std::string& tagName) const {
        lock_guard lock(lookupMutex);
        log.FormatLine(LOG_DEBUG, moduleName, "Requesting value for %s", tagName.c_str());
        auto it = lookup.find(tagName);
        if (it != lookup.end()) {
            // tagName Exists
            return it->second;
        }

        log.FormatLine(LOG_WARN, moduleName, "Requested tag '%s' does not exist", tagName.c_str());
        throw std::out_of_range("Requested tag does not exist");
    }

    Datum<std::string> CommManager::getString(const std::string& tagName) const {
        lock_guard lock(lookupMutex);
        log.FormatLine(LOG_DEBUG, moduleName, "Requesting value for %s", tagName.c_str());
        auto it = stringLookup.find(tagName);
        if (it != stringLookup.end()) {
            // tagName Exists
            return it->second;
        }

        log.FormatLine(LOG_WARN, moduleName, "Requested tag '%s' does not exist", tagName.c_str());
        throw std::out_of_range("Requested tag does not exist");
    }

    void CommManager::updateLookup(DataStore& ds) {
        lock_guard lock(lookupMutex);
        for (auto it : ds) {
            lookup[it.first] = it.second;
        }
    }
}
