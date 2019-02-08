// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <memory>
#include <sstream>

#include "ConfigMap.h"
#include "Contracts.h"
#include "Loading/LoadEstimatorFactory.h"
#include "ModelBasedAsyncPrognoserBuilder.h"
#include "Trajectory/AsyncTrajectoryService.h"

static std::vector<std::string> split(const std::string& value) {
    std::istringstream ss(value);
    std::string token;
    std::vector<std::string> result;

    while (std::getline(ss, token, ',')) {
        result.push_back(token);
    }

    return result;
}

namespace PCOE {
    const static Log& log = Log::Instance();

    const std::string AsyncPrognoserBuilder::LOAD_ESTIMATOR_KEY = "LoadEstimator";
    const std::string AsyncPrognoserBuilder::DEFAULT_LOAD_ESTIMATOR = "MovingAverage";

    const std::string MODULE_NAME = "EDPrognoserBuilder";

    AsyncPrognoserBuilder::AsyncPrognoserBuilder(ConfigMap config) : config(std::move(config)) {}

    void AsyncPrognoserBuilder::setLoadEstimatorName(const std::string& value) {
        lock_guard guard(m);
        Expect(value.length() > 0, "Load estimator name length");
        config.set(LOAD_ESTIMATOR_KEY, value);
    }

    void AsyncPrognoserBuilder::setConfigParam(const std::string& key, const std::string& value) {
        lock_guard guard(m);
        Expect(key.length() > 0, "key size");
        Expect(value.length() > 0, "value size");
        log.FormatLine(LOG_DEBUG,
                       MODULE_NAME,
                       "Setting config %s = %s",
                       key.c_str(),
                       value.c_str());
        config.set(key, split(value));
    }

    void AsyncPrognoserBuilder::setConfigParam(const std::string& key,
                                               const std::vector<std::string>& value) {
        lock_guard guard(m);
        Expect(key.length() > 0, "key size");
        config.set(key, value);
    }

    AsyncPrognoser AsyncPrognoserBuilder::build(MessageBus& bus,
                                                const std::string& sensorSource,
                                                const std::string& trajectorySource) {
        lock_guard guard(m);
        AsyncPrognoser container(bus);
        container.addEventListener(
            new AsyncTrajectoryService(bus,
                                       std::unique_ptr<TrajectoryService>(new TrajectoryService()),
                                       trajectorySource));

        if (config.hasKey(LOAD_ESTIMATOR_KEY)) {
            log.FormatLine(LOG_DEBUG,
                           MODULE_NAME,
                           "Building load estimator %s",
                           config.getString(LOAD_ESTIMATOR_KEY).c_str());
        }
        else {
            log.FormatLine(LOG_INFO,
                           MODULE_NAME,
                           "Using default Load Estimator: %s",
                           DEFAULT_LOAD_ESTIMATOR.c_str());
            config.set(LOAD_ESTIMATOR_KEY, DEFAULT_LOAD_ESTIMATOR);
        }
        auto& factory = LoadEstimatorFactory::instance();
        LoadEstimator* loadEstimator =
            factory.Create(config.getString(LOAD_ESTIMATOR_KEY), config).release();
        container.setLoadEstimator(loadEstimator);

        log.WriteLine(LOG_WARN, MODULE_NAME, "Build complete");
        return container;
    }

    void AsyncPrognoserBuilder::reset() {
        lock_guard guard(m);
        config = ConfigMap();
    }
}
