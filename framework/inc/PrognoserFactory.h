/**  Prognoser Factory - Header
 *   @class     PrognoserFactory PrognoserFactory.h
 *   @ingroup   GPIC++
 *   @ingroup   Framework
 *
 *   @brief     Prognoser Factory Class - Factory pattern
 *   Handles the creation of new prognosers
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_PROGNOSERFACTORY_H
#define PCOE_PROGNOSERFACTORY_H

#include "Prognoser.h"
#include "ModelBasedPrognoser.h"
#include "Factory.h"
#include "Singleton.h"

namespace PCOE {
    /**
     *  @class      PrognoserFactory
     *  @brief      Prognoser Factory Class - Factory pattern. Handles the creation of new prognosers
     *
     *  @see        Factory
     *  @note       Uses Factory template parent class.
     **/
    class PrognoserFactory : public Factory<Prognoser>,
        public Singleton<PrognoserFactory> {
        friend class Singleton<PrognoserFactory>;  // Needed for singleton
    private:
        /**
         *  @brief      PrognoserFactory Constructor
         *  @note       Is protected to prevent users from creating a new PrognoserFactory
         *              Consistant with the singleton pattern
         **/
        PrognoserFactory() {
            Register("modelBasedPrognoser",
                PrognoserFactory::Create<ModelBasedPrognoser>);
        }
    };
}

#endif // PCOE_PROGNOSERFACTORY_H
