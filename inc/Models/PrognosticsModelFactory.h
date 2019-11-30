// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_PROGNOSTICSMODELFACTORY_H
#define PCOE_PROGNOSTICSMODELFACTORY_H

#include "BatteryModel.h"
#include "CentrifugalPumpModel.h"
#include "CircuitBatteryModel.h"
#include "Factory.h"
#include "PrognosticsModel.h"
#include "PneumaticValveModel.h"
#include "Singleton.h"

namespace PCOE {
    /**
     * Creates new @{code PrognosticsModel} objects.
     *
     * @author Mattew Daigle
     * @author Jason Watkins
     * @since 1.0
     **/
    class PrognosticsModelFactory : public Factory<PrognosticsModel, const ConfigMap&>,
                                    public Singleton<PrognosticsModelFactory> {
        friend class Singleton<PrognosticsModelFactory>;

    private:
        /**
         * Creates a new instance of the @{code ObserverFactory} with default
         * observers registered. This constructor should only be called once,
         * from the parent @{code Singleton} class.
         **/
        PrognosticsModelFactory() {
            Register<BatteryModel>("Battery");
            Register<BatteryModel>("Battery.Electrochemistry");
            Register<CircuitBatteryModel>("Battery.Circuit");
            Register<CentrifugalPumpModel>("Centrifugal Pump");
            Register<PneumaticValveModel>("Pneumatic Valve");
        };
    };
}

#endif
