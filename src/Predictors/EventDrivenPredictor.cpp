// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include "Predictors/EventDrivenPredictor.h"
#include "Contracts.h"
#include "Messages/ProgEventMessage.h"
#include "Messages/UDataMessage.h"

namespace PCOE {
    EventDrivenPredictor::EventDrivenPredictor(MessageBus& messageBus,
                                               std::unique_ptr<Predictor>&& predictor,
                                               std::string source)
        : bus(messageBus), pred(std::move(predictor)), source(std::move(source)) {
        Expect(pred, "Predictor pointer is empty");
        lock_guard guard(m);
        bus.subscribe(this, this->source, MessageId::ModelStateEstimate);
    }

    EventDrivenPredictor::~EventDrivenPredictor() {
        lock_guard guard(m);
        bus.unsubscribe(this);
    }

    void EventDrivenPredictor::processMessage(const std::shared_ptr<Message>& message) {
        // Note (JW): If we are unable to aquire the lock within a few
        //            milliseconds, the predictor is already in the middle of a
        //            prediction, so we need to drop the current message to kep
        //            the queue from backing up.
        unique_lock lock(m, std::chrono::milliseconds(10));
        if (!lock.owns_lock()) {
            return;
        }

        Expect(message->getMessageId() == MessageId::ModelStateEstimate, "Unexpected message id");
        UDataVecMessage* m = dynamic_cast<UDataVecMessage*>(message.get());
        Expect(m != nullptr, "Unexpected message type");

        Prediction prediction = pred->predict(seconds(m->getTimestamp()), m->getValue());
        for (const auto& event : prediction.getEvents()) {
            auto peMsg = std::shared_ptr<ProgEventMessage>(
                new ProgEventMessage(event.getId(), source, event));
            bus.publish(peMsg);
        }
    }
}
