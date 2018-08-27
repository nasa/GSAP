// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Observers/EventDrivenObserver.h"
#include "Contracts.h"
#include "Messages/UDataMessage.h"

namespace PCOE {
    static const Log& log = Log::Instance();
    static const std::string MODULE_NAME = "OBS-ED";

    EventDrivenObserver::EventDrivenObserver(MessageBus& messageBus,
                                             std::unique_ptr<Observer>&& obs,
                                             std::string src)
        : bus(messageBus),
          observer(std::move(obs)),
          source(std::move(src)),
          inputWatcher(bus, source, observer->getModel().getInputs(), MessageId::ModelInputVector),
          outputWatcher(bus,
                        source,
                        observer->getModel().getOutputs(),
                        MessageId::ModelOutputVector) {
        Expect(observer, "Observer pointer is empty");
        lock_guard guard(m);
        bus.subscribe(this, source, MessageId::ModelInputVector);
        bus.subscribe(this, source, MessageId::ModelOutputVector);
    }

    EventDrivenObserver::~EventDrivenObserver() {
        lock_guard guard(m);
        bus.unsubscribe(this);
    }

    void EventDrivenObserver::processMessage(const std::shared_ptr<Message>& message) {
        // Note (JW): If we are unable to aquire the lock immediately, the
        //            observer is already processing a message. If we can't
        //            aquire the lock within a few milliseconds, the observer is
        //            probably producing a state estimate, and the message queue
        //            is backed up.
        unique_lock lock(m, std::chrono::milliseconds(10));
        if (!lock.owns_lock()) {
            log.WriteLine(LOG_DEBUG, MODULE_NAME, "Skipping observer step. Failed to aquire lock.");
            return;
        }

        switch (message->getMessageId()) {
        case MessageId::ModelInputVector:
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Set input message");
            inputMsg = message;
            break;
        case MessageId::ModelOutputVector:
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Set ouput message");
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
                log.WriteLine(LOG_TRACE, MODULE_NAME, "Initialized observer");
                auto x = observer->getModel().initialize(u, z);
                observer->initialize(latestTimestamp, x, u);
            }
            else {
                log.WriteLine(LOG_TRACE, MODULE_NAME, "Stepping observer");
                observer->step(latestTimestamp, u, z);
                log.WriteLine(LOG_TRACE, MODULE_NAME, "Publishing observer result");
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
