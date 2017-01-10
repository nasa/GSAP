/**  EmptyPredictor - Header
*   @file      EmptyPredictor Class
*   @ingroup   GPIC++
*   @ingroup    Predictors
*
*   @brief     EmptyPredictor Class - Class defining the EmptyPredictor
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: January 10, 2017
*
*   @copyright Copyright (c) 2017 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include <random>
#include <string>
#include <vector>

#include "Exceptions.h"
#include "EmptyPredictor.h"
#include "Matrix.h"

namespace PCOE {

    // ConfigMap-based Constructor
    EmptyPredictor::EmptyPredictor(GSAPConfigMap & configMap) : Predictor() {
        // Setup based on configuration parameters
        // ...

        log.WriteLine(LOG_INFO, MODULE_NAME, "EmptyPredictor created");
    }

    // Set model
    void EmptyPredictor::setModel(PrognosticsModel * model) {
        pModel = model;

        // Perform some checks on the model
        // ...
    }

    // Predict function
    void EmptyPredictor::predict(const double tP, const std::vector<UData> & state, ProgData & data) {
        // Check that model has been set
        if (pModel == NULL) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "EmptyPredictor does not have a model!");
            throw ConfigurationError("EmptyPredictor does not have a model!");
        }
        
        // Run prediction, and fill in the prog data
        // ...
    }
}
