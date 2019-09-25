// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_EVENTDRIVENPREDICTOR_H
#define PCOE_EVENTDRIVENPREDICTOR_H
#include <memory>

#include "Messages/IMessageProcessor.h"
#include "Messages/MessageBus.h"
#include "Predictors/Predictor.h"
#include "Trajectory/AsyncTrajectoryService.h"

namespace PCOE {
    /**
     * Provides an event-driven wrapper around a predictor. The event-driven
     * wrapper listens for updates state estimates from the observer and
     * produces new predictions based on those updates.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class AsyncPredictor final : public IMessageProcessor {
    public:
        /**
         * Constructs a new {@code AsyncPredictor}.
         *
         * @param messageBus The message bus on which to listen for and publish
         *                   messages. The {@code AsyncPredictor} will
         *                   immediately register to receive messages from the
         *                   message bus, and will publish results of the
         *                   observer to the message bus.
         * @param observer   The predictor that the {@code AsyncPredictor}
         *                   uses to produce predictions.
         * @param source     The name of the source that is being observed.
         * @param batch      True to publish a single message per prediction.
         *                   False to send one message per event in the
         *                   prediction.
         **/
        AsyncPredictor(MessageBus& messageBus,
                             std::unique_ptr<Predictor>&& predictor,
                             std::string source,
                             bool batch = false);

        /**
         * Unsubscribes the {@code AsyncPredictor} from the message bus.
         **/
        ~AsyncPredictor();

        const std::string& getName() {
            return source;
        }

        /**
         * Checks each message to see if it is a state update from the observer.
         * If so, runs a prediction step and publishes the result to the bus.
         *
         * @param message. The message to process.
         **/
        void processMessage(const std::shared_ptr<Message>& message) override;

    private:
        using mutex = std::timed_mutex;
        using lock_guard = std::lock_guard<mutex>;
        using unique_lock = std::unique_lock<mutex>;

        mutable mutex m;
        MessageBus& bus;
        std::unique_ptr<Predictor> pred;
        std::string source;
        bool batchEvents;
    };
}
#endif
