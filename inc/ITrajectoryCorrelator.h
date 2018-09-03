// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#ifndef PCOE_ITRAJECTORYCORRELATOR_H
#define PCOE_ITRAJECTORYCORRELATOR_H

#include "Messages/Message.h"
#include "Point3D.h"

namespace PCOE {
    
    /**
     * An interface for objects that correlate time to physical position in 3d space
     *
     * @author Chris Teubert
     * @since 1.2
     **/
    class ITrajectoryCorrelator {
    public:
        /**
         *  Get a point along a trajectory corresponding to a specific timepoint
         *  @param  tp  The timepoint for which to find the correlated position
         */
        virtual Point3D getPoint(Message::time_point tp) = 0;
    };
}
#endif
