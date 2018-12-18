// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_EVENTDRIVENPROGNOSER_H
#define PCOE_EVENTDRIVENPROGNOSER_H
#include <memory>

#include "Datum.h"
#include "Messages/ScalarMessage.h"
#include "Loading/LoadEstimator.h"
#include "Models/SystemModel.h"
#include "Observers/EventDrivenObserver.h"
#include "Predictors/EventDrivenPredictor.h"
#include "Trajectory/EventDrivenTrajectoryService.h"

namespace PCOE {
    /**
     * Holds a set of prognostic components that are related to each other. This
     * class doesn't "do" anything; it only acts as a container for other
     * objects that should be deallocated as a group. The container does not do
     * any validation, and will accept null pointers for any of its contructor
     * arguments.
     **/
    class EventDrivenPrognoser : public IMessageProcessor {
    public:
        EventDrivenPrognoser(MessageBus& messageBus, std::string source): bus(messageBus), src(source) {
        };

        EventDrivenPrognoser(const EventDrivenPrognoser&) = delete;

        EventDrivenPrognoser(EventDrivenPrognoser&& other) : bus(other.bus), src(other.src) {
            using std::swap;
            if (&other == this) {
                return;
            }

            swap(model, other.model);
            swap(loadEstimator, other.loadEstimator);
            swap(eventListeners, other.eventListeners);
        }

        ~EventDrivenPrognoser() {
            if (model != nullptr) {
                delete model;
            }
            if (loadEstimator != nullptr) {
                delete loadEstimator;
            }
            for (IMessageProcessor* listener : eventListeners) {
                delete listener;
            }
        }
        
        void setModel(SystemModel* model) {
            Expect(model != nullptr, "null model");
            Expect(this->model == nullptr, "model already set");
            this->model = model;
        }

        void setLoadEstimator(LoadEstimator* loadEstimator) {
            Expect(loadEstimator != nullptr, "null loadEstimator");
            Expect(this->loadEstimator == nullptr, "loadEstimator already set");
            this->loadEstimator = loadEstimator;
        }

        void addEventListener(IMessageProcessor* listener) {
            Expect(listener != nullptr, "null listener");
            eventListeners.push_back(listener);
        }
        
        /** @brief     Prognostic Monitor Step
         *
         *             Preform model updates. This is done every step where there is
         *             enough data. This is a required method in any component
         *             prognoser
         */
        Prediction step(std::map<MessageId, Datum<double>> data) {
            for (const auto& messagePair : data) {
                ScalarMessage<double>* message = new ScalarMessage<double>(messagePair.first, src, Message::time_point(std::chrono::milliseconds(messagePair.second.getTime())), messagePair.second.get());
                bus.publish(std::shared_ptr<Message>(message));
            }
            
            return Prediction::EmptyPrediction();
        }
        
        /**
         * Handles messages representing updates to the model inputs and
         * outputs. When sufficient new data is collected, automaticlly triggers
         * an observer step and publishes the result.
         *
         * @param message. The message to process.
         **/
        void processMessage(const std::shared_ptr<Message>& message) override {
            
        }

    private:
        MessageBus& bus;
        const std::string src;
        SystemModel* model = nullptr;
        LoadEstimator* loadEstimator = nullptr;
        std::vector<IMessageProcessor*> eventListeners;
    };
}

#endif
