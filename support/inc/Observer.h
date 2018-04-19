/**  Observer - Header
 *   @file      Observer Interface Class
 *   @ingroup   GPIC++
 *   @defgroup  Observer     Observer
 *
 *   @brief     Observer Interface Class - Class defining the Observer interface
 *
 *   @author    Matthew Daigle
 *   @version   1.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 5, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_OBSERVER_H
#define PCOE_OBSERVER_H

#include <vector>
#include "Model.h"
#include "UData.h"
#include "ThreadSafeLog.h"

namespace PCOE {
    class Observer {
    public:
        Observer() : m_initialized(false), m_t(0), pModel(NULL), log(Log::Instance()) {}
        /** @brief    Initialize function for an Observer
         *  @param    t0 Initial time
         *  @param    x0 Initial state vector
         *  @param    u0 Initial input vector
         **/

        virtual ~Observer() = default;

        virtual void initialize(const double t0, const std::vector<double> & x0,
            const std::vector<double> & u0) = 0;
        /** @brief	Step function for an Observer
         *  @param	newT New time
         *  @param	u Input vector at time newT
         *  @param	z Output vector at time newT
         **/
        virtual void step(const double newT, const std::vector<double> & u,
            const std::vector<double> & z) = 0;

        /** @brief Set model pointer
          * @param model given model pointer
        **/
        virtual void setModel(Model *model) = 0;

        // Accessors
        virtual const std::vector<double> & getStateMean() const = 0;
        virtual std::vector<UData> getStateEstimate() const = 0;
        virtual const std::vector<double> & getOutputMean() const = 0;
        double getTime() const;
        const std::vector<double> & getInputs() const;
        bool isInitialized() const;

    protected:
        bool m_initialized;             // whether or not initialized
        double m_t;                     // time
        std::vector<double> m_uOld;     // inputs at previous time step
        Model * pModel;                 // Pointer to system model.
                                        // Observer does not own this memory.
        Log &log;                       ///> Logger (Defined in ThreadSafeLog.h)
    };
}
#endif  // PCOE_OBSERVER_H
