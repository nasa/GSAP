// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PROGNOSTICSMODEL_H
#define PCOE_PROGNOSTICSMODEL_H

#include "SystemModel.h"

#include <vector>

namespace PCOE {
    /**
     * Interface for an extended model with threshold, input and predicted
     * outputs equations.
     *
     * @author Matthew Daigle
     * @author Jason Watkins
     * @author Julian Vu
     * @author Chris Teubert
     * @since 1.0
     **/
    class PrognosticsModel : public SystemModel {
    public:
        /**
         * Initializes the model with the given parameters.
         *
         * @param stateSize        The number of values in the state vector.
         * @param inputs           The names of the model inputs. The size of
         *                         this parameter also determines the number of
         *                         values in the input vector.
         * @param outputs          The names of the model outputs. The size of
         *                         this parameter also determines the number of
         *                         values in the output vector.
         * @param observables      The names of the observables produced
         *                         by the model.
         **/
        PrognosticsModel(state_type::size_type stateSize,
                         std::vector<MessageId> inputs,
                         std::vector<MessageId> outputs,
                         std::vector<std::string> observables,
                         std::vector<MessageId> events)
            : SystemModel(stateSize, inputs, outputs, observables, events) {}

        virtual ~PrognosticsModel() = default;

        /**
         * Calculate whether the model threshold is reached.
         *   Default checks if any eventState has reached zero
         *
         * @param t Time
         * @param x The model state vector at the current time step.
         * @return  For each event: true if the threshold is reached; otherwise, false.
         **/
        virtual std::vector<bool> thresholdEqn(double,
                                               const state_type& x) const {
            auto eventState = eventStateEqn(x);
            
            std::vector<bool> thresholdMet(eventState.size());
            
            for (size_type i = 0; i < eventState.size(); i++) {
                thresholdMet[i] = eventState[i] <= 0.0;
            }
            return thresholdMet;
        }
    };
}
#endif
