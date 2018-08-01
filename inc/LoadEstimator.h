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

#include <stdexcept>
#include <vector>

#include "PrognosticsModel.h"

namespace PCOE {
    typedef std::vector<double> LoadEstimate;
    typedef LoadEstimate LoadMeasurement;
    const std::string LOAD_EST_SAMPLES_KEY = "LoadEstimator.samples";

    /**
     * Abstract base class that defines the interface for load estimators.
     **/
    class LoadEstimator {
    public:
        /**
         * @brief   Find if the load estimator is sample based (i.e., if you must use setNSamples)
         *          If it is sample based, its function requires the user call setNSamples
         *          If not, setNSamples will return a runtime_error
         *
         * @return  If the load estimator is sample based
         *
         * @see     setNSamples
         **/
        virtual bool isSampleBased() {
            return false;
        }

        /**
         * @brief   Set the number of samples for the loadEstimator, if the load estimator is
         *          samplebased. Use isSampleBased to see if LoadEstimator is sample based
         *
         * @param   nSamples The number of samples
         *
         * @note    Will throw a runtime_error if not sample based (isSampleBased returns false)
         * @see     isSampleBased
         **/
        virtual void setNSamples(const unsigned int nSamples);

        /**
         * @brief Set model pointer
         * @param value given model pointer
         **/
        virtual void setModel(PrognosticsModel* value) {
            this->model = value;
        }

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
            return false;
        }

        /**
         * @brief   Set the load for that timestep, if the load estimator uses historical loading.
         *          Call usesHistoricalLoading to check if historical loading is used.
         * @param   loadMeasurement    Load measurement for the current timestep
         *
         * @note    Will throw a runtime_error if not using historical loading
         *          (i.e.,usesHistoricalLoading returns false)
         * @see     usesHistoricalLoading
         **/
        virtual void addLoad(const LoadMeasurement&) {
            throw std::runtime_error("Not supported");
        };

        /** Estimate Load
         *  @param      t           Time for estimate (s from start)
         *  @param      sample      Sample id (unsigned int)
         *  @return     Load estimate for time and sample
         **/
        virtual LoadEstimate estimateLoad(const double t, const unsigned int sample) = 0;

    protected:
        PrognosticsModel* model;
    };
}
#endif // PCOE_LOAD_ESTIMATOR_H
