// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_EVENTDRIVENPREDICTOR_H
#define PCOE_EVENTDRIVENPREDICTOR_H
#include <memory>

#include "Messages/DoubleMessage.h"
#include "Messages/IMessageProcessor.h"
#include "Messages/MessageBus.h"
#include "Messages/UDataVecMessage.h"
#include "Predictors/Predictor.h"

namespace PCOE {
    class EventDrivenPredictor final : public IMessageProcessor {
    public:
        EventDrivenPredictor(MessageBus& messageBus,
                             std::unique_ptr<Predictor>&& predictor,
                             std::string source);

        ~EventDrivenPredictor();

        void processMessage(const std::shared_ptr<Message>& message) override;

    private:
        MessageBus& bus;
        std::unique_ptr<Predictor> pred;
        std::string source;
        double latestTimestamp;
    };
}
#endif
