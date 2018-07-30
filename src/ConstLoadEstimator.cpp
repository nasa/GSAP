/**  Constant Load Estimator - Body
 *   @class     ConstLoadEstimator ConstLoadEstimator.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Constant Load Estimator Class- simulates constant loading
 *
 *   The purpose of this class is to handle the logic of estimating future loading in the case
 *   where future loading is constant.
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: February, 2018
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <random>

#include "ConstLoadEstimator.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    const std::string ConstLoadEstimator::LOADING_KEY = "LoadEstimator.loading";
    const std::string ConstLoadEstimator::STDDEV_KEY = "LoadEstimator.noise_sigma";

    const std::string DEBUG_TAG = "Const Load Estimator";

    static Log& log = Log::Instance();

    LoadEstimate generateNoise(LoadEstimate noiseStddev) {
        static std::random_device rDevice;
        static std::mt19937 generator(rDevice());
        LoadEstimate estimate(noiseStddev.size());
        for (size_t i = 0; i < noiseStddev.size(); i++) {
            std::normal_distribution<> noiseDistribution(0, noiseStddev[i]);
            estimate[i] = noiseDistribution(generator);
        }
        return estimate;
    }

    ConstLoadEstimator::ConstLoadEstimator(const GSAPConfigMap& configMap) {
        log.WriteLine(LOG_INFO, DEBUG_TAG, "Configuring");
        configMap.checkRequiredParams({LOADING_KEY});
        const std::vector<std::string>& profileStrs = configMap.at(LOADING_KEY);
        for (auto&& profileStr : profileStrs) {
            double profileItem = std::stod(profileStr);
            raw_profile.push_back(profileItem);
        }

        if (configMap.hasKey(STDDEV_KEY)) {
            log.WriteLine(LOG_INFO, DEBUG_TAG, "Inferred uncertainty type: gaussian");
            const std::vector<std::string>& stdStrs = configMap.at(STDDEV_KEY);
            for (auto&& stdStr : stdStrs) {
                stddev.push_back(std::stod(stdStr));
            }

            if (stddev.size() != raw_profile.size()) {
                log.FormatLine(LOG_ERROR,
                               DEBUG_TAG,
                               "Loading (%s) must be same size as stddev (%s), ignoring stddev and "
                               "treating as constant load");
            }
            else {
                uncertaintyMode = GAUSSIAN;
            }
        }

        log.WriteLine(LOG_TRACE, DEBUG_TAG, "Completed configuration");
    }

    void ConstLoadEstimator::setNSamples(const unsigned int nSamples) {
        if (GAUSSIAN == uncertaintyMode) {
            profiles.resize(nSamples);
            for (auto&& profileSample : profiles) {
                auto noiseSample = generateNoise(stddev);
                profileSample.resize(noiseSample.size());
                for (size_t i = 0; i < noiseSample.size(); i++) {
                    profileSample[i] = raw_profile[i] + noiseSample[i];
                }
            }
        }
    }

    ConstLoadEstimator::UType ConstLoadEstimator::getUncertaintyMode() {
        return uncertaintyMode;
    }

    LoadEstimate ConstLoadEstimator::estimateLoad(const double, const unsigned int sample) {
        LoadEstimate profileEstimate = raw_profile;

        switch (uncertaintyMode) {
        case NONE:
            break;
        case GAUSSIAN:
            log.WriteLine(LOG_TRACE, DEBUG_TAG, "Adding Noise");
            if (sample >= profiles.size()) {
                auto counter = profiles.size();
                profiles.resize(sample + 1);
                for (; counter < profiles.size(); counter++) {
                    auto noiseSample = generateNoise(stddev);
                    profiles[counter].resize(noiseSample.size());
                    for (size_t i = 0; i < noiseSample.size(); i++) {
                        profiles[counter][i] = profileEstimate[i] + noiseSample[i];
                    }
                }
            }
            profileEstimate = profiles[sample];
            break;
        default:
            break;
        }

        return profileEstimate;
    }
}
