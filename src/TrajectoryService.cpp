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
    static const std::string MODULE_NAME = "TSVC";

    std::set<TrajectoryService::time_point> TrajectoryService::getSavePts() {
        return savepts;
    }

    void TrajectoryService::setWaypoint(TrajectoryService::time_point eta, const Point3D& wp) {
        auto existing = waypoints.find(eta);
        if (existing == waypoints.end()) {
            waypoints.insert(std::make_pair(eta, wp));
            savepts.insert(eta);
        }
        else {
            (*existing).second = wp;
        }
    }

    void TrajectoryService::clearWaypoints() {
        waypoints.clear();
        savepts.clear();
    }

    void TrajectoryService::deleteWaypoint(TrajectoryService::time_point eta) {
        waypoints.erase(eta);
        savepts.erase(eta);
    }

    Point3D TrajectoryService::getPoint(TrajectoryService::time_point time) {
        Point3D* lastWP = nullptr;
        TrajectoryService::time_point lastTime;
        for (auto&& waypoint : waypoints) {
            if (time < waypoint.first) {
                if (lastWP == nullptr) {
                    throw std::out_of_range("Cannot exterpolate time before first waypoint");
                }
                // Interpolate
                double tmp = (time - lastTime).count();
                double tmp2 = (waypoint.first - lastTime).count();
                double ratio = tmp / tmp2;
                auto lat = lastWP->getLatitude() +
                           (waypoint.second.getLatitude() - lastWP->getLatitude()) * ratio;
                auto lon = lastWP->getLongitude() +
                           (waypoint.second.getLongitude() - lastWP->getLongitude()) * ratio;
                auto alt = lastWP->getAltitude() +
                           (waypoint.second.getAltitude() - lastWP->getAltitude()) * ratio;
                return Point3D(lat, lon, alt);
            }
            lastWP = &waypoint.second;
            lastTime = waypoint.first;
        }
        throw std::out_of_range("Cannot exterpolate time after last waypoint");
    }
}
