// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PROGNOSTICSMODEL_H
#define PCOE_PROGNOSTICSMODEL_H

#include "SystemModel.h"

#include <vector>

namespace PCOE {
    class PredictedOutputVector final : public DynamicArray<double> {
    public:
        using DynamicArray::DynamicArray;

        using DynamicArray::operator=;

        using DynamicArray::operator[];
    };

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
        using observables_type = PredictedOutputVector;

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
         * @param inputParamCount  The number of input parameters required by
         *                         inputEqn.
         **/
        PrognosticsModel(state_type::size_type stateSize,
                         std::vector<MessageId> inputs,
                         std::vector<MessageId> outputs,
                         std::vector<std::string> observables,
                         std::vector<MessageId> events,
                         size_type inputParamCount)
            : SystemModel(stateSize, inputs, outputs),
              events(events),
              observables(observables),
              inputParameterCount(inputParamCount) {}

        /**
         * Initializes the model with the given parameters, setting the number
         * of input parameters to the size of the input vector.
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
            : PrognosticsModel(stateSize,
                               inputs,
                               outputs,
                               observables,
                               events,
                               inputs.size()) {}

        virtual ~PrognosticsModel() = default;

        /**
         * Calculate whether the model threshold is reached.
         *
         * @param t Time
         * @param x The model state vector at the current time step.
         * @return  true if the threshold is reached; otherwise, false.
         **/
        virtual bool thresholdEqn(const double t,
                                  const state_type& x) const = 0;

        /**
         * Calculate event state.
         *
         * @param x  The model state vector at the current time step.
         * @return   The
         */
        virtual event_state_type eventStateEqn(const state_type& x) const = 0;

        /** Calculate observables of the model. Observables are those
         * that are not measured, but are interested in being predicted for
         * prognostics.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @return   The model output vector at the next time step.
         **/
        virtual observables_type observablesEquation(const double t,
                                                     const state_type& x) const {
            return getObservablesVector();
        };

        /**
         * Gets the number of input parameters required by the current model.
         **/
        inline size_type getInputParameterCount() const {
            return inputParameterCount;
        }

        inline observables_type getObservablesVector() const {
            return observables_type(observables.size());
        }

        inline const std::vector<std::string>& getObservables() const {
            return observables;
        }

        inline const std::vector<MessageId>& getEvents() const {
            return events;
        }

    private:
        std::vector<MessageId> events;
        std::vector<std::string> observables;
        size_type inputParameterCount;
    };
}
#endif
