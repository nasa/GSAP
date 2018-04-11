
//  STDINCommunicator.hpp
//  ProgInst
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 7/27/17.
//
//

#ifndef STDINCommunicator_h
#define STDINCommunicator_h

#include "CommonCommunicator.h"
#ifdef _WIN32
#include "Windows.h"
#else
#include <sys/select.h>
#include <unistd.h>
#endif

namespace PCOE {
    class STDINCommunicator : public CommonCommunicator {
    public:
#ifdef _WIN32
        using timeout_type = DWORD;
#else
        using timeout_type = timeval;
#endif

        STDINCommunicator();
        void poll() override;

    protected:
        timeout_type timeout;
    };
}

#endif /* STDINCommunicator_h */
