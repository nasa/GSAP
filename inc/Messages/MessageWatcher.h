// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGEWATCHER_H
#define PCOE_MESSAGEWATCHER_H
#include "Messages/MessageBus.h"

namespace PCOE {
    /**
     * Subscribes to a list of messages and tracks whether at least one of each
     * message has been received since the last reset.
     *
     * @remarks The watcher currently assumes that all watched messages are
     *          @{code DoubleMessage}s.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    template <class TContainer>
    class MessageWatcher final : public IMessageProcessor {
    public:
        /**
         * Constructs a new @{code MessageWatcher}.
         *
         * @paramt TContainer The type of the container used to store values.
         * @param  messageBus The message bus on which to subscribe.
         * @param  sourceName The source for which to subscribe.
         * @param  messages   The ids of the messages for which to subscribe
         * @param  container  The container used to hold values. This container
         *                    must accept indexing up to the length of @{param
         *                    messages}.
         **/
        MessageWatcher(MessageBus& messageBus,
                       const std::string& sourceName,
                       const std::vector<MessageId> messages,
                       TContainer container)
            : messageBus(messageBus), values(container) {
            for (std::size_t i = 0; i < messages.size(); ++i) {
                msgIndices.insert(std::make_pair(messages[i], i));
                messageBus.subscribe(sourceName, messages[i], this);
            }
        }

        /**
         * Unsubscribes the @{code MessageWatcher} from all messages.
         **/
        ~MessageWatcher() {
            messageBus.unsubscribe(this);
        }

        /**
         * Processes a message, updating its value in the container and marking
         * it present.
         **/
        void processMessage(const std::shared_ptr<Message>& message) override {
            const DoubleMessage* dmsg = reinterpret_cast<DoubleMessage*>(message.get());

            std::size_t i = msgIndices.at(message->getMessageId());
            values[i] = dmsg->getValue();
            if (!present[i]) {
                present[i] = true;
                allPresentCached = false;
            }
        }

        /**
         * Marks all watched messages as not present.
         **/
        void reset() {
            present.clear();
            present.resize(values.size());
            allPresentCached = false;
        }

        /**
         * Gets the value container held by the watcher.
         **/
        inline const TContainer& getValues() const {
            return values;
        }

        /**
         * Returns true if at least one of each message type has been received
         * since the last reset. Otherwise, returns false.
         **/
        bool allPresent() const {
            if (allPresentCached) {
                return allPresentValue;
            }

            for (bool b : present) {
                if (!b) {
                    allPresentCached = true;
                    allPresentValue = false;
                    return false;
                }
            }
            allPresentCached = true;
            allPresentValue = true;
            return true;
        }

    private:
        MessageBus& messageBus;
        std::unordered_map<MessageId, std::size_t> msgIndices;
        TContainer values;
        std::vector<bool> present;
        bool allPresentCached;
        bool allPresentValue;
    };
}
#endif
