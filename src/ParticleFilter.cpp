/**  ParticleFilter - Body
 *   @file       ParticleFilter.cpp
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

#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>

#include "GSAPConfigMap.h"
#include "Model.h"
#include "UData.h"

#include "Exceptions.h"
#include "ParticleFilter.h"

namespace PCOE {
    const double PI = std::atan(1) * 4.0; // Use this instead of M_PI- M_PI is not cross platform

    // Configuration Keys
    const std::string N_KEY = "Observer.N";
    const std::string PN_KEY = "Observer.processNoise";
    const std::string SN_KEY = "Observer.sensorNoise";
    const std::string NEFF_KEY = "Observer.MinNEffective";

    // Other string constants
    const std::string MODULE_NAME = "ParticleFilter";

    // Constructor
    ParticleFilter::ParticleFilter(Model* model,
                                   size_t N,
                                   std::vector<double>& processNoise,
                                   std::vector<double>& sensorNoise)
        : Observer() {
        // Set number of particles
        numParticles = N;

        // Set variance vectors
        processNoiseVariance = processNoise;
        sensorNoiseVariance = sensorNoise;
        setSensorCovariance();

        // Set model
        setModel(model);
    }

    void ParticleFilter::setSensorCovariance() {
        // Resize R and fill with 0s
        R = Matrix(sensorNoiseVariance.size(), sensorNoiseVariance.size(), 0);
        // Fill diagonals
        for (size_t i = 0; i < sensorNoiseVariance.size(); i++) {
            R[i][i] = sensorNoiseVariance[i];
        }
    }

    void ParticleFilter::checkNoiseVectors() {
        // Check that noise variance vectors are the right size
        if (processNoiseVariance.size() != pModel->getStateSize()) {
            log.WriteLine(LOG_ERROR,
                          MODULE_NAME,
                          "Process noise variance vector does not have the right number of values");
            throw std::range_error(
                "Process noise variance vector does not have the right number of values");
        }
        if (sensorNoiseVariance.size() != pModel->getOutputSize()) {
            log.WriteLine(LOG_ERROR,
                          MODULE_NAME,
                          "Sensor noise variance vector does not have the right number of values");
            throw std::range_error(
                "Sensor noise variance vector does not have the right number of values");
        }
    }

    // Set model
    void ParticleFilter::setModel(Model* model) {
        // Set the model pointer
        pModel = model;

        // Set up variables that are dependent on the model
        m_xEstimated = pModel->getStateVector();
        m_uOld = pModel->getInputVector();
        m_zEstimated = pModel->getOutputVector();

        // Set up sigma point matrices and weights for x
        particles.X.resize(pModel->getStateSize(), numParticles);
        particles.Z.resize(pModel->getOutputSize(), numParticles);
        particles.w.resize(numParticles);

        // Check noise vectors
        checkNoiseVectors();
    }

    // GSAPConfigMap-based Constructor
    ParticleFilter::ParticleFilter(GSAPConfigMap& configMap) : Observer() {
        // Check for required parameters: Q, R
        configMap.checkRequiredParams({N_KEY, PN_KEY, SN_KEY});

        // Set N
        numParticles = static_cast<std::size_t>(std::stod(configMap.at(N_KEY)[0]));
        setMinNEffective(numParticles / 3);

        // Set process noise variance
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Setting process noise variance vector");
        std::vector<std::string> PNValues = configMap.at(PN_KEY);
        std::size_t dimension = PNValues.size();
        processNoiseVariance.resize(dimension);
        for (size_t i = 0; i < dimension; i++) {
            processNoiseVariance[i] = std::stod(PNValues[i]);
        }

        // Set sensor noise variance
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Setting sensor noise variance vector");
        std::vector<std::string> SNValues = configMap.at(SN_KEY);
        dimension = SNValues.size();
        sensorNoiseVariance.resize(dimension);
        for (size_t i = 0; i < dimension; i++) {
            sensorNoiseVariance[i] = std::stod(SNValues[i]);
        }
        setSensorCovariance();

        // Set minNEff (optional)
        if (configMap.includes(NEFF_KEY)) {
            setMinNEffective(static_cast<std::size_t>(std::stod(configMap.at(NEFF_KEY)[0])));
        }

        log.WriteLine(LOG_INFO, MODULE_NAME, "Created particle filter");
    }

    // Set minNEffective
    void ParticleFilter::setMinNEffective(size_t N) {
        minNEffective = N;
    }

    // Initialize function (required by Observer interface)
    void ParticleFilter::initialize(const double t0,
                                    const Model::state_type& x0,
                                    const Model::input_type& u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");

        // Check that model has been set
        if (pModel == NULL) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Particle filter does not have a model!");
            throw ConfigurationError("Particle filter does not have a model!");
        }

        // Set up random number generator
        std::random_device rDevice;
        generator.seed(rDevice());

        // Initialize time, state, inputs
        m_t = t0;
        m_xEstimated = x0;
        m_uOld = u0;

        // Compute corresponding output estimate
        std::vector<double> zeroNoiseZ(pModel->getOutputSize(), 0);
        m_zEstimated = pModel->outputEqn(m_t, m_xEstimated, m_uOld, zeroNoiseZ);

        // Initialize particles
        for (size_t p = 0; p < numParticles; p++) {
            // Set x to x0
            particles.X.col(p, x0.vec());
            // Set z based on x
            std::vector<double> zeroNoise(pModel->getOutputSize(), 0);
            auto z0 = pModel->outputEqn(t0, x0, u0, zeroNoise);
            particles.Z.col(p, z0.vec());
            // Set w all equal, since we aren't adding any noise
            particles.w[p] = 1.0 / numParticles;
        }

        // Set initialized flag
        m_initialized = true;
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }

    // Normalize particle weighs
    void ParticleFilter::normalize() {
        // Compute sum of weights
        double sumWeights = std::accumulate(particles.w.begin(), particles.w.end(), 0.0);
        for (size_t p = 0; p < particles.w.size(); p++) {
            particles.w[p] /= sumWeights;
        }
    }

    // Resample particles
    void ParticleFilter::resample() {
        // Compute effective sample size: 1/sum(w^2)
        double nEffective = 0;
        for (size_t p = 0; p < particles.w.size(); p++) {
            nEffective += particles.w[p] * particles.w[p];
        }
        nEffective = 1.0 / nEffective;

        // Resample if nEff below threshold (systematic resampling)
        if (nEffective < minNEffective) {
            systematicResample();
        }
    }

    // Systematic resampling algorithm
    void ParticleFilter::systematicResample() {
        // Resamples the particles to be distributed around the higher-weight particles,
        // to increase the effective number of particles and reduce degeneracy.
        // Note that particle weights must be normalized before calling this function.

        // Copy particles
        Particles newParticles = particles;

        // Construct CDF
        std::vector<double> cumsum(particles.w.size());
        std::partial_sum(particles.w.begin(), particles.w.end(), cumsum.begin());
        size_t i = 1;

        // Draw starting point from U[0,1/numParticles]
        std::uniform_real_distribution<> distribution(0, 1.0 / numParticles);
        double u1 = distribution(generator);

        double u;
        for (size_t p = 0; p < numParticles; p++) {
            // Move along CDF
            u = u1 + (p - 1.0) / numParticles;
            while (u > cumsum[i]) {
                i += 1;
            }
            // Reassign particle
            newParticles.X.col(p, particles.X.col(i));
            newParticles.Z.col(p, particles.Z.col(i));
        }

        // Reassign weights so that all equal
        for (size_t p = 0; p < numParticles; p++) {
            newParticles.w[p] = 1.0 / numParticles;
        }

        // Copy over resampled particles
        particles = newParticles;
    }

    // Generate process noise
    void ParticleFilter::generateProcessNoise(std::vector<double>& noise) {
        // Ensure noise is the right size
        noise.resize(pModel->getStateSize());
        // Go through process noise variance vector and generate values
        for (size_t n = 0; n < processNoiseVariance.size(); n++) {
            std::normal_distribution<> distribution(0, sqrt(processNoiseVariance[n]));
            noise[n] = distribution(generator);
        }
    }

    // Compute likelihood
    double ParticleFilter::likelihood(const Model::output_type& zActual,
                                      const Model::output_type& zPredicted) {
        // Compute innovation
        Matrix zA(zActual.size(), 1);
        Matrix zP(zActual.size(), 1);
        zA.col(0, zActual.vec());
        zP.col(0, zPredicted.vec());
        Matrix I = zA - zP;
        Matrix expArgument = -0.5 * I.transpose() * R.inverse() * I;
        double lh = 1.0 / pow(2.0 * PI, zActual.size() / 2.0) * 1.0 / sqrt(R.determinant()) *
                    exp(expArgument[0][0]);
        return lh;
    }

    Model::state_type ParticleFilter::weightedMean(const Matrix& M,
                                                   const std::vector<double>& weights) {
        // Assumption is that samples are the columns of M
        // result must already be the correct size
        // Convert weights to matrix
        Matrix W = Matrix(weights.size(), 1);
        W.col(0, weights);
        // Compute result using matrix multiplication
        Matrix wMean = M * W;
        // Place in the result vector
        auto result = pModel->getStateVector();
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = wMean[0][i];
        }
        return result;
    }

    // Step function (required by Observer interface)
    void ParticleFilter::step(const double newT,
                              const Model::input_type& u,
                              const Model::output_type& z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");

        if (!isInitialized()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Called step before initialized");
            throw std::domain_error("ParticleFilter::step not initialized");
        }

        // Update time
        double dt = newT - m_t;
        m_t = newT;
        if (dt <= 0) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "dt is less than or equal to zero");
            throw std::domain_error("ParticleFilter::step dt is 0");
        }

        // For each particle
        for (size_t p = 0; p < numParticles; p++) {
            // Generate process noise
            std::vector<double> noise(pModel->getStateSize());
            generateProcessNoise(noise);
            std::vector<double> zeroNoise(pModel->getOutputSize());

            // Generate new particle
            auto xNew = Model::state_type(static_cast<std::vector<double>>(particles.X.col(p)));
            xNew = pModel->stateEqn(newT, xNew, m_uOld, noise, dt);
            particles.X.col(p, xNew.vec());
            auto zNew = pModel->outputEqn(newT, xNew, u, zeroNoise);
            particles.Z.col(p, zNew.vec());

            // Set weight
            particles.w[p] = likelihood(z, zNew);
            particles.w[p] = likelihood(z, zNew);
        }

        // Normalize weights
        normalize();

        // Resample particles
        resample();

        // Compute weighted means
        m_xEstimated = weightedMean(particles.X, particles.w);

        // Update uOld
        m_uOld = u;
    }

    std::vector<UData> ParticleFilter::getStateEstimate() const {
        std::vector<UData> state(pModel->getStateSize());
        for (unsigned int i = 0; i < pModel->getStateSize(); i++) {
            state[i].uncertainty(UType::WeightedSamples);
            state[i].npoints(numParticles);
            for (size_t p = 0; p < numParticles; p++) {
                state[i][SAMPLE(p)] = particles.X[i][p];
                state[i][WEIGHT(p)] = particles.w[p];
            }
        }
        return state;
    }

    size_t ParticleFilter::getNumParticles() const {
        return numParticles;
    }

    size_t ParticleFilter::getMinNEffective() const {
        return minNEffective;
    }

    const std::vector<double>& ParticleFilter::getProcessNoiseVariance() const {
        return processNoiseVariance;
    }

    const std::vector<double>& ParticleFilter::getSensorNoiseVariance() const {
        return sensorNoiseVariance;
    }

    // Get state mean
    const Model::state_type& ParticleFilter::getStateMean() const {
        return m_xEstimated;
    }

    // Get output mean
    const Model::output_type& ParticleFilter::getOutputMean() const {
        return m_zEstimated;
    }
}
