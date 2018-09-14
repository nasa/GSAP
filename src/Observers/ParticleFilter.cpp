// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>

#include "ConfigMap.h"
#include "Observers/ParticleFilter.h"
#include "UData.h"

namespace PCOE {
    const static Log& log = Log::Instance();

    // cmath may define an PI constant, but it is not part of the standard.
    // This gets a reasonably acurate value for pi in a standards-compliant way.
    const double PI = std::acos(-1);

    // Configuration Keys
    const std::string N_KEY = "Observer.ParticleCount";
    const std::string PN_KEY = "Observer.ProcessNoise";
    const std::string SN_KEY = "Observer.SensorNoise";
    const std::string NEFF_KEY = "Observer.MinEffective";

    // Other string constants
    const std::string MODULE_NAME = "OBS-PF";

    ParticleFilter::ParticleFilter(const SystemModel& m) : Observer(m) {
        uPrev = model.getInputVector();
    }

    ParticleFilter::ParticleFilter(const SystemModel& m,
                                   std::size_t count,
                                   std::vector<double>& processNoise,
                                   std::vector<double>& sensorNoise)
        : ParticleFilter(m) {
        Expect(processNoise.size() == model.getStateSize(),
               "Process noise variance vector size does not match model state vector size");
        Expect(sensorNoise.size() == model.getOutputSize(),
               "Sensor noise variance vector size does not match model output vector size");

        particleCount = count;

        // Set variance vectors
        processNoiseVariance = processNoise;
        sensorNoiseVariance = sensorNoise;
        setSensorCovariance();

        // Set up sigma point matrices and weights for x
        particles.X.resize(model.getStateSize(), particleCount);
        particles.Z.resize(model.getOutputSize(), particleCount);
        particles.w.resize(particleCount);
    }

    ParticleFilter::ParticleFilter(const SystemModel& m, const ConfigMap& config) : ParticleFilter(m) {
        requireKeys(config, {N_KEY, PN_KEY, SN_KEY});

        // Set N
        particleCount = static_cast<std::size_t>(config.getUInt64(N_KEY));
        setMinEffective(particleCount / 3);

        particles.X.resize(model.getStateSize(), particleCount);
        particles.Z.resize(model.getOutputSize(), particleCount);
        particles.w.resize(particleCount);

        // Set process noise variance
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Setting process noise variance vector");
        std::vector<std::string> PNValues = config.getVector(PN_KEY);
        std::size_t dimension = PNValues.size();
        processNoiseVariance.resize(dimension);
        for (size_t i = 0; i < dimension; i++) {
            processNoiseVariance[i] = std::stod(PNValues[i]);
        }

        // Set sensor noise variance
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Setting sensor noise variance vector");
        std::vector<std::string> SNValues = config.getVector(SN_KEY);
        dimension = SNValues.size();
        sensorNoiseVariance.resize(dimension);
        for (size_t i = 0; i < dimension; i++) {
            sensorNoiseVariance[i] = std::stod(SNValues[i]);
        }
        setSensorCovariance();

        // Set minNEff (optional)
        if (config.hasKey(NEFF_KEY)) {
            setMinEffective(static_cast<std::size_t>(config.getDouble(NEFF_KEY)));
        }

        Ensure(processNoiseVariance.size() == model.getStateSize(),
               "Process noise variance vector size does not match model state vector size");
        Ensure(sensorNoiseVariance.size() == model.getOutputSize(),
               "Sensor noise variance vector size does not match model output vector size");
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Created particle filter");
    }

    void ParticleFilter::initialize(const double t0,
                                    const SystemModel::state_type& x0,
                                    const SystemModel::input_type& u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");
        // TODO (JW): This contract is now stated in the constructor, so it
        //            should be guaranteed here. Consider removing.
        Expect(particles.X.rows() == model.getStateSize(),
               "particles.X row count does not match model state size");
        Expect(particles.X.cols() == particleCount,
               "particles.X col count does not match particle count");

        // Set up random number generator
        std::random_device rDevice;
        rng.seed(rDevice());

        // Initialize time, state, inputs
        lastTime = t0;
        uPrev = u0;

        // Compute corresponding output estimate
        std::vector<double> zeroNoiseZ(model.getOutputSize(), 0);

        // Initialize particles
        for (size_t p = 0; p < particleCount; p++) {
            particles.X.col(p, x0.vec());
            SystemModel::output_type z0 = model.getOutputVector();
            std::vector<double> zeroNoise(model.getOutputSize(), 0);
            z0 = model.outputEqn(t0, x0, zeroNoise);
            particles.Z.col(p, z0.vec());
            // Set w all equal, since we aren't adding any noise
            particles.w[p] = 1.0 / particleCount;
        }

        initialized = true;
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }

