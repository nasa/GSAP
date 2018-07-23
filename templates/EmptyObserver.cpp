/**  EmptyObserver - Body
 *   @file       EmptyObserver.cpp
 *   @ingroup    GPIC++
 *
 *   @copyright Copyright (c) 2017-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <algorithm>
#include <string>
#include <vector>
#include <cmath>

#include "Model.h"
#include "UData.h"

#include "Exceptions.h"
#include "EmptyObserver.h"

namespace PCOE {
    const static Log& log = Log::Instance();
    
    const std::string MODULE_NAME = "OBS-Empty";
    
    EmptyObserver::EmptyObserver(const Model* m, const Matrix Q, const Matrix R) : Observer(m) {
        
    }
    
    // Initialize function (required by Observer interface)
    void EmptyObserver::initialize(const double t0,
                                                const Model::state_type & x0,
                                                const Model::input_type & u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");
        Model::state_type initialized_state;
        
        // Initialization activities
        // ...
        
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }
    
    // Step function (required by Observer interface)
    void EmptyObserver::step(double t, const Model::input_type& u, const Model::output_type& z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");
        
        if (!isInitialized()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Called step before initialized");
            throw std::domain_error("EmptyObserver::step not initialized");
        }
        
        // Perform state estimation
        // Update xEstimated here
        // ...
    }
    
    std::vector<UData> EmptyObserver::getStateEstimate() const {
        std::vector<UData> state(model->getStateSize());
        
        // Fill state
        
        return state;
    }
}
