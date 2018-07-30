// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Observers/EventDrivenObserver.h"

namespace PCOE {
    EventDrivenObserver::EventDrivenObserver(MessageBus& messageBus,
                                             std::unique_ptr<Observer>&& obs,
                                             std::string src)
        : bus(messageBus),
          observer(std::move(obs)),
          source(std::move(src)),
          inputWatcher(bus,
                       source,
                       observer->getModel()->getInputs(),
                       observer->getModel()->getInputVector()),
          outputWatcher(bus,
                        source,
                        observer->getModel()->getOutputs(),
                        observer->getModel()->getOutputVector()) {
        Expect(observer, "Observer pointer is empty");
        auto inputs = observer->getModel()->getInputs();
        for (auto i : inputs) {
            bus.subscribe(this, source, i);
        }
        auto outputs = observer->getModel()->getOutputs();
        for (auto o : outputs) {
            bus.subscribe(this, source, o);
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
            if (!observer->isInitialized()) {
                auto x = observer->getModel()->initialize(u, z);
                observer->initialize(latestTimestamp, x, u);
            }
            else {
                observer->step(latestTimestamp, u, z);
                UDataVecMessage* stateEst = new UDataVecMessage(MessageId::ModelStateEstimate,
                                                                source,
                                                                observer->getStateEstimate());
                bus.publish(std::shared_ptr<Message>(stateEst));
            }
            inputWatcher.reset();
            outputWatcher.reset();
        }
    }
}
