/**  Empty Communicator - Header
 *   @class     EmptyCommunicator EmptyCommunicator.h
 *   @ingroup   GPIC++
 *   @ingroup   commCollection
 *
 *   @brief     Communicator Template
 *
 *   @see        CommonCommunicator
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 25, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */


#ifndef PCOE_EMPTYCOMMUNICATOR_H
#define PCOE_EMPTYCOMMUNICATOR_H

#include "CommonCommunicator.h"  ///< Parent Class

namespace PCOE {
    class EmptyCommunicator : public CommonCommunicator {
    public:
        /** @brief      Constructor for EmptyCommunicator - Called by the CommunicatorFactory
         *  @param      config  Reference to configuration map for the communicator
         *  @see        CommunicatorFactory
         **/
        EmptyCommunicator(const ConfigMap & config);

        /** @brief      Publisher callback funciton- used to consume data from the prognostic framework
         *  @param      data        Reference to DataStore containing all the input data
         *  @param      progData    Output from each prognoser
         **/
        void write(AllData data) override; // Comment out if not needed

        /** @brief      Subscriber callback function- used to introduce data into the prognostic framework
         *  @param      data  Reference to DataStore containing all the data
         *  @return     Updated Datastore with new data
         **/
        DataStore read() override; // Comment out if not needed
    };
}

#endif // PCOE_EMPTYCOMMUNICATOR_H
