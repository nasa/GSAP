// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGECLOCK_H
#define PCOE_MESSAGECLOCK_H
#include <chrono>
#include <ratio>

#include "Messages/MessageId.h"

namespace PCOE {
    /**
     * A clock that represents time system in messages.
     *
     * @remarks Implements the C++ named requirement @{code Clock}. See
     *          https://en.cppreference.com/w/cpp/named_req/Clock for details.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class MessageClock {
    public:
        /**
         * The type used internally to store the clock duration in ticks.
         **/
        using rep = std::uint64_t;

        /**
         * A ratio representing the tick period of the clock in seconds.
         **/
        using period = std::micro;

        /**
         * The duration type of the clock.
         *
         * @remarks This duration is defined in terms of rep and period rather
         *          than relying on std::chrono::microseconds to make absolutely
         *          sure that the representation does not change. This is
         *          important since messages may be serialized across the
         *          network to many different systems.
         **/
        using duration = std::chrono::duration<rep, period>;

        /**
         * The time point type of the clock.
         **/
        using time_point = std::chrono::time_point<MessageClock>;

        // TODO (JW): I would really prefer that this clock be steady, but that
        // isn't possible as long as we are relying solely on
        // std::chrono::system_clock as a time source, since the system clock is
        // itself not steady.
        // TODO (JW): I can't remember anymore why we are using system_clock
        // rather than std::chrono::steady_clock. Should look into that and
        // document it here.
        /**
         * Indicates that sucsessive calls to @{code now} do not yield
         * monotonically increasing results.
         **/
        static const bool is_steady = false;

        /**
         * Gets the current time as represented by this clock.
         **/
        static time_point now() {
            using namespace std::chrono;
            system_clock::time_point now = system_clock::now();
            return time_point(duration_cast<duration>(now.time_since_epoch()));
        }
    };

    /**
     * Convert a std::chrono::time_point to a floating point representation
     * of the number of seconds since the epoch.
     *
     * @author Jason Watkins
     * @since 1.2
     *
     * @paramt Clock    The clock used by the time_point being operated on.
     * @paramt Duration The duration of the time_point being operated on.
     * @param  time     The time_point to convert.
     * @return          The fractional number of seconds since the clock's
     *                  epoch.
     **/
    template <class Clock, class Duration = typename Clock::duration>
    double seconds(const std::chrono::time_point<Clock, Duration>& time) {
        using time_point = std::chrono::time_point<Clock, Duration>;

        // Note (JW): For reasonable clocks and times, I expect that count and
        //            den will be "large", and num will be "small" (probably 1).
        //            Therefore, I expect (count * num) / den will generally
        //            yield a more precise result than count * (num / den).
        auto num = static_cast<double>(time_point::period::num);
        auto den = static_cast<double>(time_point::period::den);
        auto count = static_cast<double>(time.time_since_epoch().count());
        return (count * num) / den;
    }
}
#endif
