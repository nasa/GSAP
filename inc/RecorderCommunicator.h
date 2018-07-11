/**  Recorder Communicator - Header
 *   @class     RecorderCommunicator RecorderCommunicator.h
 *   @ingroup   GPIC++
 *   @ingroup   commCollection
 *
 *   @brief     Recorder Communicator class- records all data to a file. All data is comma
 *              seperated.
 *              A Header line is written to the top of the file, then data is written
 *              (one line per timestep). The header line is in the following format:
 *                  Timestamp, [Data In], [Prognostic Results], time (in ms since epoch)
 *
 *              Each is described futher below:
 *                  Timestamp:              The current date and time
 *                  Data In:                Input data keys from the communicators
 *                  Prognostic Results:     The output of the prognosers (from the ProgData
 *                                          Structure)
 *                                          In the following format:
 *                                          pData-[%1].%2[%3]%0 where
 *                                          %1: Component Name
 *                                          %2: Element (either Events or sysTrajectories)
 *                                          %3: Identifier (Event or System Trajectory Name)
 *                                          For events the format of %0 above can be
 *                                          one of the following
 *                                          .TOE[%4](%u): Time of event element %4 in uData vector
 *                                              %u: Uncertainty Type
 *                                          .probMatrix[T+%5]: Probability of event at time %5
 *                                          .occurenceMatrix[T+%5][%6]: Occurance for time %5,
 *                                          sample %6
 *                                          For system trajectories the format of %0 above is the
 *                                          following:
 *                                          [T+%5][%4](%u): Occurance for time %5, element %4 in
 *                                          uData vector
 *                                              %u: Uncertainty Type
 *              Sensor data is then printed with a timestamp associated in the format "(t=%7)" in
 *              ms since epoch
 *              Prognostic Results are printed with a timestamp and validity in the format (v=%8,
 *              t=%7)
 *
 *   @note      This class will look for the following optional configuration parameters:
 *                  saveFile    File to which the data will be saved (default
 *                  "RecordedMessages.csv")
 *
 *   @see        Communicator
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 25, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_RECORDERCOMMUNICATOR_H
#define PCOE_RECORDERCOMMUNICATOR_H

#include "Communicator.h"
#include "DataStore.h"

namespace PCOE {
    class RecorderCommunicator : public Communicator {
    public:
        /** @brief      Constructor for RecorderCommunicator - Called by the CommunicatorFactory
         *  @param      config  Reference to configuration map for the communicator
         *  @see        CommunicatorFactory
         **/
        RecorderCommunicator(const ConfigMap& config);

        ~RecorderCommunicator() override;

        inline void poll() override {}

        DataStore read() override;

        /** @brief      Publisher callback funciton- used to consume data from the prognostic
         *              framework
         *  @param      data        Reference to DataStore containing all the input data
         *  @param      progData    Output from each prognoser
         **/
        void write(AllData) override;

    private:
        bool init; ///< Has the recorderCommunicator been initialized
        std::FILE* theFile; ///< The file to be used for writing

        // Keys to control what is written
        bool writeOccur;
        bool writeProbOccur;
        bool writePredictions;
        bool writeSysTraj;
    };
}

#endif // PCOE_RECORDERCOMMUNICATOR_H
