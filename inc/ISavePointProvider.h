// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_ISAVEPOINTPROVIDER_h
#define PCOE_ISAVEPOINTPROVIDER_h

#include <set>

#include "Messages/Message.h"

namespace PCOE {
    /**
     * An interface for objects that provide save points
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class ISavePointProvider {
    public:
        virtual std::set<Message::time_point> getSavePts() = 0;
    };
}

#endif
