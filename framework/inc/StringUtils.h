/**  String Utils - Header
 *   @ingroup   GPIC++
 *   @ingroup   Framework
 *
 *   @brief     Contains functions for operating on strings
 *
 *   @author    Chris Teubert
 *   @author    Jason Watkins
 *   @version   1.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_STRINGUTILS_H
#define PCOE_STRINGUTILS_H

#include <algorithm>
#include <cctype>
#include <functional>
#include <locale>
#include <string>

namespace PCOE {
    const char PATH_SEPARATOR =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

    // Note: Trim functions from
    // http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

    // trim from start
    static inline void trimLeadingSpace(std::string& s) {
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
    }

    // trim from end
    static inline void trimTrailingSpace(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(),
                s.end());
    }

    // trim from both ends
    static inline void trimSpace(std::string& s) {
        trimTrailingSpace(s);
        trimLeadingSpace(s);
    }

    // converts a string to all lowercase using the tolower function
    static inline void toLower(std::string& s) {
        std::transform(s.begin(), s.end(), s.begin(), tolower);
    }
}

#endif // PCOE_STRINGUTILS_H
