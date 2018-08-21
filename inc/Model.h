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
         * @param z  The model output vector at the current time step.
         * @return   A stateSize x stateSize Jacobian Matrix.
         **/
        virtual const Matrix stateJacobianMatrix(const double t,
                                      const state_type& x,
                                      const input_type& u,
                                      const noise_type& n,
                                      double dt = 1.0,
                                      double epsilon = 0.1) {
            Matrix jacobian(stateSize,stateSize); //Checked Matrix.h, should be correct type
            for (int i=0; i<int(stateSize); i++) {
                //set up
                state_type x_minus = x;
                state_type x_plus = x;
                x_minus[i] -= epsilon;
                x_plus[i] += epsilon;
                
                //partial derivatives of all state eq'ns w.r.t. xi
                x_plus = stateEqn(t, x_plus, u, n, dt);
                x_minus = stateEqn(t, x_minus, u, n, dt);
                //state_type dx_i = subtractStates(x_plus,x_minus);
                //dx_i = divState(2*epsilon);
                
                //convert to Matrix form and differentiate
                Matrix x_plus_matrix = stateToMatrix(x_plus);
                Matrix x_minus_matrix = stateToMatrix(x_minus);
                
                Matrix dx_i = x_plus_matrix;
                dx_i -= x_minus_matrix;
                dx_i /= (2*epsilon);
                
                //set result as ith column
                jacobian.col(i, dx_i);
            }
            /* for i = 1:statesize, (forgive my syntax, I've been using python)
             state_type x_minus = x;
             state_type x_plus = x;
             x_minus[i] -= epsilon;
             x_plus[i] += epsilon;
             state_type dx_i = (eqn(t,x_minus,u,n,dt) - eqn(t,x_plus,u,n,dt))/(2*epsilon);
             j.col(i) = dx_i;
             */
            return jacobian;
            
        }
        
        /**
         * OVERLOADING (maybe) AIN'T GONNA WORK BECAUSE OUTPUTEQN DOESN'T USE DT
         * Calculate the Jacobian for the output equation. Useful for EKF.
         *
         * @param t  Time
         * @param x  The model state vector at the current time step.
         * @param u  The model input vector at the current time step.
         * @param n  The process noise vector.
         * @param z  The model output vector at the current time step.
         * @return   A stateSize x outputSize Jacobian.
         **/
        /*virtual const Matrix outputJacobian(const double t,
                                      const state_type& x,
                                      const input_type& u,
                                      const noise_type& n) {
            
        }*/
        
        
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

        /**
         * Element-wise summation of two state vectors
         **/
        inline state_type addStates(state_type& x1,state_type& x2) {
            state_type xSum = getStateVector();
            for (int i=0; i<int(stateSize); i++) {
                xSum[i] = x1[i]+x2[i];
            }
            return xSum;
        }
        
        /**
         * Element-wise subtraction of two state vectors (x1-x2)
         **/
        inline state_type subtractStates(state_type& x1,state_type& x2) {
            state_type xDiff = getStateVector();
            for (int i=0; i<int(stateSize); i++) {
                xDiff[i] = x1[i]-x2[i];
            }
            return xDiff;
        }
        
        /**
         * Divides every element by a constant
         **/
        inline state_type divState(const double div) {
            state_type xDiv = getStateVector();
            for (int i=0; i<int(stateSize); i++) {
                xDiv[i] /= div;
            }
            return xDiv;
        }
        
        /** 
         * Converts a state_type to a (stateSize x 1) Matrix
         **/
        inline Matrix stateToMatrix(state_type& x){
            Matrix m(stateSize,1);
            for (int i=0; i<int(stateSize); i++){
                m[i][0] = double(x[i]);
            }
            return m;
        }
        
        
    private:
        double defaultTimeStep = 1.0;
        //double defaultEpsilon = 0.01;
        state_type::size_type stateSize;
        std::vector<MessageId> inputs;
        std::vector<MessageId> outputs;
        
        
    };
}

#endif
