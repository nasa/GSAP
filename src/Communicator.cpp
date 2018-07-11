/** @file Communicator.cpp
 *
 *  @author  Jason Watkins <jason-watkins@outlook.com>
 *  @version 0.1.0
 *  @date    2016-07-28
 *
 *  @copyright Copyright (c) 2013-2018 United States Government as represented by
 *    the Administrator of the National Aeronautics and Space Administration.
 *    All Rights Reserved.
 */

#include "Communicator.h"

namespace PCOE {
    Communicator::Communicator()
        : subscribers(), writeItems(), readWaiting(false), m(), cv(), sm() {
        // Implemenation Note: The use of sm/scv here is needed to prevent
        // read/write operations being requested before the processing thread
        // is started. Otherwise, the processing thread may block when it has
        // items to process.
        unique_lock slock(sm);
        start();
        scv.wait(slock);
    }

    Communicator::~Communicator() {
        unique_lock lock(m);
        setState(ThreadState::Stopped);
        cv.notify_one();
        lock.unlock();
        join();
    }

    void Communicator::enqueue(const AllData& data) {
        lock_guard lock(m);
        writeItems.push(data);
        cv.notify_one();
    }

    void Communicator::setRead() {
        lock_guard lock(m);
        readWaiting = true;
        cv.notify_one();
    }

    void Communicator::subscribe(const Callback& fn) {
        lock_guard lock(m);
        subscribers.push_back(fn);
    }

    void Communicator::stop() {
        lock_guard lock(m);
        Thread::stop();
        cv.notify_one();
    }

    void Communicator::run() {
        unique_lock slock(sm);
        slock.unlock();
        unique_lock lock(m);
        scv.notify_one();
        while (getState() != ThreadState::Stopped) {
            cv.wait(lock);
            log.WriteLine(LOG_TRACE, "COMM", "Starting run");

            if (getState() == ThreadState::Stopped) {
                // Exit early to avoid long program exit times
                break;
            }
            while (!writeItems.empty() || readWaiting) {
                if (!writeItems.empty()) {
                    log.WriteLine(LOG_TRACE, "COMM", "Writing data");
                    AllData p = writeItems.front();
                    writeItems.pop();
                    write(p);
                }
                else if (readWaiting) {
                    log.WriteLine(LOG_TRACE, "COMM", "Reading data");
                    DataStore ds = read();
                    log.WriteLine(LOG_TRACE, "CommonComm", "Read complete. Calling callbacks");

                    readWaiting = false;
                    for (Callback& fn : subscribers) {
                        lock.unlock();
                        fn(ds);
                        lock.lock();
                    }
                    log.WriteLine(LOG_TRACE, "CommonComm", "Callbacks completed");
                }
            }
        }
        log.WriteLine(LOG_TRACE, "COMM", "Exiting run");
    }
}
