// Copyright (c) 2015-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_Prognoser_H
#define PCOE_Prognoser_H

#include <map>

#include "ConfigMap.h"
#include "DataStore.h"
#include "Messages/MessageId.h"
#include "Predictors/Predictor.h"

namespace PCOE {
    /**
     * Provides an interface for synchronous prognosers.
     *
     * @author Chris Teubert
     * @author Jason Watkins
     **/
    class Prognoser {
    public:
        Prognoser() = default;

        virtual ~Prognoser() = default;

        virtual Prediction step(std::map<MessageId, Datum<double>> data) = 0;

    protected:
        Log& log = Log::Instance();
    };
}
#endif
