// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include "Predictors/EventDrivenPredictor.h"
#include "Contracts.h"
#include "Messages/ProgEventMessage.h"

namespace PCOE {
    EventDrivenPredictor::EventDrivenPredictor(MessageBus& messageBus,
                                               std::unique_ptr<Predictor>&& predictor,
                                               std::string source)
        : bus(messageBus), pred(std::move(predictor)), source(std::move(source)) {
        Expect(pred, "Predictor pointer is empty");
        bus.subscribe(this, this->source);
    }

    EventDrivenPredictor::~EventDrivenPredictor() {
        bus.unsubscribe(this);
    }

    void EventDrivenPredictor::processMessage(const std::shared_ptr<Message>& message) {
        switch (message->getMessageId()) {
        case MessageId::ModelStateEstimate: {
            UDataVecMessage* m = reinterpret_cast<UDataVecMessage*>(message.get());

            Prediction prediction = pred->predict(seconds(m->getTimestamp()), m->getValue());
            auto peMsg = std::shared_ptr<ProgEventMessage>(
                new ProgEventMessage(MessageId::BatteryEod, source, prediction.events[0]));
            bus.publish(peMsg);
            break;
        }
        default:
            // Ignore messages we aren't interested in.
            break;
        }
    }
}
