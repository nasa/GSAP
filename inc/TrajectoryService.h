// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_TRAJECTORYSERVICE_h
#define PCOE_TRAJECTORYSERVICE_h

#include <string>
#include <vector>
#include <map>

#include "ISavePointProvider.h"
#include "ITrajectoryCorrelator.h"
#include "Messages/IMessageProcessor.h"
#include "Messages/MessageBus.h"
#include "Messages/WaypointMessage.h"

namespace PCOE {
    class Waypoint {
        
    };
    
    /**
     * Maintains knowledge of the route (series of waypoints) and predicted path for a vehicle. 
     * Provides estimates of a vehicles position at a given time, and path-driven save points.
     *
     * @author Jason Watkins
     * @author Christopher Teubert
     * @since 1.2
     **/
    class TrajectoryService : public ISavePointProvider, public IMessageProcessor, public ITrajectoryCorrelator {
    public:
        /**
         * Constructs a new {@code TrajectoryService}.
         *
         * @param messageBus The message bus on which to listen for and publish
         *                   messages. The {@code TrajectoryService} will
         *                   immediately register to receive messages from the
         *                   message bus, and will publish results of the
         *                   observer to the message bus.
         * @param source     The name of the source that is being observed.
         **/
        TrajectoryService(MessageBus& messageBus,
                          std::string source);
        
        /**
         * Unsubscribes the {@code TrajectoryService} from the message bus.
         **/
        ~TrajectoryService();
        
        std::set<Message::time_point> getSavePts() override;
        
        void processMessage(const std::shared_ptr<Message>& message) override;
        
        Point3D getPoint(Message::time_point) override;
        
    private:
        using mutex = std::timed_mutex;
        using lock_guard = std::lock_guard<mutex>;
        using unique_lock = std::unique_lock<mutex>;
        
        std::set<Message::time_point> savepts;
        MessageBus& bus;
        std::string source;
        mutable mutex m;
        std::map<MessageClock::time_point, WaypointMessage> waypoints;
        std::map<MessageId, std::size_t> valueIndices;
        std::vector<double> values;
    };
}
#endif
