/**  Prognostic Event Class- Header
 *   @file      Prognostic Event Class Definition
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Event Class used to handle storage, access, and updating of the data
 *              associated with a specific prognostic event (Example: End of life, end of
 *              discharge). This data includes the following:
 *              1. Meta Information from ProgMeta
 *              2. timeOfEvent with uncertainty: When the event occurs
 *              3. probMatrix with uncertainty: The probability that the event has occured at
 *                 different timestamps
 *              4. occurrenceMatrix: A collection of samples in a simulation for different
 *                 timestamps- if the event has occured for each sample. This is the most detailed
 *                 representation of uncertainty. probMatrix can be calculated from this
 *
 *   @note      In many cases there will be only one event (End of life).
 *   @note      In many cases timeOfEvent will be the only parameter that is used. probMatrix and
 *              occurrenceMatrix is for advanced users.
 *
 *   @author    Chris Teubert
 *   @author    Jason Watkins
 *   @version   1.1.0
 *
 *   Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *   Created: March 3, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#ifndef PCOE_PROGEVENT_H
#define PCOE_PROGEVENT_H

#include "ProgMeta.h"
#include "UData.h"

namespace PCOE {
    namespace TOE_Unit {
        const std::string S = "seconds";
        const std::string US = "microseconds";
        const std::string CYCLES = "cycles";
    }

    // *------------------------*
    // |         Classes        |
    // *------------------------*

    /** @class      ProgEvent
     *  @brief      Used to store data about a specific Prognostic Event (ex: EOL)
     */
    class ProgEvent : public ProgMeta {
        friend class ProgEvents; // Container class for ProgEvent

    public:
        /// @brief      Constructor - Set defaults
        ProgEvent();

        /// @brief Sets the time the event will occur.
        inline void setTOE(const UData& value) {
            timeOfEvent = value;
        }

        /// @brief Gets the time the event will occur.
        const UData& getTOE() const {
            return timeOfEvent;
        }

        /// @brief Gets the time the event will occur.
        UData& getTOE() {
            return timeOfEvent;
        }

        /** @brief Set the number of timestamps for which prognostic relevant prognostic data
         *         will be recorded
         *  @param timestampCount Number of timestamps
         **/
        void setNumTimes(const unsigned int timestampCount);

        /** @brief  Get the number of timestamps for which prognostic relevant prognostic data
         *          will be recorded
         *  @return Number of timestamps
         **/
        unsigned int getNumTimes() const;

        /** @brief      Set the uncertaintyType to be used (from the UTYPE enum)
         *  @param      uncertType      The Uncertainty type to use
         *  @see        UTYPE
         **/
        void setUncertainty(const UType uncertType);

        /** @brief      Get the uncertainty type used (from the UTYPE enum)
         *  @return     The uncertainty type used
         *  @see        UTYPE
         **/
        UType getUncertainty() const;

        std::string timeOfEventUnit; ///< Unit of the ToE


        /** @brief      Set the number of points in the events uncertainty representation (could be
         *samples or number of events)
         *  @param      sampleCount      The number of points
         *
         *  @note       This is handled by ProgEvents, the ProgEvent container class
         *  @see        ProgEvents
         **/
        void setNPoints(const unsigned int sampleCount);

        const UData &getEventState() const;

        UData& getEventState();

        void setEventState(const UData &eventState);

    private:
        UData eventState;

        /// @brief the time the event will occur.
        UData timeOfEvent;
    };
}

#endif // PCOE_PROGEVENT_H
