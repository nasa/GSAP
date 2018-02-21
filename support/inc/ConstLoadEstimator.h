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

        /** ConstLoadEstimator constructor.
         *  @param      configMap   Configuration map of configuration parameters in the prognoser configuration
         *              file
         *
         *  The constructor configures the ConstLoadEstimator
         **/
        ConstLoadEstimator(GSAPConfigMap & configMap);
        
        void setNSamples(const unsigned int);
        
        /** Estimate Load
         *  @param      t           Time for estimate (s from start)
         *  @param      sample      Sample id (unsigned int)
         *  @return     Load estimate for time and sample
         **/
        LoadEstimate estimateLoad(const double, const unsigned int);
        
     protected:
        std::vector<LoadEstimate> profiles; // Configured profile
        LoadEstimate raw_profile;
        LoadEstimate stddev;
        
        typedef enum UncertaintyType {
            NONE,
            GAUSSIAN
        } UType;
        
        UType uncertaintyMode = NONE;
    };
}
#endif //PCOE_CONST_LOAD_EST_H
