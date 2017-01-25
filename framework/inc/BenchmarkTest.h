/**  Model-based Prognoser - Header
*   @class     ModelBasedPrognoser ModelBasedPrognoser.h
*   @ingroup   GPIC++
*   @ingroup   ProgLib
*
*   @brief     Model-based Prognoser Class
*
*   General model-based prognoser class. It gets created for a specified model, observer, and predictor.
*
*   @author    Matthew Daigle
*   @version   0.1.0
*
*   @pre       Prognostic Configuration File and Prognoster Configuration Files
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 16, 2016
*
*   @copyright Copyright (c) 2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef PCOE_MODELBASEDPROGNOSER_H
#define PCOE_MODELBASEDPROGNOSER_H

#include <memory>

#include "CommonPrognoser.h"
#include "PrognosticsModel.h"
#include "Observer.h"
#include "Predictor.h"

namespace PCOE {
    class BenchmarkPrognoser : public CommonPrognoser
    {
    private:
        std::unique_ptr<PrognosticsModel> model;
        std::unique_ptr<Observer> observer;
        std::unique_ptr<Predictor> predictor;
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;
        bool initialized;
        double firstTime;
        double lastTime;
    public:
        /** @brief      Model-based Prognoser Constructor
         *  @param      config Map of config parameters from the prognoser config file
         */
        BenchmarkPrognoser(GSAPConfigMap & config);

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
         //void checkInputValidity() {};

         /** @brief     check if there is enough new data to preform prognosis
          *  @return    if there is enough data
          *
          *             Check if the data exists and is new enough to be used for
          *             prognosis. If false is returned prognostics steps will be
          *             skipped. By default this returns true- making this step
          *             optional in the component prognoser implementation
          */
          //bool isEnoughData() {return true;};

          /** @brief     check the validity of any prognostics results.
           *
           *             This could be as simple as bound checks or a complicated
           *             analysis. By default this a simple bounds test on timeToEvent
           *             - making this step optional in the component prognoser
           *             implementation
           *             Default implemented in CommonPrognoser
           */
           //void checkResultValidity();
    };

    extern bool regModelProg;
}

#endif // PCOE_MODELBASEDPROGNOSER_H
