// Copyright (c) 2022 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_LOADLISTENER_H
#define PCOE_LOADLISTENER_H

#include <memory>
#include <string>
#include <vector>

#include "Loading/LoadEstimator.h"
#include "Messages/MessageBus.h"
#include "Messages/VectorMessage.h"
#include "Models/PrognosticsModel.h"

namespace PCOE {
    class LoadListener: public IMessageProcessor {
    public:

        LoadListener(MessageBus& bus, const std::string& src,LoadEstimator* le) : bus(bus), le(le) {
            bus.subscribe(this, src, MessageId::ModelInputVector);
        }

        ~LoadListener() {
            bus.unsubscribe(this);
        }

        void processMessage(const std::shared_ptr<Message>& message) override {

            if (le->canAddLoad()) {
                auto imsgVec = std::dynamic_pointer_cast<DoubleVecMessage, Message>(message);
                le->addLoad(imsgVec->getValue());
            }
        }

    private:
        MessageBus& bus;
        LoadEstimator* le;
        std::vector<double> measurement;
    };

}

#endif /* PCOE_LOADLISTENER_H */
