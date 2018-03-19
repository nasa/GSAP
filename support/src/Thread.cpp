// Author: Jason Watkins <jason-watkins@outlook.com>
// Author: Chris Teubert <christopher.a.teubert@nasa.gov>
// Date:   2016-06-21
//
//  Copyright (c) 2016 United States Government as represented by
//  the Administrator of the National Aeronautics and Space Administration.
//  All Rights Reserved.

#include "Thread.h"

namespace PCOE {
    Thread::Thread()
        : log(Log::Instance()), moduleName(""), m(), state(ThreadState::Created),
          thread() {
        log.WriteLine(LOG_DEBUG, moduleName, "Creating");
    }

    Thread::Thread(Thread&& src) : Thread() {
        swap(*this, src);
    }

    Thread& Thread::operator=(Thread&& src) {
        swap(*this, src);
        return *this;
    }

    void swap(Thread& a, Thread& b) noexcept {
        Thread::unique_lock lockA(a.m, std::defer_lock);
        Thread::unique_lock lockB(b.m, std::defer_lock);
        std::lock(lockA, lockB);
        using std::swap;
        swap(a.log, b.log);
        swap(a.moduleName, b.moduleName);
        a.state = b.state.exchange(a.state.load());
        swap(a.thread, b.thread);
    }

    Thread::~Thread() {
        lock_guard guard(m);
        if (thread.joinable()) {
            log.WriteLine(LOG_DEBUG, moduleName, "Joining thread in destructor");
            //state = ThreadState::Stopped;
            stop();
            //thread.join();
            join();
        }
    }

    void Thread::enable() {
        lock_guard guard(m);
        switch (state.load())
        {
        case ThreadState::Created:
            log.WriteLine(LOG_DEBUG, moduleName, "Enabling");
            state = ThreadState::Enabled;
            if (thread.get_id() == std::thread::id()) {
                startThread();
            }
            break;
        case ThreadState::Enabled:
            log.WriteLine(LOG_WARN, moduleName, "Already Enabled");
            break;
        default:
            throw std::domain_error("Cannot enter Enabled state from current state.");
            break;
        }
    }

    void Thread::start() {
        lock_guard guard(m);
        switch (state.load())
        {
        case ThreadState::Created:
        case ThreadState::Enabled:
        case ThreadState::Paused:
            log.WriteLine(LOG_DEBUG, moduleName, "Starting");
            state = ThreadState::Started;
                // Check to see if thread already created
                // startThread() should only be run once
            if (thread.get_id() == std::thread::id()) {
                // Should only run if not enabled first
                startThread();
            }
            break;
        case ThreadState::Started:
            log.WriteLine(LOG_WARN, moduleName, "Already Started");
            break;
        default:
            throw std::domain_error("Cannot enter Started state from current state.");
            break;
        }
    }

    void Thread::startThread() {
        log.WriteLine(LOG_DEBUG, moduleName, "Starting Thread");
        thread = std::thread(&Thread::runInternal, this);
    }

    void Thread::pause() {
        lock_guard guard(m);
        switch (state.load())
        {
        case ThreadState::Enabled:
        case ThreadState::Started:
            log.WriteLine(LOG_DEBUG, moduleName, "Pausing");
            state = ThreadState::Paused;
            break;
        case ThreadState::Paused:
            log.WriteLine(LOG_WARN, moduleName, "Already Paused");
            break;
        default:
            throw std::domain_error("Cannot enter Paused state from current state.");
            break;
        }
    }

    void Thread::stop() {
        lock_guard guard(m);
        switch (state.load())
        {
        case ThreadState::Enabled:
        case ThreadState::Started:
        case ThreadState::Paused:
            log.WriteLine(LOG_DEBUG, moduleName, "Stopping");
            state = ThreadState::Stopped;
            break;
        case ThreadState::Stopped:
            log.WriteLine(LOG_WARN, moduleName, "Already Stopped");
            break;
        default:
            throw std::domain_error("Cannot enter Stopped state from current state.");
            break;
        }
    }

    void Thread::join() {
        lock_guard guard(m);
        if (thread.joinable()) {
            thread.join();
            state = ThreadState::Ended;
        }
        else {
            log.WriteLine(LOG_ERROR, moduleName, "Could not join thread.");
        }
    }

    ThreadState Thread::getState() const {
        return state;
    }

    std::thread::id Thread::getID() const {
        return thread.get_id();
    }

    void Thread::runInternal() {
        try {
            run();
        }
        catch (const std::exception& ex) {
            std::string msg;
            msg.append(typeid(ex).name());
            msg.append(" ");
            msg.append(ex.what());
            log.WriteLine(LOG_ERROR, moduleName, msg);
        }
        catch (...) {
            log.WriteLine(LOG_ERROR, moduleName, "Unknown exception");
        }
    }
}
