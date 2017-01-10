/** @class     Thread
 * 
 *  @brief     Abstract class that controls a thread of execution and provides
 *             state information for inheritors to use.
 * 
 *  @author    Jason Watkins <jason-watkins@outlook.com>
 *  @author    Chris Teubert <christopher.a.teubert@nasa.gov>
 *  @date      2016-06-21
 *  @version   0.1.0
 * 
 *  @copyright Copyright (c) 2016 United States Government as represented by
 *    the Administrator of the National Aeronautics and Space Administration.
 *    All Rights Reserved.
 */

#ifndef PCOE_THREAD_H
#define PCOE_THREAD_H

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include "ThreadSafeLog.h"

namespace PCOE {
    /** Specifies the execution states of a @see{Thread}.
     * 
     *  @remarks The states represented by this enum are not used directly by
     *           the @see{Thread} class. Classes that inherit from run may
     *           support these states in their override of the
     *           @see{Thread::run} method.
     * 
     *  @author  Jason Watkins <jason-watkins@outlook.com>
     *  @author  Chris Teubert <christopher.a.teubert@nasa.gov>
     *  @date    2016-06-21
     *  @version 0.1.0
     */
    enum class ThreadState {
        Created, /**< The thread has not been enabled or started yet. */
        Enabled, /**< The thread has started executing, but isn't doing work yet. */
        Started, /**< The thread has started and is performing work. */
        Paused, /**< The thread is continuing to execute, but is not performing work. */
        Stopped, /**< The thread has been stopped, but may still be executing. */
        Ended /**< The thread has terminated */
    };

    class Thread {
    public:
        /** @brief Initializes a new instance of the @see{Thread} class that
         *         uses the default logger and has not system thread associated
         *         with it yet.
         */
        Thread();

        /** Deleted. The underlying thread implementation may not allow copying.
         */
        Thread(const Thread& other) = delete;

        /** @brief Initializes a new instance of the @see{Thread} class that
         *         represents the same thread of execution as @p other. This
         *         leaves @p other in the default state.
         * 
         *  @param[in,out] other The @see{Thread} from which to contruct this thread.
         */
        Thread(Thread&& other);

        /** Deleted. The underlying thread implementation may not allow copying.
         */
        Thread& operator=(const Thread& other) = delete;

        /** @brief Assigns the information from @p other to the current
         *         @see{Thread}. This leaves @p other in the default state.
         * 
         *  @param[in,out] other The @see{Thread} from which to assign this thread.
         */
        Thread& operator=(Thread&& other);

        /** @brief Swaps the information in two @see{Thread} objects.
         * 
         *  @param[in,out] a The first @see{Thread} to swap.
         *  @param[in,out] b The second @see{Thread} to swap.
         */
        friend void swap(Thread& a, Thread& b) noexcept;

        /** @brief Cleans up thread resources. Sets the thread state to stopped
          *        and joins the thread. The destructor returns when the thread
          *        terminates.
          */
        virtual ~Thread();

        /** Enables the underlying thread and sets the Thread state to Enabled.
         * 
         *  @remarks If the underlying thread of execution has not yet been created,
         *           calling enable will create it.
         * 
         *  @exception std::runtime_error If the thread is not in the Created or
         *                                Enabled state.
         */
        virtual void enable();

        /** Enables the underlying thread and sets the Thread state to Started.
         * 
         *  @remarks If the underlying thread of execution has not yet been created,
         *           calling enable will create it.
         * 
         *  @exception std::runtime_error If the thread is not in the Created,
         *                                Enabled, Started, or Paused state.
         */
        virtual void start();

        /** Sets the Thread state to Paused.
         * 
         *  @exception std::runtime_error If the thread is not in the Enabled,
         *                                Started, or Paused state.
         */
        virtual void pause();

        /** Sets the Thread state to Stopped.
         * 
         *  @exception std::runtime_error If the thread is not in the Enabled,
         *                                Started, Paused, or Stopped state.
         */
        virtual void stop();

        /** Blocks the calling thread until the thread of execution terminates.
         */
        void join();

        /** Gets the current state of the thread.
         */
        ThreadState getState() const;

        /** Gets the ID of the underlying thread of execution.
         */
        std::thread::id getID() const;

    protected:
        /** @brief When overriden in a derived class, provides the function
         *         that is excuted on the underlying thread.
         */
        virtual void run() = 0;

        inline void setState(ThreadState ts) noexcept {
            state = ts;
        }

        Log log; /**< The log file used to log information about the thread. */
        std::string moduleName; /**< The name of the module that owns the thread. */

    private:
        void startThread();
        void runInternal();

        using mutex = std::recursive_mutex;
        using lock_guard = std::lock_guard<mutex>;
        using unique_lock = std::unique_lock<mutex>;
        mutex m;
        std::atomic<ThreadState> state;
        std::thread thread;
    };
}
#endif // PCOE_THREAD_H
