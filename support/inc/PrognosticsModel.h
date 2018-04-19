/**  PrognosticsModel - Header
*   @file       PrognosticsModel.h
*   @ingroup    GSAP-Support
*
*   @brief      PrognosticsModel class.
*               Extends Model class to add threshold, input, and predicted output equations.
*
*   @author     Matthew Daigle
*   @version    0.1.0
*
*   @pre        N/A
*
*      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
*      Created: March 5, 2016
*
*   @copyright Copyright (c) 2018 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#ifndef PCOE_PROGNOSTICSMODEL_H
#define PCOE_PROGNOSTICSMODEL_H

#include "Model.h"

#include <vector>

namespace PCOE {
    class PrognosticsModel : public Model {
     protected:
        unsigned int numInputParameters;    // Number of input parameters used in inputEqn
        unsigned int numPredictedOutputs;   // Number of predicted outputs

     public:
        virtual ~PrognosticsModel() = default;

        /** @brief      Execute threshold equation
        *   @param      t Time
        *   @param      x State vector
        *   @param      u Input vector
        **/
        virtual bool thresholdEqn(const double t, const std::vector<double> & x,
            const std::vector<double> & u) = 0;
        /** @brief      Execute input equation.
        *               Determines what input (u) should be at the given time for the given input parameters.
        *   @param      t Time
        *   @param      inputParameters Vector of input parameters, which are values that specify how to define u for the given time.
        *   @param      u Input vector. Gets overwritten.
        **/
        virtual void inputEqn(const double t, const std::vector<double> & inputParameters,
            std::vector<double> & u) = 0;
        
        /** @brief      Execute predicted output equation.
        *               Predicted outputs are those that are not measured, but are interested in being predicted for prognostics.
        *   @param      t Time
        *   @param      x State vector
        *   @param      u Input vector
        *   @param      z Predicted output vector. Gets overwritten.
        **/
        virtual void predictedOutputEqn(const double t, const std::vector<double> & x,
            const std::vector<double> & u, std::vector<double> & z) = 0;

        inline unsigned int getNumInputParameters() const { return numInputParameters; }
        inline unsigned int getNumPredictedOutputs() const { return numPredictedOutputs; }
    };
}
#endif  // PCOE_PROGNOSTICSMODEL_H
