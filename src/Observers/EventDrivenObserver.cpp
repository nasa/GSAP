// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Observers/EventDrivenObserver.h"
#include "Contracts.h"
#include "Messages/UDataMessage.h"

namespace PCOE {
    EventDrivenObserver::EventDrivenObserver(MessageBus& messageBus,
                                             std::unique_ptr<Observer>&& obs,
                                             std::string src)
        : bus(messageBus),
          observer(std::move(obs)),
          source(std::move(src)),
          inputWatcher(bus, source, observer->getModel()->getInputs(), MessageId::ModelInputVector),
          outputWatcher(bus,
                        source,
                        observer->getModel()->getOutputs(),
                        MessageId::ModelOutputVector) {
        Expect(observer, "Observer pointer is empty");
        bus.subscribe(this, source, MessageId::ModelInputVector);
        bus.subscribe(this, source, MessageId::ModelOutputVector);
    }

    EventDrivenObserver::~EventDrivenObserver() {
        bus.unsubscribe(this);
    }

    void EventDrivenObserver::processMessage(const std::shared_ptr<Message>& message) {
        // TODO (JW): Figure out exactly what needs to be protected by a mutex
        switch (message->getMessageId()) {
        case MessageId::ModelInputVector:
            inputMsg = message;
            break;
        case MessageId::ModelOutputVector:
            outputMsg = message;
            break;
        default:
            Unreachable("Unexpected message type");
        }

        if (inputMsg && outputMsg) {
            latestTimestamp = seconds(message->getTimestamp());
            auto typedInMsg = dynamic_cast<VectorMessage<double>*>(inputMsg.get());
            auto typedOutMsg = dynamic_cast<VectorMessage<double>*>(outputMsg.get());
            const auto& u = Model::input_type(typedInMsg->getValue());
            const auto& z = Model::output_type(typedOutMsg->getValue());
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
            inputMsg = nullptr;
            outputMsg = nullptr;
        }
    }
}
