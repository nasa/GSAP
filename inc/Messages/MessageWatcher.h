// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGEWATCHER_H
#define PCOE_MESSAGEWATCHER_H
#include <map>

#include "Contracts.h"
#include "Messages/MessageBus.h"
#include "Messages/ScalarMessage.h"
#include "Messages/VectorMessage.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    /**
     * Subscribes to a list of messages and tracks whether at least one of each
     * message has been received. Each time a full set of messages is received,
     * The values are published to the message bus with a user-specified
     * message id.
     *
     * @remarks The watcher currently assumes that all watched messages are
     *          @{code DoubleMessage}s.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    template <class T>
    class MessageWatcher final : public IMessageProcessor {
    public:
        /**
         * Constructs a new @{code MessageWatcher}.
         *
         * @paramt T          The scalar type being watched.
         * @param  messageBus The message bus on which to subscribe.
         * @param  sourceName The source for which to subscribe.
         * @param  messages   The ids of the messages for which to subscribe
         * @param  pubId      The MessageID used when publishing the watched
         *                    data to the bus.
         * @param  container  The container used to hold values. This container
         *                    must accept indexing up to the length of @{param
         *                    messages}.
         **/
        MessageWatcher(MessageBus& messageBus,
                       std::string sourceName,
                       const std::vector<MessageId> messages,
                       MessageId pubId)
            : log(ThreadSafeLog::Instance()),
              messageBus(messageBus),
              source(std::move(sourceName)),
              pubId(pubId),
              values(messages.size()) {
            for (std::size_t i = 0; i < messages.size(); ++i) {
                present.push_back(false);
                msgIndices.insert(std::make_pair(messages[i], i));
                messageBus.subscribe(this, source, messages[i]);
                log.FormatLine(LOG_DEBUG,
                               "MSGWACH",
                               "Subscribed to id 0x%llx for source %s",
                               messages[i],
                               source.c_str());
            }
            Ensure(messages.size() == values.size(), "Mismatched container sizes");
            Ensure(present.size() == values.size(), "Mismatched present and value sizes");
        }

        /**
         * Unsubscribes the @{code MessageWatcher} from all messages.
         **/
        ~MessageWatcher() {
            lock_guard guard(m);
            messageBus.unsubscribe(this);
        }

        /**
         * Processes a message, updating its value in the container and marking
         * it present.
         **/
        void processMessage(const std::shared_ptr<Message>& message) override {
            lock_guard guard(m);
            auto smsg = dynamic_cast<ScalarMessage<T>*>(message.get());
            Expect(smsg != nullptr, "Unexpected message type");
            log.FormatLine(LOG_DEBUG,
                           "MSGWACH",
                           "Processing message with id 0x%llx from source %s",
                           static_cast<std::uint64_t>(message->getMessageId()),
                           message->getSource().c_str());

            std::size_t i = msgIndices.at(message->getMessageId());
            values[i] = smsg->getValue();
            if (!present[i]) {
                present[i] = true;
                allPresentCached = false;
            }

            if (allPresent()) {
                auto vmsg = new VectorMessage<T>(pubId, source, message->getTimestamp(), values);
                log.FormatLine(LOG_DEBUG,
                               "MSGWACH",
                               "Publishming message for source %s",
                               message->getSource().c_str());
                messageBus.publish(std::shared_ptr<Message>(vmsg));
                reset();
            }
        }

    private:
        /**
         * Marks all watched messages as not present.
         **/
        void reset() {
            present.clear();
            present.resize(values.size());
            allPresentCached = false;
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
        using mutex = std::mutex;
        using lock_guard = std::lock_guard<mutex>;

        Log& log;
        MessageBus& messageBus;
        std::string source;
        MessageId pubId;
        std::map<MessageId, std::size_t> msgIndices;
        std::vector<T> values;
        std::vector<bool> present;
        mutable bool allPresentCached = false;
        mutable bool allPresentValue = false;
        mutable mutex m;
    };
}
#endif
