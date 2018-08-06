/**  Moving Average Load Estimator - Body
 *   @class     MovingAverageLoadEstimator
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Moving Average Load Estimator Class- uses a moving average to
 *              generate a future loading estimate similar to recent past
 *
 *   The purpose of this class is to handle the logic of estimating future loading in the case
 *   where future loading is assumed to be similar to past (e.g., when future loading is unknown.
 *   uses simple moving average
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

#include "Loading/MovingAverageLoadEstimator.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    const size_t MovingAverageLoadEstimator::DEFAULT_WINDOW_SIZE = 10;

    const std::string DEBUG_TAG = "Moving Average Load Estimator";
    // TODO(CT): Add noise/uncertainty

    static Log& log = Log::Instance();
    const std::string MovingAverageLoadEstimator::WINDOW_SIZE_KEY = "LoadEstimator.window_size";
    MovingAverageLoadEstimator::MovingAverageLoadEstimator(const ConfigMap& configMap) {
        log.WriteLine(LOG_INFO, DEBUG_TAG, "Configuring");
        if (configMap.hasKey(WINDOW_SIZE_KEY)) {
            windowSize = configMap.getUInt64(WINDOW_SIZE_KEY);
            windowSize = windowSize >= 0 ? windowSize : 0; // Handle size < 0
        }

        log.WriteLine(LOG_TRACE, DEBUG_TAG, "Completed configuration");
    }

    void MovingAverageLoadEstimator::addLoad(const LoadEstimate& newLoads) {
        log.WriteLine(LOG_DEBUG, DEBUG_TAG, "Received new load data... updating load estimate");

        if (estimateBuffer.size() < windowSize) {
            if (estimateBuffer.empty()) {
                // First estimate
                log.WriteLine(LOG_TRACE, DEBUG_TAG, "Buffer empty... initializing");
                lastEstimate = newLoads;
            }
            else {
                // Buffer not yet full
                log.WriteLine(LOG_TRACE, DEBUG_TAG, "Buffer not yet full... adding point");

                // Update estimates
                const auto& nEstimates = estimateBuffer.size();
                for (size_t i = 0; i < newLoads.size(); i++) {
                    // Iterate through dimensions of load
                    // estimate = ((lastEstimate*nPts + newPt)/newNPts)
                    // where lastEstimate*nPts = lastCumulative
                    // and (lastEstimate*nPts + newPt) = newCumulative
                    lastEstimate[i] =
                        (lastEstimate[i] * nEstimates + newLoads[i]) / (nEstimates + 1);
                }
            }

            // Add new data to buffer
            estimateBuffer.push_back(newLoads);
        }
        else {
            // Buffer full
            log.WriteLine(LOG_TRACE, DEBUG_TAG, "Buffer full... replacing oldest");

            // Update estimates
            for (size_t i = 0; i < lastEstimate.size(); i++) {
                // Iterate through dimensions of load
                // estimate = lastEstimate + (addedPoint - removedPoint)/windowSize
                lastEstimate[i] += (newLoads[i] - estimateBuffer[currentElement][i]) / windowSize;
            }

            // Add new Data to buffer
            estimateBuffer[currentElement] = newLoads;
            currentElement = (currentElement + 1) % windowSize;
        }
        log.WriteLine(LOG_TRACE, DEBUG_TAG, "Completed load estimate update");
    }

    LoadEstimate MovingAverageLoadEstimator::estimateLoad(const double, const unsigned int) {
        log.WriteLine(LOG_TRACE, DEBUG_TAG, "Sampling...");
        return lastEstimate;
    }
}
