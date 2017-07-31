/** @file CommonCommunicator.cpp
 *
 *  @author  Jason Watkins <jason-watkins@outlook.com>
 *  @version 0.1.0
 *  @date    2016-07-28
 *
 *  @copyright Copyright (c) 2013-2016 United States Government as represented by
 *    the Administrator of the National Aeronautics and Space Administration.
 *    All Rights Reserved.
 */

#include "CommonCommunicator.h"

namespace PCOE {
    CommonCommunicator::CommonCommunicator() : subscribers(), writeItems(),
        readWaiting(false), m(), cv(), sm() {
        // Implemenation Note: The use of sm/scv here is needed to prevent
        // read/write operations being requested before the processing thread
        // is started. Otherwise, the processing thread may block when it has
        // items to process.
        unique_lock slock(sm);
        start();
        scv.wait(slock);
    }

    CommonCommunicator::~CommonCommunicator() {
        unique_lock lock(m);
        setState(ThreadState::Stopped);
        cv.notify_one();
        lock.unlock();
        join();
    }

    void CommonCommunicator::enqueue(const AllData & data) {
        lock_guard lock(m);
        writeItems.push(data);
        cv.notify_one();
    }

    void CommonCommunicator::setRead() {
        lock_guard lock(m);
        readWaiting = true;
        cv.notify_one();
    }

    void CommonCommunicator::subscribe(const Callback& fn) {
        lock_guard lock(m);
        subscribers.push_back(fn);
    }

    void CommonCommunicator::stop() {
        lock_guard lock(m);
        Thread::stop();
        cv.notify_one();
    }

    void CommonCommunicator::run() {
        unique_lock slock(sm);
        slock.unlock();
        unique_lock lock(m);
        scv.notify_one();
        while (getState() != ThreadState::Stopped) {
            cv.wait(lock);

            if (getState() == ThreadState::Stopped) {
                // Exit early to avoid long program exit times
                break;
            }
            while (!writeItems.empty() || readWaiting) {
                if (!writeItems.empty()) {
                    AllData p = writeItems.front();
                    writeItems.pop();
                    write(p);
                }
                else if (readWaiting) {
                    DataStore ds = read();
                    log.WriteLine(LOG_INFO, "CommonComm", "Returned to run()");

                    readWaiting = false;
                    log.WriteLine(LOG_INFO, "CommonComm", "Running Callbacks");

                    for (Callback& fn : subscribers) {
                        lock.unlock();
                        fn(ds);
                        lock.lock();
                    }
                    log.WriteLine(LOG_INFO, "CommonComm", "completed");

                }
            }
        }
    }
}
