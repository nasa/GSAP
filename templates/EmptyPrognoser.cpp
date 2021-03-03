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

#include <vector>

#include "ConfigMap.h"
#include "DataPoint.h"
#include "EmptyPrognoser.h"
#include "ProgEvent.h"

namespace PCOE {
    std::string moduleName = "Empty Prognoser";
    EmptyPrognoser::EmptyPrognoser(ConfigMap& configMap) {
        // Handle Configuration
        log.WriteLine(LOG_DEBUG, moduleName, "Configuring");
        // std::string a = configMap.at("ExampleParam");
    }

    Prediction EmptyPrognoser::step(std::map<MessageId, Datum<double>> data) {
        // Run calculations
        std::vector<ProgEvent> events;
        std::vector<UData> eventState = {UData(1.0)}; // Example Event state (each element is a time

        // Example System State [Time][state variable] -> UData
        std::vector<std::vector<UData>> systemState = {{UData(99.2), UData(110.10)}};
        
        UData toe = UData(124222.01); // Example time of event
        events.push_back(ProgEvent(MessageId::TestEvent0, eventState, systemState, toe));

        std::vector<DataPoint> trajectories; // System Trajectories
        DataPoint exampleTrajectory;
        exampleTrajectory.setNumTimes(1); // Save only one time
        exampleTrajectory[0] = UData(0.5);
        trajectories.push_back(exampleTrajectory);

        Prediction pred(events, trajectories);
        return pred;
    }
}
