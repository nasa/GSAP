/**  Constant Load Estimator - Header
 *   @class     ConstLoadEstimator ConstLoadEstimator.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Constant Load Estimator Class- simulates constant loading
 *
 *   The purpose of this class is to handle the logic of estimating future loading in the case 
 *   where future loading is constant.
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

#ifndef PCOE_CONST_LOAD_EST_H
#define PCOE_CONST_LOAD_EST_H

#include "LoadEstimator.h"

namespace PCOE {
    class ConstLoadEstimator : public LoadEstimator {
     public:
        static const std::string LOADING_KEY; // Key for loading config file
        static const std::string STDDEV_KEY;  // Optional key for gaussian STDDEV in loading config file
        
        typedef enum UncertaintyType {
            NONE,
            GAUSSIAN
        } UType;

        /** ConstLoadEstimator constructor.
         *  @param      configMap   Configuration map of configuration parameters in the prognoser configuration
         *              file
         *
         *  The constructor configures the ConstLoadEstimator
         **/
        ConstLoadEstimator(GSAPConfigMap & configMap);
        
        /** Set the number of samples for the loadEstimator
         *  @param  nSamples    The number of samples
         **/
        void setNSamples(const unsigned int);
        
        /** Get the mode of uncertainty being used
         *  @return The uncertainty mode
         **/
        UType getUncertaintyMode();
        
        /** Estimate Load
         *  @param      t           Time for estimate (s from start)
         *  @param      sample      Sample id (unsigned int)
         *  @return     Load estimate for time and sample
         **/
        LoadEstimate estimateLoad(const double, const unsigned int);
        
     protected:
        std::vector<LoadEstimate> profiles; // The profiles to for each sample (used when UncertType != NONE)
        LoadEstimate raw_profile; // The raw profile
        LoadEstimate stddev;      // The std of raw_profile
        
        UType uncertaintyMode = NONE;   // The uncertainty mode
    };
}
#endif //PCOE_CONST_LOAD_EST_H
