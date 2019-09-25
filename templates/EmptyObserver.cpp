// Copyright (c) 2017-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

// Supress warnings for unused parameters. Remove this when copying the template
// to create a new instance of the templated class.
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#elif defined __clang__
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "Models/SystemModel.h"
#include "UData.h"

#include "EmptyObserver.h"
#include "Exceptions.h"

namespace PCOE {
    const static Log& log = Log::Instance();

    const std::string MODULE_NAME = "OBS-Empty";

    EmptyObserver::EmptyObserver(const SystemModel& m, const Matrix Q, const Matrix R)
        : Observer(m) {}

    // Initialize function (required by Observer interface)
    void EmptyObserver::initialize(const double t0,
                                   const SystemModel::state_type& x0,
                                   const SystemModel::input_type& u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");
        auto initialized_state = model.getStateVector();

        // Initialization activities
        // ...

        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }

    // Step function (required by Observer interface)
    void EmptyObserver::step(double t,
                             const SystemModel::input_type& u,
                             const SystemModel::output_type& z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");

        if (!isInitialized()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Called step before initialized");
            throw std::domain_error("EmptyObserver::step not initialized");
        }

        // Perform state estimation
        // ...
    }

    std::vector<UData> EmptyObserver::getStateEstimate() const {
        std::vector<UData> state(model.getStateSize());

        // Fill state

        return state;
    }
}
