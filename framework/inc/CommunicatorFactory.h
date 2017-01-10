/**  Communicator Factory - Header
 *   @class     CommunicatorFactory CommunicatorFactory.h
 *   @ingroup   GPIC++
 *   @ingroup   Framework
 *
 *   @brief     Communicator Factory Class - Factory pattern
 *   Handles the creation of new communicators
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *   @pre       Prognostic Configuration File and Communicator Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */
#ifndef PCOE_COMMUNICATORFACTORY_H
#define PCOE_COMMUNICATORFACTORY_H

#include "Factory.h"
#include "Singleton.h"
#include "CommonCommunicator.h"

namespace PCOE {
    /**
     *  @class      PrognoserFactory
     *  @brief      Prognoser Factory Class - Factory pattern. Handles the creation of new prognosers
     *
     *  @see        Factory
     *  @note       Uses Factory template parent class.
     **/
    class CommunicatorFactory : public Factory<CommonCommunicator>,
        public Singleton<CommunicatorFactory> {
    private:
        friend class Singleton<CommunicatorFactory>;
        /**
         *  @brief      PrognoserFactory Constructor
         *  @note       Is protected to prevent users from creating a new PrognoserFactory
         *              Consistent with the singleton pattern
         **/
        CommunicatorFactory() {};
    };
}

#endif // PCOE_COMMUNICATORFACTORY_H