    void ParticleFilter::step(const double newT,
                              const SystemModel::input_type& u,
                              const SystemModel::output_type& z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");
        Expect(initialized, "Step before initialization");
        Expect(newT - lastTime > 0, "Time has not advanced");

        double dt = newT - lastTime;
        lastTime = newT;

        std::vector<double> noise(model.getStateSize());
        std::vector<double> zeroNoise(model.getOutputSize());
        for (std::size_t p = 0; p < particleCount; p++) {
            generateProcessNoise(noise);

            // Generate new particle
            auto xNew = SystemModel::state_type(static_cast<std::vector<double>>(particles.X.col(p)));
            xNew = model.stateEqn(newT, xNew, uPrev, noise, dt);
            particles.X.col(p, xNew.vec());
            auto zNew = model.outputEqn(newT, xNew, zeroNoise);
            particles.Z.col(p, zNew.vec());

            // Set weight
            particles.w[p] = likelihood(z, zNew);
            particles.w[p] = likelihood(z, zNew);
        }

        normalize();
        resample();
        uPrev = u;
    }

    std::vector<UData> ParticleFilter::getStateEstimate() const {
        std::vector<UData> state(model.getStateSize());
        for (unsigned int i = 0; i < model.getStateSize(); i++) {
            state[i].uncertainty(UType::WeightedSamples);
            state[i].npoints(particleCount);
            for (std::size_t p = 0; p < particleCount; p++) {
                state[i][SAMPLE(p)] = particles.X[i][p];
                state[i][WEIGHT(p)] = particles.w[p];
            }
        }
        return state;
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

        if (nEffective < minEffective) {
            systematicResample();
        }
    }

    void ParticleFilter::systematicResample() {
        // Resamples the particles to be distributed around the higher-weight particles,
        // to increase the effective number of particles and reduce degeneracy.
        // Note that particle weights must be normalized before calling this function.

        // Copy particles
        Particles newParticles = particles;

        // Construct CDF
        std::vector<double> cumSum(particles.w.size());
        std::partial_sum(particles.w.begin(), particles.w.end(), cumSum.begin());
        size_t i = 1;

        // Draw starting point from U[0,1/particleCount]
        std::uniform_real_distribution<> distribution(0, 1.0 / particleCount);
        double u1 = distribution(rng);

        double u;
        for (size_t p = 0; p < particleCount; p++) {
            // Move along CDF
            u = u1 + (p - 1.0) / particleCount;
            while (u > cumSum[i]) {
                i += 1;
            }
            // Reassign particle
            newParticles.X.col(p, particles.X.col(i));
            newParticles.Z.col(p, particles.Z.col(i));
        }

        // Reassign weights so that all equal
        for (size_t p = 0; p < particleCount; p++) {
            newParticles.w[p] = 1.0 / particleCount;
        }

        // Copy over resampled particles
        particles = newParticles;
    }

    void ParticleFilter::generateProcessNoise(std::vector<double>& noise) {
        // TODO (JW): The first contract is the one originally checked, but the
        //            second is the one actually required by the for loop. They
        //            should be the same anyway. Consider removing check on
        //            model state size.
        Expect(noise.size() == model.getStateSize(), "Noise size does not match model state size");
        Expect(noise.size() == processNoiseVariance.size(),
               "Noise size does not match process noise variance size");
        for (size_t n = 0; n < processNoiseVariance.size(); n++) {
            std::normal_distribution<> distribution(0, sqrt(processNoiseVariance[n]));
            noise[n] = distribution(rng);
        }
    }

    double ParticleFilter::likelihood(const SystemModel::output_type& zActual,
                                      const SystemModel::output_type& zPredicted) {
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

    void ParticleFilter::setSensorCovariance() {
        // Resize R and fill with 0s
        R = Matrix(sensorNoiseVariance.size(), sensorNoiseVariance.size(), 0);
        // Fill diagonals
        for (std::size_t i = 0; i < sensorNoiseVariance.size(); i++) {
            R[i][i] = sensorNoiseVariance[i];
        }
    }

    SystemModel::state_type ParticleFilter::weightedMean(const Matrix& M,
                                                   const std::vector<double>& weights) {
        Expect(M.rows() == model.getStateSize(), "M rows does not match model state size");
        Expect(M.cols() == weights.size(), "M cols does not match weights size");

        Matrix W = Matrix(weights.size(), 1);
        W.col(0, weights);

        Matrix wMean = M * W;

        auto result = model.getStateVector();
        for (std::size_t i = 0; i < result.size(); i++) {
            result[i] = wMean[0][i];
        }
        return result;
    }
}
