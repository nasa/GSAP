// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include "Messages/EmptyMessage.h"
#include "Contracts.h"

namespace PCOE {
    EmptyMessage::EmptyMessage(MessageId id, std::string source, time_point timestamp)
        : Message(id, source, timestamp) {
        Expect((static_cast<std::uint64_t>(id) & 0x0000FF0000000000L) == 0,
               "Message id is not empty");
    }
}
