// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_TRAJECTORYSERVICE_h
#define PCOE_TRAJECTORYSERVICE_h

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ISavePointProvider.h"
#include "ITrajectoryCorrelator.h"
#include "Messages/WaypointMessage.h"

namespace PCOE {
    /**
     * Maintains knowledge of the route (series of waypoints) and predicted path for a vehicle. 
     * Provides estimates of a vehicles position at a given time, and path-driven save points.
     *
     * @author Jason Watkins
     * @author Christopher Teubert
     * @since 1.2
     **/
    class TrajectoryService : public ISavePointProvider, public ITrajectoryCorrelator {
    public:
        std::set<Message::time_point> getSavePts() override;
        
        Point3D getPoint(Message::time_point) override;
        
        void setWaypoint(const WaypointMessage &);
        
        void deleteWaypoint(Message::time_point);
        
        void clearWaypoints();
        
    protected:
        std::set<Message::time_point> savepts;
        std::map<MessageClock::time_point, WaypointMessage> waypoints;
        std::map<MessageId, std::size_t> valueIndices;
        std::vector<double> values;
    };
}
#endif
