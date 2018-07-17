/**  ParticleFilter - Header
 *   @file       ParticleFilter.h
 *   @ingroup    GPIC++
 *   @defgroup   Observer     Observer
 *
 *   @brief      Particle filter class. Implements Particle Filter state estimation algorithm for
 * nonlineary models. Uses the Model class.
 *
 *   @author     Matthew Daigle
 *   @version    0.1.0
 *
 *   @pre        N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: February 8, 2017
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_ParticleFilter_H
#define PCOE_ParticleFilter_H

#include <random>
#include <vector>

#include "Matrix.h"
#include "Observer.h"

namespace PCOE {
    // Including class prototype to avoid including header
    class GSAPConfigMap;

    struct Particles {
        Matrix X; // state matrix, numStates x N
        Matrix Z; // output matrix, numOutputs x N
        std::vector<double> w; // weights
    };

    class ParticleFilter final : public Observer {
    private:
        size_t numParticles;
        size_t minNEffective;
        Particles particles;
        std::vector<double> processNoiseVariance;
        std::vector<double> sensorNoiseVariance;
        Matrix R;
        Model::state_type m_xEstimated;
        Model::output_type m_zEstimated;
        std::mt19937 generator;

        /** @brief Check that noise vector sizes and model are consistent
         **/
        void checkNoiseVectors();

        /** @brief Normalize particle weights
         **/
        void normalize();

        /** @brief Resample particles
         **/
        void resample();

        /** @brief Systematic resampling algorithm
         **/
        void systematicResample();

        /** @brief Generate process noise
         *  @param noise process noise result
         **/
        void generateProcessNoise(std::vector<double>& noise);

        /** @brief Compute likeihood of output sample given measurement
         **/
        double likelihood(const Model::output_type& zActual, const Model::output_type& zPredicted);

        /** @brief Set up sensor noise covariance matrix based on variance vector
         **/
        void setSensorCovariance();

        /** @brief Compute weighted mean
         **/
        Model::state_type weightedMean(const Matrix& M, const std::vector<double>& weights);

    public:
        /** @brief Constructor
         *   @param m Model pointer
         *   @param processNoise Process noise variance vector
         *   @param sensorNoise Sensor noise variance vector
         **/
        ParticleFilter(Model* m,
                       size_t N,
                       std::vector<double>& processNoise,
                       std::vector<double>& sensorNoise);

        /** @brief Constructor given a ConfigMap
         *   @param configMap configuration map specifying parameters (Q, R)
         **/
        explicit ParticleFilter(GSAPConfigMap& configMap);

        /** @brief Set model pointer
         *   @param model given model pointer
         **/
        void setModel(Model* model);

        /** @brief Initialize PF
         *   @param t0 Initial time
         *   @param x0 Initial state vector
         *   @param u0 Initial input vector
         **/
        void initialize(const double t0, const Model::state_type& x0, const Model::input_type& u0);

        /** @brief Estimation step. Updates xEstimated, zEsitmated, P, and sigmaX.
         *   @param newT Time value at new step
         *   @param u Input vector at current time
         *   @param z Output vector at current time
         **/
        void step(const double newT, const Model::input_type& u, const Model::output_type& z);

        /** @brief Set minimum number of effective particles
         *   @param N minimum number of effective particles
         **/
        void setMinNEffective(size_t N);

        // Accessors
        const Model::state_type& getStateMean() const override;
        const Model::output_type& getOutputMean() const override;
        std::vector<UData> getStateEstimate() const;

        size_t getNumParticles() const;

        size_t getMinNEffective() const;

        const std::vector<double>& getProcessNoiseVariance() const;

        const std::vector<double>& getSensorNoiseVariance() const;
    };
}

#endif // PCOE_ParticleFilter_H
