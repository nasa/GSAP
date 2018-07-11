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
        // Key for loading config file
        static const std::string LOADING_KEY;
        // Optional key for gaussian STDDEV in loading config file
        static const std::string STDDEV_KEY;

        typedef enum UncertaintyType { NONE, GAUSSIAN } UType;

        /**
         * @brief Initializes the load estimator with values from the given
         *        config.
         *
         * @param configMap Configuration used to set up the load esitmator.
         **/
        ConstLoadEstimator(GSAPConfigMap& configMap);

        /**
         * @brief   Find if the load estimator is sample based (i.e., if you must use setNSamples)
         *          If it is sample based, its function requires the user call setNSamples
         *          If not, setNSamples will return a runtime_error
         *
         * @return  If the load estimator is sample based
         *
         * @see     setNSamples
         **/
        bool isSampleBased(){
            return true;
        }
        
        /**
         * @brief Set the number of samples for the loadEstimator
         *
         * @param nSamples The number of samples
         **/
        void setNSamples(const unsigned int nSamples);

        /**
         * @brief Get the mode of uncertainty being used.
         **/
        UType getUncertaintyMode();

        /**
         * @brief produces a load estimate for the given time.
         *
         * @param t      The number of seconds from the start to estimate a
         *               load for.
         * @param sample Sample id
         * @return Load estimate for time and sample
         **/
        LoadEstimate estimateLoad(const double t, const unsigned int sample);

    protected:
        // The profiles to for each sample are used when UncertType != NONE
        std::vector<LoadEstimate> profiles;
        LoadEstimate raw_profile;
        LoadEstimate stddev;

        UType uncertaintyMode = NONE;
    };
}
#endif // PCOE_CONST_LOAD_EST_H
