// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PROGNOSTICSMODEL_H
#define PCOE_PROGNOSTICSMODEL_H

#include "Model.h"

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
     * @since 1.0
     **/
    class PrognosticsModel : public Model {
    public:
        using predicted_output_type = PredictedOutputVector;

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
         * @param predictedOutputs The names of the predicted outputs produced
         *                         by the model.
         * @param inputParamCount  The number of input parameters required by
         *                         inputEqn.
         **/
        PrognosticsModel(state_type::size_type stateSize,
                         std::vector<MessageId> inputs,
                         std::vector<MessageId> outputs,
                         std::vector<std::string> predictedOutputs,
                         std::vector<MessageId> events,
                         size_type inputParamCount)
            : Model(stateSize, inputs, outputs),
              events(events),
              predictedOutputs(predictedOutputs),
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
         * @param predictedOutputs The names of the predicted outputs produced
         *                         by the model.
         **/
        PrognosticsModel(state_type::size_type stateSize,
                         std::vector<MessageId> inputs,
                         std::vector<MessageId> outputs,
                         std::vector<std::string> predictedOutputs,
                         std::vector<MessageId> events)
            : PrognosticsModel(stateSize,
                               inputs,
                               outputs,
                               predictedOutputs,
                               events,
                               inputs.size()) {}

        virtual ~PrognosticsModel() = default;

        /**
         * Calculate whether the model threshold is reached.
         *
         * @param t Time
         * @param x The model state vector at the current time step.
         * @param u The model input vector at the current time step.
         * @return  true if the threshold is reached; otherwise, false.
         **/
        virtual bool thresholdEqn(const double t,
                                  const state_type& x,
                                  const input_type& u) const = 0;

        /**
         * Calculate event state.
         *
         * @param x  The model state vector at the current time step.
         * @return   The
         */
        virtual event_state_type eventStateEqn(const state_type& x) const = 0;

        /** Calculate predicted outputs of the model. Predicted outputs are those
         * that are not measured, but are interested in being predicted for
         * prognostics.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @param u  The model input vector at the current time step.
         * @param z  The model output vector at the current time step.
         * @return   The model output vector at the next time step.
         **/
        virtual predicted_output_type predictedOutputEqn(const double t,
                                                         const state_type& x,
                                                         const input_type& u,
                                                         const output_type& z) const = 0;

        /**
         * Gets the number of input parameters required by the current model.
         **/
        inline size_type getInputParameterCount() const {
            return inputParameterCount;
        }

        inline predicted_output_type getPredictedOutputVector() const {
            return predicted_output_type(predictedOutputs.size());
        }

        inline const std::vector<std::string>& getPredictedOutputs() const {
            return predictedOutputs;
        }

        inline const std::vector<MessageId>& getEvents() const {
            return events;
        }

    private:
        std::vector<MessageId> events;
        std::vector<std::string> predictedOutputs;
        size_type inputParameterCount;
    };
}
#endif
