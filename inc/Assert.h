// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_ASSERT_H
#define PCOE_ASSERT_H

#include "ThreadSafeLog.h"
#include <stdexcept>

namespace PCOE {
    /// An exception that is thrown when an assertion is violated.
    ///
    /// \author Jason Watkins
    /// \since 1.2
    class AssertException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    [[noreturn]] inline void LogAndThrow(const std::string& str) {
        Log& log = Log::Instance();
        log.WriteLine(LOG_ERROR, "ASSERT", str);
        throw PCOE::AssertException(str);
    }
}

#if (0) // TODO(JW): Document a release flag and check whether it's defined here
#if defined(__GNUC__)
#define ASSUME(x) (x ? static_cast<void>(0) : __builtin_unreachable())
#elif defined(_MSC_VER)
#define ASSUME(x) __assume(x)
#else
#define ASSUME(x) static_cast<void>(0)
#endif

#define Expect(cond, msg) ASSUME(cond)
#define Ensure(cond, msg) ASSUME(cond)

#else

// Preprocessor black magic to make numeric values (like __LINE__) into strings
#define MAKE_STRING_INNER(x) #x
#define MAKE_STRING(x) MAKE_STRING_INNER(x)

// the LIKELY macro converts its argument to 0 or 1 and, on supported platforms,
// signals to the branch predictor that the condition should be true.
#if defined(__clang__)
#define LIKELY(x) __builtin_expect(!!(x), 1)
#elif defined(__GNUC__)
#define LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define LIKELY(x) (!!(x))
#endif

#define CONTRACT_REQUIRE(type, cond, msg) \
    (LIKELY(cond) \
         ? static_cast<void>(0) \
         : LogAndThrow(type " violated at " __FILE__ ": " MAKE_STRING(__LINE__) ". " msg))

/// Describes a function precondition that must be satisfied. In debug builds,
/// this function will throw an exception when the constraint is violated.
///
/// \author Jason Watkins
/// \since 1.2
///
/// \param cond The condition that must be satisfied
/// \param msg  The message that will be included in the thrown exception if the
///             condition is violated.
#define Expect(cond, msg, log) CONTRACT_REQUIRE("Precondition", cond, msg)

/// Describes a function postcondition that must be satisfied. In debug builds,
/// this function will throw an exception when the constraint is violated.
///
/// \author Jason Watkins
/// \since 1.2
///
/// \param cond The condition that must be satisfied
/// \param msg  The message that will be included in the thrown exception if the
///             condition is violated.
#define Ensure(cond, msg, log) CONTRACT_REQUIRE("Postcondition", cond, msg)

#endif

#endif
