
//  STDINCommunicator.hpp
//  ProgInst
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 7/27/17.
//
//

#ifndef STDINCommunicator_h
#define STDINCommunicator_h

#include <sys/select.h>

#include "CommonCommunicator.h"  ///< Parent Class

namespace PCOE {
    class STDINCommunicator : public CommonCommunicator {
    public:
        STDINCommunicator();
        void poll() override;

    protected:
        timeval timeout;
    };
}

#endif /* STDINCommunicator_h */
