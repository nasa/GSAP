/**  Random Communicator - Header
 *   @class     RandomCommunicator RandomCommunicator.h
 *   @ingroup   GPIC++
 *   @ingroup   commCollection
 *
 *   @brief     Random Communicator class- sets all input data to random number between 0-MaxRand with step StepSize
 *
 *   @note      This class will look for the following optional configuration parameters:
 *                  max     The maximum random number generated (default 10000)
 *                  step    The step size between possible random numbers (default 0.5)
 *
 *   @see        CommonCommunicator
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 21, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_RANDOMCOMMUNICATOR_H
#define PCOE_RANDOMCOMMUNICATOR_H

#include "CommonCommunicator.h"  ///< Parent Class

namespace PCOE {
    class RandomCommunicator : public CommonCommunicator {
    public:
        /** @brief      Constructor for RandomCommunicator - Called by the CommunicatorFactory
         *  @param      config  Reference to configuration map for the communicator
         *  @see        CommunicatorFactory
         **/
        RandomCommunicator(const ConfigMap & config);

        inline void poll() override { setRead(); }

        /** @brief      Subscriber callback function- used to introduce data into the prognostic framework
         *  @param      data  Reference to DataStore containing all the data
         *  @return     Updated Datastore with new data
         **/
        DataStore read() override;

        void write(AllData) override;

    private:
        unsigned long int maxRand;  ///< Maximum Random Number allowed
        double stepSize;            ///< Minimum Step between random numbers
        DataStore data;
    };
}

#endif // PCOE_RANDOMCOMMUNICATOR_H
