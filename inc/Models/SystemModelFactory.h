/**  Model Factory - Header
 *   @class     ModelFactory ModelFactory.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Model Factory Class - Factory pattern
 *   Handles the creation of new models
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 21, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_MODELFACTORY_H
#define PCOE_MODELFACTORY_H

#include "SystemModel.h"
#include "BatteryModel.h"
#include "Factory.h"
#include "Singleton.h"

namespace PCOE {
    /**
     *  @class      ModelFactory
     *  @brief      Model Factory Class - Factory pattern. Handles the creation of new models
     *
     *  @see        Factory
     *  @note       Uses Factory template parent class.
     **/
    class SystemModelFactory : public Factory<SystemModel>,
        public Singleton<SystemModelFactory> {
        friend class Singleton<SystemModelFactory>;
     private:
        /**
         *  @brief      ModelFactory Constructor
         *  @note       Is protected to prevent users from creating a new ModelFactory
         *              Consistant with the singleton pattern
         **/
        SystemModelFactory() {
            Register("Battery", SystemModelFactory::Create<BatteryModel>);
        };
    };
}

#endif // PCOE_MODELFACTORY_H
