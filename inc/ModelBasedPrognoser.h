/**  Model-based Prognoser - Header
 *   @class     ModelBasedPrognoser ModelBasedPrognoser.h
 *   @ingroup   GPIC++
 *   @ingroup   ProgLib
 *
 *   @brief     Model-based Prognoser Class
 *
 *   General model-based prognoser class. It gets created for a specified model, observer, and
 * predictor.
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 16, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_MODELBASEDPROGNOSER_H
#define PCOE_MODELBASEDPROGNOSER_H

#include <memory>

#include "Observers/Observer.h"
#include "Predictors/Predictor.h"
#include "Prognoser.h"
#include "PrognosticsModel.h"

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
        /** @brief      Model-based Prognoser Constructor
         *  @param      config Map of config parameters from the prognoser config file
         */
        ModelBasedPrognoser(ConfigMap& config);

        /** @brief     Prognostic Monitor Step
         *
         *             Preform model updates. This is done every step where there is
         *             enough data. This is a required method in any component
         *             prognoser
         */
        void step();

        //*------------------------------------------------------*
        //|          Optional Methods- Uncomment to use          |
        //*------------------------------------------------------*

        /** @brief     check the validity of any input (sensor) data.
         *
         *             This could be as simple as bound checks or a complicated
         *             analysis. By default this is not done- making this step
         *             optional in the component prognoser implementation
         */
        // void checkInputValidity() {};

        /** @brief     check if there is enough new data to preform prognosis
         *  @return    if there is enough data
         *
         *             Check if the data exists and is new enough to be used for
         *             prognosis. If false is returned prognostics steps will be
         *             skipped. By default this returns true- making this step
         *             optional in the component prognoser implementation
         */
        // bool isEnoughData() {return true;};

        /** @brief     check the validity of any prognostics results.
         *
         *             This could be as simple as bound checks or a complicated
         *             analysis. By default this a simple bounds test on timeToEvent
         *             - making this step optional in the component prognoser
         *             implementation
         *             Default implemented in Prognoser
         */
        // void checkResultValidity();
    };

    extern bool regModelProg;
}

#endif // PCOE_MODELBASEDPROGNOSER_H
