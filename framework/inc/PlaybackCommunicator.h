/**  Playback Communicator - Header
 *   @class     PlaybackCommunicator PlaybackCommunicator.h
 *   @ingroup   GPIC++
 *   @ingroup   commCollection
 *
 *   @brief     Playback Communicator class- playback input data from a csv file
 *
 *   @note      This class will look for the following optional configuration parameters:
 *                  file        Name of the file that will be played back (default RecordedMessages.csv)
 *
 *   @see        CommonCommunicator
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 27, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_PLAYBACKCOMMUNICATOR_H
#define PCOE_PLAYBACKCOMMUNICATOR_H

#include <fstream>
#include <string>
#include <vector>

#include "CommonCommunicator.h"  ///< Parent Class
#include "CommunicatorFactory.h"

namespace PCOE {
    class PlaybackCommunicator : public CommonCommunicator {
     public:
        /** @brief      Constructor for PlaybackCommunicator - Called by the CommunicatorFactory
         *  @param      config  Reference to configuration map for the communicator
         *  @see        CommunicatorFactory
         **/
        PlaybackCommunicator(const ConfigMap &config);

        inline void poll() override { setRead(); }

        /** @brief      subscriber callback funciton- used to introduce data into the prognostic framework
         *  @param      data  Reference to DataStore containing all the data
         *  @return     Updated Datastore with new data
         **/
        DataStore read() override;

        /** @brief      Publisher callback funciton- used to consume data from the prognostic framework
         *  @param      data        Reference to DataStore containing all the input data
         *  @param      progData    Output from each prognoser
         **/
        void write(AllData) override;

        ~PlaybackCommunicator();

     private:
        std::ifstream playbackStream;       ///< The stream to be used as a header
        std::vector<std::string> header;    ///< The input parameters to be played back (from the header)
        char delim;                         ///< Delimiter
        bool timestampFromFile;
    };
}

#endif // PCOE_PLAYBACKCOMMUNICATOR_H
