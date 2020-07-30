// Copyright(c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <stdexcept>
#include <sstream>

namespace PCOE {
    /** @brief Defines a type of object to be thrown as exception. It reports
     *         errors that are due to improperly formed program inputs.
     */
    class FormatError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    /** @brief Defines a type of object to be thrown as exception. It reports
     *         errors that are due to improperly configured objects
     */
    class ConfigurationError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    /** @brief Defines a type of object to be thrown as exception. It reports
     *         errors that are due to out of range element access
     */
    class OutOfRangeError : public std::out_of_range {
    public:
        using std::out_of_range::out_of_range;
    };
}
