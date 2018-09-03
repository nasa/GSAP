// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_ParticleFilter_H
#define PCOE_ParticleFilter_H

#include <random>
#include <vector>

#include "Matrix.h"
#include "Observers/Observer.h"

namespace PCOE {
    class ConfigMap;

    struct Particles {
        Matrix X; // state matrix, numStates x particleCount
        Matrix Z; // output matrix, numOutputs x particleCount
        std::vector<double> w; // weights
    };

    /**
     * Implements particle filter state estimation algorithm for non-linear
     * models.
     *
     * @author Mattew Daigle
     * @author Jason Watkins
     * @since 1.1
     **/
    class ParticleFilter final : public Observer {
    private:
        /**
         * Constructs a new @{code ParticleFilter} instance and initializes the
         * model. This constructor is only intended to be used by other
         * constructors in particle filter to set up model-related parameters.
         **/
        explicit ParticleFilter(const Model& m);

    public:
        /**
         * Initializes a new @{code ParticleFilter} instance with the specified
         * model and parameters.
         *
         * @param m             The model being observed. The @{code
         *                      ParticleFilter} does not take ownership of the
         *                      model pointer.
         * @param particleCount The number of particles to use.
         * @param processNoise  The process noise.
         * @param sensorNoise   The sensor noise.
         **/
        ParticleFilter(const Model& m,
                       size_t particleCount,
                       std::vector<double>& processNoise,
                       std::vector<double>& sensorNoise);

        /**
         * Initializes a new @{code ParticleFilter} instance with the specified
         * model and with parameters read from the given configuration.
         *
         * @param m      The model being observed. The @{code ParticleFilter}
         *               does not take ownership of the model pointer.
         * @param config A config from which particle count, process noise, and
         *               sensor noise will be read.
         **/
        ParticleFilter(const Model& m, const ConfigMap& config);

        /**
         * Initializes the particles and sets the initial state estimate.
         *
         * @param t0 Initial time
         * @param x0 Initial model state
         * @param u0 Initial model input
         **/
        void initialize(const double t0,
                        const Model::state_type& x0,
                        const Model::input_type& u0) override;

        /**
         * Performs a single state estimation with the given model inputs and
         * outputs.
         *
         * @param t The time at which to make a prediction.
         * @param u The model input vector at time @{code t}.
         * @param z The model output vector at time @{code t}.
         **/
        void step(double t, const Model::input_type& u, const Model::output_type& z) override;

        /**
         * Sets the miniumn effective number of particles.
         **/
        inline void setMinEffective(std::size_t value) {
            minEffective = value;
        }

        /**
         * Returns the current mean state estimate of the observer.
         *
         * @return The last calculated state estimate.
         **/
        inline const Model::state_type& getStateMean() const override {
            return xEstimated;
        }

        /**
         * Returns the current state estimate of the observer, including
         * uncertainty.
         *
         * @return The last calculated state estimate calcualted by the
         *         observer.
         **/
        std::vector<UData> getStateEstimate() const override;

        /**
         * Returns the current mean output estimate of the observer.
         *
         * @return The last output estimate calcualted by the observer..
         **/
        inline const Model::output_type& getOutputMean() const override {
            return zEstimated;
        }

        /**
         * Gets the number of particles used by the particle filter.
         **/
        inline std::size_t getParticleCount() const {
            return particleCount;
        }

        /**
         * Gets the minium effective number of particles.
         **/
        inline std::size_t getMinEffective() const {
            return minEffective;
        }

        const std::vector<double>& getProcessNoiseVariance() const {
            return processNoiseVariance;
        }

        const std::vector<double>& getSensorNoiseVariance() const {
            return sensorNoiseVariance;
        }

    private:
        std::size_t particleCount;
        std::size_t minEffective;
        Particles particles;
        std::vector<double> processNoiseVariance;
        std::vector<double> sensorNoiseVariance;
        Matrix R;
        Model::state_type xEstimated;
        Model::output_type zEstimated;
        std::mt19937 rng;

        void normalize();

        void resample();

        void systematicResample();

        void generateProcessNoise(std::vector<double>& noise);

        double likelihood(const Model::output_type& zActual, const Model::output_type& zPredicted);

        void setSensorCovariance();

        Model::state_type weightedMean(const Matrix& M, const std::vector<double>& weights);
    };
}

#endif
