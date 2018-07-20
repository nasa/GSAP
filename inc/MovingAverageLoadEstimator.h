/**  Moving Average Load Estimator - Header
 *   @class     MovingAverageLoadEstimator
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Moving Average Load Estimator Class- uses a moving average to
 *              generate a future loading estimate similar to recent past
 *
 *   The purpose of this class is to handle the logic of estimating future loading in the case
 *   where future loading is assumed to be similar to past (e.g., when future loading is unknown
 *
 *   @note      This relies on the user to always give the same size vector for loads
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoser Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: April, 2018
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */
#ifndef MovingAverageLoadEstimator_h
#define MovingAverageLoadEstimator_h

#include "LoadEstimator.h"

namespace PCOE {
    class MovingAverageLoadEstimator : public LoadEstimator {
    public:
        static const std::string
            WINDOW_SIZE_KEY; // Key for window size (number of steps in average window)
        static const size_t DEFAULT_WINDOW_SIZE;

        MovingAverageLoadEstimator() = default;

        /** MovingAverageLoadEstimator constructor.
         *  @param      configMap   Configuration map of configuration parameters in the prognoser
         *configuration file
         *
         *  The constructor configures the MovingAverageLoadEstimator
         **/
        MovingAverageLoadEstimator(GSAPConfigMap& configMap);

        /**
         * @brief   Find if the load estimator uses historical loading.
         *          If doesn't use historical loading, its function requires the user call addLoad
         *          If not, addLoad will return a runtime_error
         *
         * @return  If the load estimator uses historical loading
         *
         * @see     addLoad
         **/
        virtual bool usesHistoricalLoading() {
            return true;
        }

        /** @brief Set the load for that timestep
         *  @param  loadEstimate    Load estimate for the current timestep
         **/
        void addLoad(const LoadEstimate& newLoads);

        /** Estimate Load
         *  @param      t           Time for estimate (s from start)
         *  @param      sample      Sample id (unsigned int)
         *  @return     Load estimate for time and sample
         **/
        LoadEstimate estimateLoad(const double, const unsigned int);

    protected:
        // Estimate buffer for single sample
        std::vector<LoadEstimate> estimateBuffer;
        size_t currentElement = 0;
        size_t windowSize = DEFAULT_WINDOW_SIZE;
        LoadEstimate lastEstimate;
    };
}
#endif /* MovingAverageLoadEstimator_h */
