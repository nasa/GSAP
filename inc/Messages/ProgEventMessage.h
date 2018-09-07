// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_PROGEVENT_H
#define PCOE_MESSAGES_PROGEVENT_H

#include "Messages/ScalarMessage.h"
#include "ProgEvent.h"

namespace PCOE {
    using ProgEventMessage = ScalarMessage<ProgEvent>;
}

#endif
