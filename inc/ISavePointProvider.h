// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_ISAVEPOINTPROVIDER_H
#define PCOE_ISAVEPOINTPROVIDER_H

#include <set>

#include "Messages/Message.h"

namespace PCOE {
    /**
     * An interface for objects that provide save points.
     *
     * @author Chris Teubert
     * @since 1.2
     **/
    class ISavePointProvider {
    public:
        /**
         *  @return A set of savepoints
         */
        virtual std::set<Message::time_point> getSavePts() = 0;
        
        /**
         *  @return If the data has changed since the last time getSavePts was called
         */
        virtual bool hasChangedSinceSavePtsCall() const = 0;
    };
}
#endif
