// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Observers/EventDrivenObserver.h"

namespace PCOE {
    EventDrivenObserver::EventDrivenObserver(MessageBus& messageBus,
                                             std::unique_ptr<Observer>&& observer,
                                             std::string source)
        : bus(messageBus),
          obs(std::move(observer)),
          source(std::move(source)),
          inputWatcher(bus,
                       source,
                       obs->getModel()->getInputs(),
                       obs->getModel()->getInputVector()),
          outputWatcher(bus,
                        source,
                        obs->getModel()->getOutputs(),
                        obs->getModel()->getOutputVector()) {
        Expect(obs, "Observer pointer is empty");
        auto inputs = obs->getModel()->getInputs();
        for (auto i : inputs) {
            bus.subscribe(this, this->source, i);
        }
        auto outputs = obs->getModel()->getOutputs();
        for (auto o : outputs) {
            bus.subscribe(this, this->source, o);
        }
    }

    EventDrivenObserver::~EventDrivenObserver() {
        bus.unsubscribe(this);
    }

    void EventDrivenObserver::processMessage(const std::shared_ptr<Message>& message) {
        latestTimestamp = seconds(message->getTimestamp());

        if (inputWatcher.allPresent() && outputWatcher.allPresent()) {
            auto u = inputWatcher.getValues();
            auto z = outputWatcher.getValues();
            if (!obs->isInitialized()) {
                auto x = obs->getModel()->initialize(u, z);
                obs->initialize(latestTimestamp, x, u);
            }
            else {
                obs->step(latestTimestamp, u, z);
                UDataVecMessage* stateEst = new UDataVecMessage(MessageId::ModelStateEstimate,
                                                                source,
                                                                obs->getStateEstimate());
                bus.publish(std::shared_ptr<Message>(stateEst));
            }
        }
    }
}
