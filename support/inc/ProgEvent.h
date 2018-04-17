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
 *   @version   0.1.0
 *
 *   Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *   Created: March 3, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
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

        /** @brief      Set the number of samples in the occurrence matrix (resizes matrix)
         *  @param      nSamples        The number of samples that the occurrence matrix will use
         **/
        void setNumOccurrenceSamples(const unsigned int nSamples);

        /** @brief      Get the number of samples in the occurrence matrix
         *  @return     The number of samples that the occurrence matrix uses
         **/
        unsigned int getNumOccurrenceSamples() const;

        /// @brief the time the event will occur.
        /// @deprecated getter/setter methods getTOE/setTOE should be used
        ///             instead. This field may be made private in future
        ///             releases.
        UData timeOfEvent;

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

        /// @brief      The probability of event occuring within the prediction horizon
        double eventProb;

        /// @brief      The probability the the event will occur at each time stamp
        std::vector<double> probMatrix; // 1-dim: Time

        /** @brief A two dimentional matrix storing wether the event has occured for each
         *         sample. The matrix has the dimensions time x unweighted samples so that:
         *         occurrenceMatrix[0][7]  represents wether the event has occured for sample 7
         *         at time 0.
         *  @see   setNumOccurrenceSamples
         *  @note  probMatrix can be calculated from this
         **/
        std::vector<std::vector<bool>> occurrenceMatrix; // 2-dim: Time x samples

        /** @brief Set the number of timestamps for which prognostic relevant prognostic data
         *         will be recorded
         *  @param nTimesIn Number of timestamps
         **/
        void setNumTimes(const unsigned int nTimesIn);

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
        bool isEventDriven; ///< Event is event driven

    private:
        unsigned int nSamples; ///< Number of samples in the occurrence matrix
        ///@todo(CT): IS THIS NECESSARY?

        /** @brief      Set the number of points in the events uncertainty representation (could be
         *samples or number of events)
         *  @param      nPoints      The number of points
         *
         *  @note       This is handled by ProgEvents, the ProgEvent container class
         *  @see        ProgEvents
         **/
        void setNPoints(const unsigned int nPoints);
    };
}

#endif // PCOE_PROGEVENT_H
