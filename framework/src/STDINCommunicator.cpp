//
//  StdinCommunicator.cpp
//  ProgInst
//
//  Created by Teubert, Christopher (ARC-TI)[SGT, INC] on 7/27/17.
//
//

#include <iostream>
#include <stdio.h>

#include "STDINCommunicator.h"

const std::string MODULE_NAME = "STDINComm";

namespace PCOE {
    STDINCommunicator::STDINCommunicator() {
        log.WriteLine(LOG_INFO, MODULE_NAME, "Configuring");

#ifdef _WIN32
        timeout = 0;
#else
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
#endif
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Configured");
    }

    void STDINCommunicator::poll() {
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Entering Poll");
#ifdef _WIN32
        HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
        if (stdin_handle == NULL) {
            log.WriteLine(LOG_WARN, MODULE_NAME, "No standard input attached");
        }
        else if (stdin_handle == INVALID_HANDLE_VALUE) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Unable to retrieve standard input handle");
        }
        else {
            const DWORD nLength = 1;
            INPUT_RECORD lpBuffer[nLength];
            LPDWORD lpNumberOfEventsRead;
            if (!PeekConsoleInput(stdin_handle, lpBuffer, nLength, lpNumberOfEventsRead)) {
                log.WriteLine(LOG_ERROR, MODULE_NAME, "Unable to read standard input handle");
            }
            else if (*lpNumberOfEventsRead > 0) {
                log.WriteLine(LOG_TRACE, MODULE_NAME, "Data Received");
                setRead();
            }
            else {
                log.WriteLine(LOG_TRACE, MODULE_NAME, "Nothing on STDIN");
                // Timeout
            }
        }
#else
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        int rc = select(1, &fds, nullptr, nullptr, &timeout);
        if (rc < 0) {
            perror("select");
        }
        else if (rc == 0 || (feof(stdin))) {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Nothing on STDIN");
            // Timeout
        }
        else {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Data Received");
            setRead();
        }
#endif
    }
}
