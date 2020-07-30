// Copyright (c) 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <iostream>

#include "ConfigMap.h"
#include "Exceptions.h"
#include "Loading/ProfileLoadEstimator.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    Log& l = Log::Instance();
    const std::string LOG_TAG = "ProfileLoadEstimator";
    const std::string LOADING_LEN_KEY = "LoadEstimator.LoadingProfileLength";
    const std::string ELEMENT_BASE_KEY = "LoadEstimator.Element[";

    ProfileLoadEstimator::ProfileLoadEstimator(const ConfigMap& config) {
        using namespace std::chrono;

        requireKeys(config, {LOADING_LEN_KEY});

        uint32_t nElements = config.getUInt32(LOADING_LEN_KEY);

        profile.resize(nElements);
        for (uint32_t i = 0; i < nElements; i++) {
            const std::string TAG_BASE = ELEMENT_BASE_KEY + std::to_string(i) + "]";
            requireKeys(config, {TAG_BASE + ".Duration", TAG_BASE + ".Loads"});
            double duration_s = config.getDouble(TAG_BASE + ".Duration");
            LoadEstimate inputs = config.getDoubleVector(TAG_BASE + ".Loads");
            profile[i] = ProfileElement(duration_s, inputs);
        }
    }

    LoadEstimator::LoadEstimate ProfileLoadEstimator::estimateLoad(const double t_s) {
        const static double startT_s = t_s;
        double relative_t_s = t_s-startT_s;

        for (ProfileElement& step : profile) {
            if (step.first > relative_t_s) {
                return step.second;
            }
            else {
                relative_t_s -= step.first;
            }
        }
        l.WriteLine(LOG_WARN, LOG_TAG, "Reached end of profile, returning empty load estimate.");
        throw OutOfRangeError("Reached end of profile, returning empty load estimate.");
    }
}
