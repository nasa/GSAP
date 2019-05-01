// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_PREDICTION_H
#define PCOE_MESSAGES_PREDICTION_H

#include "Messages/ScalarMessage.h"
#include "Predictors/Predictor.h"
#include "ProgEvent.h"

namespace PCOE {
    /**
     * A message the carries a single PredictionMessage.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class PredictionMessage final : public Message {
    public:
        /**
         * Constructs a new instance of @{code PredictionMessage}.
         *
         * @param id        The id of the message.
         * @param source    The source of the message.
         * @param timestamp The time at which the message or the data contained
         *                  by the message was generated.
         * @param value     The value of the message.
         **/
        PredictionMessage(std::string source, time_point timestamp, const Prediction& value)
            : Message(MessageId::Prediction, source, timestamp), value(value) {}

        /**
         * Gets the value associated with the message.
         **/
        inline const Prediction& getValue() const {
            return value;
        }

    protected:
        std::uint32_t getPayloadSize() const override final {
            throw std::domain_error("Serialization not supported");
        }

        void serializePayload(std::ostream& os) const override final {
            (void)os;
            throw std::domain_error("Serialization not supported");
        }

    private:
        Prediction value;
    };
}

#endif
