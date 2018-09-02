// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <stdexcept>
#include <thread>

#include "Messages/Message.h"
#include "Messages/WaypointMessage.h"
#include "ThreadSafeLog.h"
#include "TrajectoryService.h"

namespace PCOE {
    static const Log& log = Log::Instance();
    static const std::string MODULE_NAME = "PRED-ED";
    
    TrajectoryService::TrajectoryService(MessageBus& messageBus,
                      std::string source)
    : bus(messageBus), source(source) {
        lock_guard guard(m);
        bus.subscribe(this, this->source, MessageId::RouteStart);
        bus.subscribe(this, this->source, MessageId::RouteEnd);
        bus.subscribe(this, this->source, MessageId::RouteClear);
        bus.subscribe(this, this->source, MessageId::RouteDeleteWP);
        bus.subscribe(this, this->source, MessageId::RouteSetWP);
    }
    
    TrajectoryService::~TrajectoryService() {
        lock_guard guard(m);
        bus.unsubscribe(this);
    }
    
    std::set<Message::time_point> TrajectoryService::getSavePts() {
        return savepts;
    }
    
    Point3D TrajectoryService::getPoint(Message::time_point time) {
        WaypointMessage * last_wp = nullptr;
        for (auto && waypoint : waypoints) {
            if (time < waypoint.first) {
                if (last_wp == nullptr) {
                    throw std::out_of_range("Cannot exterpolate time before first waypoint");
                }
                // Interpolate
                double tmp = (time - last_wp->getEta()).count();
                double tmp2 =(waypoint.first - last_wp->getEta()).count();
                double ratio = tmp/tmp2;
                auto lat = last_wp->getLatitude()+ (waypoint.second.getLatitude() - last_wp->getLatitude())*ratio;
                auto lon = last_wp->getLongitude() + (waypoint.second.getLongitude() - last_wp->getLongitude())*ratio;
                auto alt = last_wp->getAltitude() + (waypoint.second.getAltitude() - last_wp->getAltitude())*ratio;
                return Point3D(lat, lon, alt);
            }
            last_wp = &waypoint.second;
        }
        throw std::out_of_range("Cannot exterpolate time after last waypoint");
    }
    
    void TrajectoryService::processMessage(const std::shared_ptr<Message>& message) {
        unique_lock lock(m, std::chrono::milliseconds(10));
        
        MessageId id = message->getMessageId();
        switch (id) {
            case MessageId::RouteStart:
                break;
            case MessageId::RouteEnd:
                // TODO (JW): Generate some message that can be used to trigger prediction?
                break;
            case MessageId::RouteClear:
                waypoints.clear();
                savepts.clear();
                break;
            case MessageId::RouteDeleteWP: {
                auto msg = dynamic_cast<U64Message*>(message.get());
                Require(msg, "Unexpected message type for RouteDeleteWP");
                
                auto eta = MessageClock::time_point(MessageClock::duration(msg->getValue()));
                waypoints.erase(eta);
                savepts.erase(eta);
                break;
            }
            case MessageId::RouteSetWP: {
                auto msg = dynamic_cast<WaypointMessage*>(message.get());
                Require(msg, "Unexpected message type for RoutSetWP");
                
                auto eta = msg->getEta();
                auto existing = waypoints.find(eta);
                if (existing == waypoints.end()) {
                    waypoints.insert(std::make_pair(eta, *msg));
                    savepts.insert(eta);
                }
                else {
                    auto& wp = (*existing).second;
                }
                break;
            }
            default: {
                auto it = valueIndices.find(id);
                if (it != valueIndices.end()) {
                    auto msg = dynamic_cast<DoubleMessage*>(message.get());
                    Require(msg, "Unexpected message type for input message");
                    
                    const double prevWeight = 0.9;
                    const double newWeight = 1 - prevWeight;
                    auto i = (*it).second;
                    values[i] = values[i] * prevWeight + msg->getValue() * newWeight;
                }
                break;
            }
        }
    }
}
