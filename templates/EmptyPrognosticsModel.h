/**  @copyright Copyright (c) 2016-2019 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */
#ifndef EmptyPrognosticsModel_H
#define EmptyPrognosticsModel_H

#include <vector>

#include "ConfigMap.h"
#include "Models/PrognosticsModel.h"

class EmptyPrognosticsModel final : public PCOE::PrognosticsModel {
public:
    // Constructor
    EmptyPrognosticsModel();

    // Constructor based on configMap
    EmptyPrognosticsModel(const PCOE::ConfigMap& paramMap);

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
                        double dt) const override;

    /**
     * Calculate the model output.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @param n  The process noise vector.
     * @return   The model output vector at the next time step.
     **/
    output_type outputEqn(double t,
                          const state_type& x) const override;

    /**
     * Initialize the model state.
     *
     * @param u The model input vector.
     * @param z The model output vector.
     * @returns The initial model state vector.
     **/
    state_type initialize(const input_type& u, const output_type& z) const override;

    /** @brief      Execute threshold equation
     *   @param     t Time
     *   @param     x State vector
     *  @return     For each event: true if threshold is reached, otherwise false
     **/
    std::vector<bool> thresholdEqn(double t, const state_type& x) const override;

    /** Calculate observables of the model. Observables are those
     * that are not measured, but are interested in being predicted for
     * prognostics.
     *
     * @param t  Time
     * @param x  The model state vector at the current time step.
     * @return   The model output vector at the next time step.
     **/
    observables_type observablesEqn(double t,
                                             const state_type& x) const override;
};
#endif
