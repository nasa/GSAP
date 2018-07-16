/**  Model - Body
 *   @file       Model.cpp
 *   @ingroup    GSAP-Support
 *
 *   @brief      Model class
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

#include "Model.h"

#include <vector>

namespace PCOE {
    std::vector<double> Model::stateEqn(const double t,
                                        const std::vector<double>& x,
                                        const std::vector<double>& u,
                                        const std::vector<double>& n) const {
        return stateEqn(t, x, u, n, m_dt);
    }

    unsigned int Model::getNumStates() const {
        return numStates;
    }

    unsigned int Model::getNumInputs() const {
        return inputs.size();
    }

    unsigned int Model::getNumOutputs() const {
        return outputs.size();
    }

    double Model::getDt() const {
        return m_dt;
    }

    void Model::setDt(const double newDt) {
        m_dt = newDt;
    }
}
