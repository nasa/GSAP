/**  PrognosticsModel Factory - Header
 *   @class     PrognosticsModelFactory PrognosticsModelFactory.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     PrognosticsModel Factory Class - Factory pattern
 *   Handles the creation of new models
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 25, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_PROGNOSTICSMODELFACTORY_H
#define PCOE_PROGNOSTICSMODELFACTORY_H

#include "BatteryModel.h"
#include "Factory.h"
#include "PrognosticsModel.h"
#include "Singleton.h"

namespace PCOE {
    /**
     *  @class      PrognosticsModelFactory
     *  @brief      PrognosticsModel Factory Class. Handles the creation of new prognosers
     *
     *  @see        Factory
     *  @note       Uses Factory template parent class.
     **/
    class PrognosticsModelFactory : public Factory<PrognosticsModel>,
                                    public Singleton<PrognosticsModelFactory> {
        friend class Singleton<PrognosticsModelFactory>;

    private:
        /**
         *  @brief      PrognosticsModelFactory Constructor
         *  @note       Is protected to prevent users from creating a new PrognosticsModelFactory
         *              Consistant with the singleton pattern
         **/
        PrognosticsModelFactory() {
            Register("Battery", PrognosticsModelFactory::Create<BatteryModel>);
        };
    };
}

#endif // PCOE_PROGNOSTICSMODELFACTORY_H
