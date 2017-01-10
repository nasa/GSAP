/**  Predictor Factory - Header
 *   @class     PredictorFactory PredictorFactory.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *   @ingroup    Predictors
 *
 *   @brief     Predictor Factory Class - Factory pattern
 *   Handles the creation of new predictors
 *
 *   @author    Matthew Daigle
 *   @version   0.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 22, 2016
 *
 *   @copyright Copyright (c) 2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_PREDICTORFACTORY_H
#define PCOE_PREDICTORFACTORY_H

#include "Predictor.h"
#include "Factory.h"
#include "Singleton.h"
#include "MonteCarloPredictor.h"

namespace PCOE {
    /**
     *  @class      PredictorFactory
     *  @brief      Predictor Factory Class. Handles the creation of new predictors
     *
     *  @see        Factory
     *  @note       Uses Factory template parent class.
     **/
    class PredictorFactory : public Factory<Predictor>,
        public Singleton<PredictorFactory> {
        friend class Singleton<PredictorFactory>;

        /**
         *  @brief      PredictorFactory Constructor
         *  @note       Is protected to prevent users from creating a new PredictorFactory
         *              Consistant with the singleton pattern
         **/
        PredictorFactory() {
            Register("MC", PredictorFactory::Create<MonteCarloPredictor>);
        }
    };
}

#endif // PCOE_PREDICTORFACTORY_H
