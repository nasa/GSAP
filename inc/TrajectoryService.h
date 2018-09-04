// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_TRAJECTORYSERVICE_H
#define PCOE_TRAJECTORYSERVICE_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ISavePointProvider.h"
#include "ITrajectoryCorrelator.h"
#include "Point3D.h"

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
        /**
         * The type used by 3Dpoints to describe when they correlate to
         **/
        using time_point = MessageClock::time_point;

        /**
         *  @return A set of savepoints
         */
        std::set<time_point> getSavePts() const override;

        /**
         *  Get a point along a trajectory corresponding to a specific timepoint
         *  @param  tp  The timepoint for which to find the correlated position
         */
        Point3D getPoint(time_point) const override;

        bool hasChangedSinceSavePtsCall() const override {
            return changedSinceSavePtsCall;
        }

        /**
         *  Set a waypoint, creates if waypoint doesn't exist already
         *  @param  tp  Timepoint for waypoint
         *  @param  wp  Waypoint (point in 3D space
         */
        void setWaypoint(time_point tp, const Point3D& wp);

        /**
         *  delete a waypoint
         *  @param  tp  Timepoint for waypoint
         */
        void deleteWaypoint(time_point);

        /**
         *  Clear all waypoints
         */
        void clearWaypoints();

    protected:
        mutable bool changedSinceSavePtsCall = false;
        std::set<time_point> savepts;
        std::map<time_point, Point3D> waypoints;
    };
}
#endif
