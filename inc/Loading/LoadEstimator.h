// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_LOADESTIMATOR_H
#define PCOE_LOADESTIMATOR_H

#include <stdexcept>
#include <vector>

namespace PCOE {
    const std::string LOAD_EST_SAMPLES_KEY = "LoadEstimator.SampleCount";

    /**
     * Abstract base class that defines the interface for load estimators.
     *
     * @author Chris Teubert
     * @author Jason Watkins
     * @since 1.1
     **/
    class LoadEstimator {
    public:
        using LoadEstimate = std::vector<double>;
        using LoadMeasurement = std::vector<double>;

        virtual ~LoadEstimator() = default;

        /**
         * When overriden in a derived class, gets a value indicating whether
         * calls to {@code addLoad} are allowed.
         *
         * @see addLoad
         **/
        virtual inline bool canAddLoad() {
            return false;
        }

        /**
         * When overriden in a derived class, uses measured load in an
         * implementation-specific way.
         *
         * @param load An estimate of the system's current loading.
         * @exception std::runtime_error If the load estimator does not support
         *                               historical loading.
         *
         * @see canAddLoad
         **/
        virtual inline void addLoad(const LoadMeasurement& load) {
            static_cast<void>(load);
            throw std::runtime_error("Not supported");
        };

        /**
         * When overriden in a derived class, estimates the load at a given time
         * and sample.
         *
         * @param t      The timestamp to estimate load for.
         * @return       The current estimated load.
         **/
        virtual LoadEstimate estimateLoad(const double t) = 0;
    };
}
#endif
