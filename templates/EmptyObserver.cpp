/**  EmptyObserver - Body
 *   @file       EmptyObserver.cpp
 *   @ingroup    GPIC++
 *   @defgroup   Observer     Observer
 *
 *   @brief      EmptyObserver class
 *
 *   @author     Matthew Daigle
 *   @version    0.1.0
 *
 *   @pre        N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: January 10, 2017
 *
 *   @copyright Copyright (c) 2017 United States Government as represented by
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
    
    // Set model
    void EmptyObserver::setModel(Model * model) {
        // Set the model pointer
        // pModel = model;
    }
    
    // Initialize function (required by Observer interface)
    void EmptyObserver::initialize(const double t0, const std::vector<double> & x0,
                                           const std::vector<double> & u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");
        
        // Check that model has been set
        if (pModel == NULL) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "EmptyObserver does not have a model!");
            throw ConfigurationError("EmptyObserver does not have a model!");
        }
        
        // Initialize time, inputs
        m_t = t0;
        m_uOld = u0;
        
        // Other initialization activities
        // ...
        
        // Set initialized flag
        m_initialized = true;
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }
    
    // Get state mean
    const std::vector<double> & EmptyObserver::getStateMean() const {
        // return xEstimated;
    }
    
    // Get output mean
    const std::vector<double> & EmptyObserver::getOutputMean() const {
        // return zEstimated;
    }
    
    // Step function (required by Observer interface)
    void EmptyObserver::step(const double newT, const std::vector<double> & u,
                                     const std::vector<double> & z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");
        
        if (!isInitialized()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Called step before initialized");
            throw std::domain_error("EmptyObserver::step not initialized");
        }
        
        // Update time
        double dt = newT - m_t;
        m_t = newT;
        if (dt <= 0) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "dt is less than or equal to zero");
            throw std::domain_error("EmptyObserver::step dt is 0");
        }
        
        // Perform state estimation
        // ...
        
        // Update uOld
        m_uOld = u;
    }
    
    std::vector<UData> EmptyObserver::getStateEstimate() const {
        // return state;
    }
}
