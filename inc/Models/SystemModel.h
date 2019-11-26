// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MODEL_H
#define PCOE_MODEL_H

#include <string>
#include <vector>

#include "DynamicArray.h"
#include "Matrix.h"
#include "Messages/MessageId.h"

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

    class ObservablesVector final : public DynamicArray<double> {
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
     * @author Julian Vu
     * @authon Chris Teubert
     * @since 1.0
     **/
    class SystemModel {
    public:
        using size_type = std::size_t;
        using input_type = InputVector;
        using output_type = OutputVector;
        using state_type = StateVector;
        using observables_type = ObservablesVector;
        using event_state_type = StateVector;
        using noise_type = std::vector<double>;

        /**
         * Initializes the system model with the given parameters.
         *
         * @param stateSize The number of values in the state vector.
         * @param inputs    The names of the model inputs. The size of this
         *                  parameter also determines the number of values in
         *                  the input vector.
         * @param outputs   The names of the model outputs. The size of this
         *                  parameter also determines the number of values in
         *                  the output vector.
         **/
        SystemModel(state_type::size_type stateSize,
                    std::vector<MessageId> inputs,
                    std::vector<MessageId> outputs,
                    std::vector<std::string> observables,
                    std::vector<MessageId> events)
            : stateSize(stateSize),
              inputs(std::move(inputs)),
              outputs(std::move(outputs)),
              observables(std::move(observables)),
              events(std::move(events)) {}

        /**
         * Default destructor. A virtual default destructor is necessary to
         * ensure that resources in classes inherting from this class are
         * cleaned up, even if the destructor is called through a pointer of the
         * base class type.
         **/
        virtual ~SystemModel() = default;

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
        inline state_type stateEqn(const double t,
                                    const state_type& x,
                                    const input_type& u,
                                    const noise_type& n,
                                    const double dt) const  {
            state_type x_new = stateEqn(t, x, u, dt);
            
            // Add process noise
            for (size_type it = 0; it < getStateSize(); it++) {
                x_new[it] += dt * n[it];
            }
            return x_new;
        }
        
        /**
         * Calculate the model state using the given sampling time without noise
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @param u  The model input vector at the current time step.
         * @param dt The size of the time step to calculate.
         * @return   The model state vector at the next time step.
         **/
        virtual state_type stateEqn(const double t,
                                   const state_type& x,
                                   const input_type& u,
                                   const double dt) const  = 0;

        /**
         * Calculate the model output.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @param n  The process noise vector.
         * @return   The model output vector at the next time step.
         **/
        inline output_type outputEqn(const double t,
                                      const state_type& x,
                                     const noise_type& n) const {
            output_type z = outputEqn(t, x);
            
            // Add sensor noise
            for (size_type it = 0; it < getOutputSize(); it++) {
                z[it] += n[it];
            }
            return z;
        };
        
        /**
         * Calculate the model output without noise
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @return   The model output vector at the next time step.
         **/
        virtual output_type outputEqn(const double t,
                                      const state_type& x) const = 0;

        /**
         * Calculate event state.
         *
         * @param x  The model state vector at the current time step.
         * @return   The
         */
        virtual event_state_type eventStateEqn(const state_type& x) const {
            (void)x;
            return event_state_type();
        }

        /** Calculate observables of the model. Observables are those
         * that are not measured, but are interested in being predicted for
         * prognostics.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @return   The model output vector at the next time step.
         **/
        virtual observables_type observablesEqn(double t, const state_type& x) const {
            (void)t;
            (void)x;
            return getObservablesVector();
        }

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
        inline const std::vector<MessageId>& getInputs() const {
            return inputs;
        }

        /**
         * Gets the names of the outputs.
         **/
        inline const std::vector<MessageId>& getOutputs() const {
            return outputs;
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
        double defaultTimeStep = 1.0;
        state_type::size_type stateSize;
        std::vector<MessageId> inputs;
        std::vector<MessageId> outputs;
        std::vector<std::string> observables;
        std::vector<MessageId> events;
    };
}

#endif
