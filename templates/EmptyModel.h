/**  EmptyModel - Header
 *   @file       EmptyModel.h
 *   @ingroup    GSAP-Support
 *
 *   @brief      EmptyModel model class for prognostics
 *
 *   @author     Matthew Daigle
 *   @version    1.1.0
 *
 *   @pre        N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: January 10, 2017
 *
 *   @copyright Copyright (c) 2017-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef EmptyModel_H
#define EmptyModel_H

#include "ConfigMap.h"
#include "Models/Model.h"

class EmptyModel final : public PCOE::Model {
public:
    // Constructor
    EmptyModel();

    // Constructor based on configMap
    EmptyModel(const PCOE::ConfigMap& paramMap);

    /**
     * Calculate the model state using the given sampling time.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @param u  The model input vector at the current time step.
     * @param n  The process noise vector.
     * @param dt The size of the time step to calculate
     * @return   The model state vector at the next time step.
     **/
    state_type stateEqn(double t,
                        const state_type& x,
                        const input_type& u,
                        const noise_type& n,
                        double dt) const override;

    /**
     * Calculate the model output.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @param u  The model input vector at the current time step.
     * @param n  The process noise vector.
     * @param dt The size of the time step to calculate
     * @return   The model output vector at the next time step.
     **/
    output_type outputEqn(double t,
                          const state_type& x,
                          const input_type& u,
                          const noise_type& n) const override;

    /**
     * Initialize the model state.
     *
     * @param u The model input vector.
     * @param z The model output vector.
     * @returns The initial model state vector.
     **/
    state_type initialize(const input_type& u, const output_type& z) const override;
};

#endif
