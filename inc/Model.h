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
        
        using DynamicArray::operator+=;
        
        using DynamicArray::operator-=;

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
     * @since 1.0
     **/
    class Model {
    public:
        using size_type = std::size_t;
        using input_type = InputVector;
        using output_type = OutputVector;
        using state_type = StateVector;
        using event_state_type = double;
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
              std::vector<MessageId> inputs,
              std::vector<MessageId> outputs)
            : stateSize(stateSize), inputs(std::move(inputs)), outputs(std::move(outputs)) {}

        /**
         * Default destructor. A virtual default destructor is necessary to
         * ensure that resources in classes inherting from this class are
         * cleaned up, even if the destructor is called through a pointer of the
         * base class type.
         **/
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
         * Calculate the Jacobian for the state equation. Useful for EKF.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @param u  The model input vector at the current time step.
         * @param n  The process noise vector.
         * @param dt  The size of the time step to calculate.
         * @param epsilon the step size to use in the definition of the derivative.
         * @return   A stateSize by stateSize Jacobian Matrix.
         **/
        virtual Matrix getStateJacobian(const double t,
                                        const state_type& x,
                                        const input_type& u,
                                        const noise_type& n,
                                        double dt = 1.0,
                                        double epsilon = 0.01) const {
            Matrix jacobian(stateSize,stateSize);
            for (size_type i = 0; i < stateSize; i++) {
                //perturb state vectors by epsilon/2
                state_type diff = getStateVector(0.0);
                diff[i] = epsilon/2.0;
                state_type x_plus = x + diff;
                state_type x_minus = x - diff;

                //state equations of perturbed xi's
                x_plus = stateEqn(t, x_plus, u, n, dt);
                x_minus = stateEqn(t, x_minus, u, n, dt);
                
                //convert to Matrix form and differentiate (w.r.t. xi)
                Matrix x_p(x_plus);
                Matrix x_m(x_minus);
                
                Matrix dx_i = x_p - x_m;
                dx_i /= (epsilon);
                
                //set result as ith column of Jacobian
                jacobian.col(i, dx_i);
            }
            return jacobian;
            
        }
        
        /**
         * Calculate the Jacobian for the output equation. Useful for EKF.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @param u  The model input vector at the current time step.
         * @param n  The process noise vector.
         * @param z  The model output vector at the current time step.
         * @param epsilon the step size to use in the definition of the derivative.
         * @return   An outputSize by stateSize Jacobian.
         **/
        virtual Matrix getOutputJacobian(const double t,
                                         const state_type& x,
                                         const input_type& u,
                                         const noise_type& n,
                                         double epsilon = 0.01) const {
            Matrix jacobian(outputSize,stateSize);
            for (size_type i = 0; i < stateSize; i++) {
                //perturb state vectors by epsilon/2
                state_type diff = getStateVector(0.0);
                diff[i] = epsilon/2.0;
                state_type x_plus = x + diff;
                state_type x_minus = x - diff;
                
                //output equations of perturbed xi's
                x_plus = outputEqn(t, x_plus, u, n);
                x_minus = outputEqn(t, x_minus, u, n);
                
                //convert to Matrix form and differentiate (w.r.t. xi)
                Matrix x_p(x_plus);
                Matrix x_m(x_minus);
                
                Matrix dx_i = x_p - x_m;
                dx_i /= (epsilon);
                
                //set result as ith column of Jacobian
                jacobian.col(i, dx_i);
            }
            return jacobian;
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
         * Gets a state vector of the correct size for the current model, initialized to @p value.
         **/
        inline state_type getStateVector(double value) const {
            state_type st(stateSize);
            for (size_type i = 0; i < stateSize; ++i) {
                st[i] = value;
            }
            return st;
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

        /**
         * Converts a state_type to a (stateSize x 1) Matrix
         * New constructor for Matrix in Matrix.h that takes a DynamicArray
         **/
        inline Matrix stateToMatrix(state_type& x){
            Matrix m(stateSize,1);
            for (size_type i=0; i < stateSize; i++){
                m[i][0] = double(x[i]);
            }
            return m;
        }
        
        
    private:
        double defaultTimeStep = 1.0;
        //double defaultEpsilon = 0.01;
        state_type::size_type stateSize;
        output_type::size_type outputSize;
        std::vector<MessageId> inputs;
        std::vector<MessageId> outputs;
        
        
    };
}

#endif
