// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include "Predictors/EventDrivenPredictor.h"
#include "Contracts.h"
#include "Messages/ProgEventMessage.h"
#include "Messages/UDataMessage.h"

namespace PCOE {
    static const Log& log = Log::Instance();
    static const std::string MODULE_NAME = "PRED-ED";

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
            log.WriteLine(LOG_DEBUG, MODULE_NAME, "Skipping prediction. Failed to aquire lock.");
            return;
        }

        Expect(message->getMessageId() == MessageId::ModelStateEstimate, "Unexpected message id");
        UDataVecMessage* m = dynamic_cast<UDataVecMessage*>(message.get());
        Expect(m != nullptr, "Unexpected message type");

        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting prediction");
        Prediction prediction = pred->predict(seconds(m->getTimestamp()), m->getValue());
        log.FormatLine(LOG_TRACE, MODULE_NAME, "Publishing events for source %s", source.c_str());
        for (const auto& event : prediction.getEvents()) {
            auto peMsg = std::shared_ptr<ProgEventMessage>(
                new ProgEventMessage(event.getId(), source, event));
            bus.publish(peMsg);
            log.FormatLine(LOG_TRACE,
                           MODULE_NAME,
                           "Publishing event with id %x",
                           static_cast<std::uint64_t>(event.getId()));
        }
    }
}
