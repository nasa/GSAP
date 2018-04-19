/**  Random Communicator - Body
 *   @class     RandomCommunicator RandomCommunicator.h
 *   @ingroup   GPIC++
 *   @ingroup   commCollection
 *
 *   @brief     Random Communicator class- sets all input data to random number between 0-MaxRand with step StepSize
 *
 *   @note      This class will look for the following optional configuration parameters:
 *                  max     The maximum random number generated (default 10000)
 *                  step    The step size between possible random numbers (default 0.5)
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 21, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <cmath>
#include <cstdlib>
#include <string>

#include "RandomCommunicator.h"

namespace PCOE {
    // Defaults
    const unsigned long DEFAULT_MAX_RAND = 10000;
    /// Default Maximum random number
    const double DEFAULT_STEP_SIZE = 0.5;
    /// Default Stepsize

    // Configuration Keys
    const std::string STEP_KEY = "step";
    const std::string MAX_KEY = "max";

    // Log Parameters
    const std::string MODULE_NAME = "RandomComm";

    unsigned int seed = static_cast<unsigned int>(time(nullptr));

    RandomCommunicator::RandomCommunicator(const ConfigMap & config) :
        maxRand(DEFAULT_MAX_RAND),
        stepSize(DEFAULT_STEP_SIZE) {
        // Handle Configuration
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Configuring");
        if (config.includes(STEP_KEY)) {
            stepSize = std::stod(config.at(STEP_KEY)[0]);
            log.FormatLine(LOG_DEBUG, MODULE_NAME,
                "Configuring- Step size set to %f", stepSize);
        }

        if (config.includes(MAX_KEY)) {
            maxRand = std::stoul(config.at(MAX_KEY)[0]);
            log.FormatLine(LOG_DEBUG, MODULE_NAME,
                "Configuring- Max Rand set to %ul", maxRand);
        }
        maxRand = static_cast<unsigned long>(ceil(maxRand / stepSize));
        log.WriteLine(LOG_TRACE, MODULE_NAME,
            "Completed Configuration");
    }

    DataStore RandomCommunicator::read() {
        log.WriteLine(LOG_TRACE, MODULE_NAME,
            "Step- Setting each variable to a random number");

        // Set every variable to a random number between 0-MaxRand
        //  with a step of StepSize
        for (auto & dataIt : data) {
            dataIt.second = static_cast<double>(static_cast<unsigned long>(rand()) % maxRand)*stepSize;
            log.FormatLine(LOG_TRACE, MODULE_NAME,
                "Step- Setting %s to %f",
                dataIt.first.c_str(),
                static_cast<double>(dataIt.second));
        }
        return data;
    }

    void RandomCommunicator::write(AllData dataIn) {
        data = dataIn.doubleDatastore;
    }
}
