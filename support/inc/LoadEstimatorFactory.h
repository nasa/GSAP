/**  LoadEstimator Factory - Header
 *   @class     LoadEstimatorFactory LoadEstimatorFactory.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     LoadEstimator Factory Class - Factory pattern
 *   Handles the creation of new load estimators
 *
 *   @author    Chris Teubert
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Christopher Teubert (christopher.a.teubert@nasa.gov)
 *      Created: February 14, 2018 (Happy Valentines Day!)
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_LOADESTIMATORFACTORY_H
#define PCOE_LOADESTIMATORFACTORY_H

#include "LoadEstimator.h"
#include "ConstLoadEstimator.h"
#include "MovingAverageLoadEstimator.h"
#include "Factory.h"
#include "Singleton.h"

namespace PCOE {
    /**
     *  @class      LoadEstimatorFactory
     *  @brief      Load Estimator Factory Class - Factory pattern. Handles the creation of new load estimators
     *
     *  @see        Factory
     *  @note       Uses Factory template parent class.
     **/
    class LoadEstimatorFactory : public Factory<LoadEstimator>,
    public Singleton<LoadEstimatorFactory> {
        friend class Singleton<LoadEstimatorFactory>;
    private:
        /**
         *  @brief      LoadEstimatorFactory Constructor
         *  @note       Is protected to prevent users from creating a new LoadEstimatorFactory
         *              Consistant with the singleton pattern
         **/
        LoadEstimatorFactory() {
            Register("const", LoadEstimatorFactory::Create<ConstLoadEstimator>);
            Register("movingAverage", LoadEstimatorFactory::Create<MovingAverageLoadEstimator>);
        };
    };
}

#endif // PCOE_LOADESTIMATORFACTORY_H
