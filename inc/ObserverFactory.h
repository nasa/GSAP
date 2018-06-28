/**  Observer Factory - Header
 *   @class     ObserverFactory ObserverFactory.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Observer Factory Class - Factory pattern
 *   Handles the creation of new observers
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

#ifndef PCOE_OBSERVERFACTORY_H
#define PCOE_OBSERVERFACTORY_H

#include "Observer.h"
#include "Factory.h"
#include "Singleton.h"
#include "UnscentedKalmanFilter.h"
#include "ParticleFilter.h"

namespace PCOE {
    /**
     *  @class      ObserverFactory
     *  @brief      Singleton Observer Factory Class. Handles the creation of new prognosers
     *
     *  @see        Factory
     *  @note       Uses Factory template parent class.
     **/
    class ObserverFactory : public Factory<Observer>,
        public Singleton<ObserverFactory> {
        friend class Singleton<ObserverFactory>;

        /**
         *  @brief      ObserverFactory Constructor
         *  @note       Is protected to prevent users from creating a new ObserverFactory
         *              Consistant with the singleton pattern
         **/
        ObserverFactory() {
            Register("UKF", ObserverFactory::Create<UnscentedKalmanFilter>);
            Register("PF", ObserverFactory::Create<ParticleFilter>);
        }
    };
}

#endif // PCOE_OBSERVERFACTORY_H
