// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MODEL_H
#define PCOE_MODEL_H

#include <string>
#include <vector>

#include "DynamicArray.h"
#include "Matrix.h"

namespace PCOE {
    class InputVector final : public DynamicArray<double> {
    public:
        using DynamicArray::DynamicArray;

        using DynamicArray::operator=;

        using DynamicArray::operator[];
    };

    class OutputVector final : public DynamicArray<double> {
    public:
        using DynamicArray::DynamicArray;

        using DynamicArray::operator=;

        using DynamicArray::operator[];
    };

    class StateVector final : public DynamicArray<double> {
    public:
        using DynamicArray::DynamicArray;

        using DynamicArray::operator=;

        using DynamicArray::operator[];
    };

    /**
     * Represents a State-space representation model of a system. In particular,
     * The following variables are used throughout the interface as defined
     * in the linked Wikipedia article:
     * t: Continuous time
     * x: The state vector
     * y: The output vector
     * u: The input vector
     * A: The state matrix
     * B: The input matrix
     * C: The output matrix
     * D: The feedthrough matrix
     *
     * In addition, the following variables are added to the description above:
     * n: The process noise vector.
     *
     * @see https://en.wikipedia.org/wiki/State-space_representation
     * @author Mattew Daigle
     * @author Jason Watkins
     * @since 1.0
     **/
    class Model {
    public:
        using size_type = std::size_t;
        using input_type = InputVector;
        using output_type = OutputVector;
        using state_type = StateVector;
        using noise_type = std::vector<double>;

        /**
         * Initializes the model with the given parameters.
         *
         * @param stateSize The number of values in the state vector.
         * @param inputs    The names of the model inputs. The size of this
         *                  parameter also determines the number of values in
         *                  the input vector.
         * @param outputs   The names of the model outputs. The size of this
         *                  parameter also determines the number of values in
         *                  the output vector.
         **/
        Model(state_type::size_type stateSize,
              std::vector<std::string> inputs,
              std::vector<std::string> outputs)
            : stateSize(stateSize), inputs(inputs), outputs(outputs) {}

        virtual ~Model() = default;

        /**
         * Calculate the model state using the default sampling time.
         *
         * @param t Time
         * @param x The model state vector at the current time step.
         * @param u The model input vector at the current time step.
         * @param n The process noise vector.
         * @return  The model state vector at the next time step.
         **/
        inline state_type stateEqn(const double t,
                                   const state_type& x,
                                   const input_type& u,
                                   const noise_type& n) const {
            return stateEqn(t, x, u, n, defaultTimeStep);
        }

        /**
         * Calculate the model state using the given sampling time.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @param u  The model input vector at the current time step.
         * @param n  The process noise vector.
         * @param dt The size of the time step to calculate.
         * @return   The model state vector at the next time step.
         **/
        virtual state_type stateEqn(const double t,
                                    const state_type& x,
                                    const input_type& u,
                                    const noise_type& n,
                                    const double dt) const = 0;

        /**
         * Calculate the model output.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @param u  The model input vector at the current time step.
         * @param n  The process noise vector.
         * @param z  The model output vector at the current time step.
         * @return   The model output vector at the next time step.
         **/
        virtual output_type outputEqn(const double t,
                                      const state_type& x,
                                      const input_type& u,
                                      const noise_type& n) const = 0;

        /**
         * Initialize the model state.
         *
         * @param u The initial model input vector.
         * @param z The initial model output vector.
         * @returns The initial model state vector.
         **/
        virtual state_type initialize(const input_type& u, const output_type& z) const = 0;

        /**
         * Gets an empty state vector of the correct size for the current model.
         **/
        inline state_type getStateVector() const {
            return state_type(stateSize);
        }

        /**
         * Gets the size of the the state vector.
         */
        inline state_type::size_type getStateSize() const {
            return stateSize;
        }

        /**
         * Gets an empty input vector of the correct size for the current model.
         **/
        inline input_type getInputVector() const {
            return input_type(inputs.size());
        }

        /**
         * Gets the size of the the input vector.
         */
        inline input_type::size_type getInputSize() const {
            return inputs.size();
        }

        /**
         * Gets an empty output vector of the correct size for the current model.
         **/
        inline output_type getOutputVector() const {
            return output_type(outputs.size());
        }

        /**
         * Gets the size of the the output vector.
         */
        inline output_type::size_type getOutputSize() const {
            return outputs.size();
        }

        /**
         * Gets the time step used by the state equation if no timestep is
         * specified.
         **/
        inline double getDefaultTimeStep() const {
            return defaultTimeStep;
        }

        /**
         * Sets the time step used by the state equation if no timestep is
         * specified.
         **/
        inline void setDefaultTimeStep(const double value) {
            defaultTimeStep = value;
        }

        /**
         * Gets the names of the inputs.
         **/
        inline const std::vector<std::string>& getInputs() const {
            return inputs;
        }

        /**
         * Gets the names of the outputs.
         **/
        inline const std::vector<std::string>& getOutputs() const {
            return outputs;
        }

    private:
        double defaultTimeStep = 1.0;
        state_type::size_type stateSize;
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;
    };
}

#endif
