//
//  StdinCommunicator.cpp
//  ProgInst
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 7/27/17.
//
//

#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "STDINCommunicator.h"

const std::string MODULE_NAME = "STDINComm";

namespace PCOE {
    STDINCommunicator::STDINCommunicator() {
        log.WriteLine(LOG_INFO, MODULE_NAME, "Configuring");

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Configured");
    }
    void STDINCommunicator::poll() {
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Entering Poll");
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        int rc = select(1, &fds, nullptr, nullptr, &timeout);
        if (rc < 0) {
            perror("select");
        } else if (rc == 0 || (feof(stdin))) {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Nothing on STDIN");
            // Timeout
        } else {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Data Received");
            setRead();
        }
    }
}
