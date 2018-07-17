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

#include "Model.h"
#include "ThreadSafeLog.h"
#include "UData.h"
#include <vector>

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

        virtual void initialize(const double t0,
                                const Model::state_type& x0,
                                const Model::input_type& u0) = 0;
        /** @brief	Step function for an Observer
         *  @param	newT New time
         *  @param	u Input vector at time newT
         *  @param	z Output vector at time newT
         **/
        virtual void step(const double newT,
                          const Model::input_type& u,
                          const Model::output_type& z) = 0;

        /** @brief Set model pointer
         * @param model given model pointer
         **/
        virtual void setModel(Model* model) = 0;

        // Accessors
        virtual const Model::state_type& getStateMean() const = 0;

        virtual std::vector<UData> getStateEstimate() const = 0;

        virtual const Model::output_type& getOutputMean() const = 0;

        inline double getTime() const {
            return m_t;
        }

        inline const Model::input_type& getInputs() const {
            return m_uOld;
        }

        inline bool isInitialized() const {
            return m_initialized;
        }

    protected:
        bool m_initialized; // whether or not initialized
        double m_t; // time
        Model::input_type m_uOld; // inputs at previous time step
        Model* pModel; // Pointer to system model.
                       // Observer does not own this memory.
        Log& log; ///> Logger (Defined in ThreadSafeLog.h)
    };
}
#endif // PCOE_OBSERVER_H
