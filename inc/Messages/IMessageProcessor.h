// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_IMESSAGECONSUMER_H
#define PCOE_MESSAGES_IMESSAGECONSUMER_H
#include <memory>

#include "Messages/Message.h"

namespace PCOE {
    /**
     * An interface for objects that consume messages.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class IMessageProcessor {
    public:
        /**
         * Virtual destructor to allow for deletion of objects via their
         * interface pointers.
         **/
        virtual ~IMessageProcessor() = default;

        /**
         * When implmenented in a derived class, processes the message in some
         * implmentation-specifc way.
         *
         * @param message The message to process.
         **/
        virtual void processMessage(const std::shared_ptr<Message>& message) = 0;
    };
}
#endif
