/**  Model-based Prognoser - Header
 *   @class     ModelBasedPrognoser ModelBasedPrognoser.h
 *   @ingroup   GPIC++
 *   @ingroup   ProgLib
 *
 *   @brief     Model-based Prognoser Class
 *
 *   General model-based prognoser class. It gets created for a specified model, observer,
 *   predictor, and load-estimator.
 *
 *   @author    Matthew Daigle
 *   @author    Christopher Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognoster Configuration Files or all of the following: model, observer, predictor, load estimator
 *
 *      Created: March 16, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_MODELBASEDPROGNOSER_H
#define PCOE_MODELBASEDPROGNOSER_H

#include <memory>
#include <map>

#include "Models/PrognosticsModel.h"
#include "Observers/Observer.h"
#include "Prognoser.h"

namespace PCOE {
    class ModelBasedPrognoser : public Prognoser {
    private:
        std::unique_ptr<PrognosticsModel> model;
        std::unique_ptr<Observer> observer;
        std::unique_ptr<Predictor> predictor;
        std::unique_ptr<LoadEstimator> loadEstimator;
        bool initialized;
        double lastTime;

    public:
        /** @brief      Create a model based prognoser from configuration
         *  @param      config Map of config parameters from the prognoser config file
         */
        ModelBasedPrognoser(ConfigMap& config);
	    
	    /** @brief 	    Create a model based prognoser from parts
	     *    @param	    mdl	    Prognostics model
	     *    @param	    obs	    Observer
	     *    @param	    pred    Predictor
	     *  @param	    ldest    Load Estimator
	     **/
	    ModelBasedPrognoser(PrognosticsModel & mdl, Observer & obs, Predictor & pred, LoadEstimator & ldest);

        /** @brief     Prognostic Monitor Step
         *
         *             Preform model updates. This is done every step where there is
         *             enough data. This is a required method in any component
         *             prognoser
         */
	    Prediction step(std::map<MessageId, Datum<double> > data);
    };

    extern bool regModelProg;
}

#endif // PCOE_MODELBASEDPROGNOSER_H
