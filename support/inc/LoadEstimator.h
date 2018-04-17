/**  Load Estimator - Header
 *   @class     LoadEstimator ConstLoadEstimator.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Load Estimator Class- abstract parent class for load estimators
 *
 *   The purpose of this class is to define the interface for Load Estimators which handle the logic 
 *      of estimating future loading.
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: February, 2018
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_LOAD_ESTIMATOR_H
#define PCOE_LOAD_ESTIMATOR_H

#include <vector>

#include "GSAPConfigMap.h"
#include "PrognosticsModel.h"

namespace PCOE {
    typedef std::vector<double> LoadEstimate;
    const std::string LOAD_EST_SAMPLES_KEY = "LoadEstimator.samples";
    
    /// Load Estimator Abstract Parent Class
    class LoadEstimator {
     public:
        /** Set the number of samples for the loadEstimator
         *  @param  nSamples    The number of samples
         **/
        virtual void setNSamples(const unsigned int) {};
        
        /** @brief Set model pointer
         *  @param model given model pointer
         **/
        virtual void setModel(PrognosticsModel *model) {
            pModel = model;
        }
        
        /** @brief Set the load for that timestep
         *  @param  loadEstimate    Load estimate for the current timestep
         **/
        virtual void addLoad(const LoadEstimate & ) {}
        
        /** Estimate Load
         *  @param      t           Time for estimate (s from start)
         *  @param      sample      Sample id (unsigned int)
         *  @return     Load estimate for time and sample
         **/
        virtual LoadEstimate estimateLoad(const double t, const unsigned int sample) = 0;
        
     protected:
        PrognosticsModel * pModel;  // model used for prediction
    };
}
#endif //PCOE_LOAD_ESTIMATOR_H
