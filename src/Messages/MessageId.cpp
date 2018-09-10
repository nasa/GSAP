// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <unordered_map>

#include "Contracts.h"
#include "Messages/MessageId.h"

namespace PCOE {
    MessageId messageIdFromString(const std::string& key) {
        static std::unordered_map<std::string, MessageId> messages{
            {"All", MessageId::All},
            {"Volts", MessageId::Volts},
            {"Amperes", MessageId::Amperes},
            {"Watts", MessageId::Watts},
            {"Kelvin", MessageId::Kelvin},
            {"Centigrade", MessageId::Centigrade},
            {"Fahrenheit", MessageId::Fahrenheit},
            {"ModelStateEstimate", MessageId::ModelStateEstimate},
            {"TestInput0", MessageId::TestInput0},
            {"TestInput1", MessageId::TestInput1},
            {"TestInput2", MessageId::TestInput2},
            {"TestInput3", MessageId::TestInput3},
            {"TestOutput0", MessageId::TestOutput0},
            {"TestOutput1", MessageId::TestOutput1},
            {"TestOutput2", MessageId::TestOutput2},
            {"TestOutput3", MessageId::TestOutput3},
            {"BatteryEod", MessageId::BatteryEod},
            {"BatteryEol", MessageId::BatteryEol},
            {"RouteStart", MessageId::RouteStart},
            {"RouteEnd", MessageId::RouteEnd},
            {"RouteClear", MessageId::RouteClear},
            {"RouteDeleteWP", MessageId::RouteDeleteWP},
            {"RouteSetWP", MessageId::RouteSetWP},
            {"Intialize", MessageId::Intialize},
            {"Start", MessageId::Start},
            {"Stop", MessageId::Stop},
            {"Pause", MessageId::Pause},
            {"Resume", MessageId::Resume}};

        std::unordered_map<std::string, MessageId>::const_iterator result = messages.find(key);

        Ensure(result != messages.cend(), "Invalid key");
        return (*result).second;
    }
}
