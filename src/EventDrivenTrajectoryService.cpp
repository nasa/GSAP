// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <stdexcept>
#include <thread>

#include "Messages/Message.h"
#include "Messages/WaypointMessage.h"
#include "ThreadSafeLog.h"
#include "EventDrivenTrajectoryService.h"

namespace PCOE {
    static const Log& log = Log::Instance();
    static const std::string MODULE_NAME = "TRAJ_SERVICE_ED";
    
    EventDrivenTrajectoryService::EventDrivenTrajectoryService(MessageBus& messageBus,
                                         std::string source)
    : bus(messageBus), source(source) {
        lock_guard guard(m);
        bus.subscribe(this, this->source, MessageId::RouteStart);
        bus.subscribe(this, this->source, MessageId::RouteEnd);
        bus.subscribe(this, this->source, MessageId::RouteClear);
        bus.subscribe(this, this->source, MessageId::RouteDeleteWP);
        bus.subscribe(this, this->source, MessageId::RouteSetWP);
    }
    
    EventDrivenTrajectoryService::~EventDrivenTrajectoryService() {
        lock_guard guard(m);
        bus.unsubscribe(this);
    }
    
    void EventDrivenTrajectoryService::processMessage(const std::shared_ptr<Message>& message) {
        unique_lock lock(m, std::chrono::milliseconds(10));
        
        MessageId id = message->getMessageId();
        switch (id) {
            case MessageId::RouteStart:
                break;
            case MessageId::RouteEnd:
                // TODO (JW): Generate some message that can be used to trigger prediction?
                break;
            case MessageId::RouteClear:
                clearWaypoints();
                break;
            case MessageId::RouteDeleteWP: {
                auto msg = dynamic_cast<U64Message*>(message.get());
                Require(msg, "Unexpected message type for RouteDeleteWP");
                
                auto eta = MessageClock::time_point(MessageClock::duration(msg->getValue()));
                deleteWaypoint(eta);
                break;
            }
            case MessageId::RouteSetWP: {
                auto msg = dynamic_cast<WaypointMessage*>(message.get());
                Require(msg, "Unexpected message type for RoutSetWP");
                
                setWaypoint((*msg).getEta(), (*msg).getPosition());
                break;
            }
            default: {
                break;
            }
        }
    }
}
