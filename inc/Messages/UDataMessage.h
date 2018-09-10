// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_MESSAGES_UDATAMESSAGE_H
#define PCOE_MESSAGES_UDATAMESSAGE_H

#include "Messages/ScalarMessage.h"
#include "Messages/VectorMessage.h"
#include "UData.h"

namespace PCOE {
    using UDataMessage = ScalarMessage<UData>;
    using UDataVecMessage = VectorMessage<UData>;
}

#endif
