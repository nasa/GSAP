// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include "Contracts.h"
#include "Messages/PredictionMessage.h"
#include "Messages/ProgEventMessage.h"
#include "Messages/UDataMessage.h"
#include "Predictors/AsyncPredictor.h"

namespace PCOE {
    static const Log& log = Log::Instance();
    static const std::string MODULE_NAME = "PRED-ED";

    AsyncPredictor::AsyncPredictor(MessageBus& messageBus,
                                   std::unique_ptr<Predictor>&& predictor,
                                   std::string source,
                                   bool batch)
        : bus(messageBus),
          pred(std::move(predictor)),
          source(std::move(source)),
          batchEvents(batch) {
        Expect(pred, "Predictor pointer is empty");
        lock_guard guard(m);
        bus.subscribe(this, this->source, MessageId::ModelStateEstimate);
    }

    AsyncPredictor::~AsyncPredictor() {
        lock_guard guard(m);
        bus.unsubscribe(this);
    }

    void AsyncPredictor::processMessage(const std::shared_ptr<Message>& message) {
        // Note (JW): If we are unable to aquire the lock within a few
        //            milliseconds, the predictor is already in the middle of a
        //            prediction, so we need to drop the current message to keep
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
        if (batchEvents) {
            auto pMsg = std::shared_ptr<PredictionMessage>(
                new PredictionMessage(source, m->getTimestamp(), std::move(prediction)));
            bus.publish(pMsg);
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Publishing prediction");
        }
        else {
            for (const auto& event : prediction.getEvents()) {
                auto peMsg = std::shared_ptr<ProgEventMessage>(
                    new ProgEventMessage(event.getId(), source, m->getTimestamp(), event));
                bus.publish(peMsg);
                log.FormatLine(LOG_TRACE,
                               MODULE_NAME,
                               "Publishing event with id %x",
                               static_cast<std::uint64_t>(event.getId()));
            }
        }
    }
}
