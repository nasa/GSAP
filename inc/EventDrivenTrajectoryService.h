// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_EVENTDRIVENTRAJECTORYSERVICE_H
#define PCOE_EVENTDRIVENTRAJECTORYSERVICE_H

#include <map>
#include <string>
#include <vector>

#include "Messages/IMessageProcessor.h"
#include "Messages/MessageBus.h"
#include "Messages/WaypointMessage.h"
#include "TrajectoryService.h"

namespace PCOE {
    /**
     * Maintains knowledge of the route (series of waypoints) and predicted path for a vehicle.
     * Provides estimates of a vehicles position at a given time, and path-driven save points.
     *
     * @author Christopher Teubert
     * @since 1.2
     **/
    class EventDrivenTrajectoryService : public IMessageProcessor {
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
         */
        EventDrivenTrajectoryService(MessageBus& messageBus,
                                     std::unique_ptr<TrajectoryService>&& ts,
                                     std::string source);

        /**
         * Unsubscribes the {@code TrajectoryService} from the message bus.
         */
        ~EventDrivenTrajectoryService();
        
        TrajectoryService & getTrajectoryService() {
            return *trajService.get();
        }

        /**
         *  Process a received trajectory message
         */
        void processMessage(const std::shared_ptr<Message>& message) override;

    private:
        using mutex = std::timed_mutex;
        using lock_guard = std::lock_guard<mutex>;
        using unique_lock = std::unique_lock<mutex>;
        
        std::unique_ptr<TrajectoryService> trajService;

        MessageBus& bus;
        std::string source;
        mutable mutex m;
    };
}
#endif
