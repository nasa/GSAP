// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <stdexcept>
#include <thread>

#include "Messages/WaypointMessage.h"
#include "ThreadSafeLog.h"
#include "TrajectoryService.h"

namespace PCOE {
    static const Log& log = Log::Instance();
    static const std::string MODULE_NAME = "PRED-ED";
    
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
}
