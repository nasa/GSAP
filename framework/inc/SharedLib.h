/**  Shared Library - Header
 *   @class     SharedLib SharedLib.h
 *   @ingroup   GPIC++
 *   @ingroup   Framework
 *
 *   @brief     Shared Library. The shared library includes shared consts and functions that are used in the framework, communicator, prognosers, and support
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_SHAREDLIB_H
#define PCOE_SHAREDLIB_H

#include <string>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <chrono>

namespace PCOE {
    const char PATH_SEPARATOR =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

    // Note: Trim functions from http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

    // trim from start
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    }

    // trim from end
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }

    // trim from both ends
    static inline void trim(std::string &s) {
        rtrim(s);
        ltrim(s);
    }

    static inline unsigned long long millisecondsNow() {
        using std::chrono::system_clock;
        using std::chrono::milliseconds;
        return static_cast<unsigned long long>(system_clock::now().time_since_epoch() / milliseconds(1));
    }

    // converts a string to all lowercase using the tolower function
    static inline void lower(std::string &s){
          std::transform(s.begin(), s.end(), s.begin(), tolower);

  }
}

#endif // PCOE_SHAREDLIB_H
