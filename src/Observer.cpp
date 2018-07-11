/**  Observer - Body
*   @file      Observer Interface Class
*   @ingroup   GPIC++
*   @defgroup  Observer     Observer
*
*   @brief     Observer Interface Class - Class defining the Observer interface
*
*   @author    Matthew Daigle
*   @version   1.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 5, 2016
*
*   @copyright Copyright (c) 2018 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include "Observer.h"

#include <vector>

namespace PCOE {
    // Get current time
    double Observer::getTime() const {
        return m_t;
    }

    // Get current inputs
    const std::vector<double> & Observer::getInputs() const {
        return m_uOld;
    }

    // Check if initialized
    bool Observer::isInitialized() const {
        return m_initialized;
    }
}
