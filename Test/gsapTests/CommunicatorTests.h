/** @file CommunicatorTests.h
 *  @brief Tests the basic functionality of Communicator
 *
 *  @author    Jason Watkins <jason-watkins@outlook.com>
 *  @version   0.2.0
 *  @date      2016-08-04
 *
 *  @copyright Copyright (c) 2013-2018 United States Government as represented by
 *             the Administrator of the National Aeronautics and Space
 *             Administration. All Rights Reserved.
 **/

#ifndef COMMUNICATORTESTS_H
#define COMMUNICATORTESTS_H

#include "Communicator.h"

using namespace PCOE;

namespace TestCommunicator {
    class TestCommunicator : public Communicator {
    public:
        void poll() override {
            setRead();
        }

        DataStore read() override {
            ++readCount;
            return readData;
        }

        void write(AllData aData) override {
            ++writeCount;
            writeData     = aData.doubleDatastore;
        }

        int readCount  = 0;
        int writeCount = 0;
        DataStore readData;
        DataStore writeData;
    };

    void construct();
    void enqueue();
    void subscribe();
    void stop();
}

#endif // CommunicatorTESTS_H
