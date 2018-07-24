/**  Prognostic Data Structure- Header
 *   @file      Prognostic Data Structure Classes
 *   @ingroup   GPIC++
 *   @defgroup  ProgData     Prognostic Data Structure
 *
 *   @brief     Prognostic Data Structure Classes - Classes used for storing, distributing, and manipulation prognostic data
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: December 8, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_PROGDATA_H
#define PCOE_PROGDATA_H

#include <vector>
#include <string>
#include <map>
#include <cmath> // For NAN


#include "PContainer.h"
#include "DataPoints.h"
#include "ProgEvents.h"
namespace PCOE {
    // *------------------------*
    // |        Classes         |
    // *------------------------*

    /** @class      ProgData
     *  @brief      Top level class for storing all prognostic data
     */
    class ProgData {
    public:
        /// @brief      Basic constructor for ProgData
        ProgData();

        /** @brief      Constructor for ProgData with metadata
         *  @param      prognoserName Name of the prognoser used (ex: Battery)
         *  @param      componentName Name of the component prognosed (ex: Batt2a)
         *  @param      uniqueId Unique identifier for component prognosed, often a serial number. (ex: 143jjxv52332)
         **/
        ProgData(const std::string & prognoserName, const std::string & componentName, const std::string & uniqueId);

        /** @brief      Set the predictions to be considered by interval and number of predictions
         *  @param      interval        Interval between predictions
         *  @param      nPredictions    Number of predictions considered
         *
         *  @see        getTimes
         */
        void setPredictions(const double interval, const unsigned int nPredictions);

        /** @brief      Set the predictions to be considered from a vector
         *  @param      pred            A vector containing future prediction timestamps
         *
         *  @see        getTimes
         */
        void setPredictions(const std::vector<double> & pred);

        /** @brief      Get the times used for preditions
         *  @return     The times used for predictions
         *
         *  @note       These prediction times are set with setPredictions
         *  @see        setPrecitions
         **/
        const std::vector<double> & getTimes() const { return times; }

        /** @brief      Set the uncertaintyType to be used (from the UTYPE enum)
         *  @param      uncertType      The type of uncertainty to use
         *  @see        UTYPE
         **/
        void setUncertainty(const UType uncertType);

        /** @brief      Setup the number of samples to be used in the occurrence matrix
         *  @param      nSamples The number of samples in the occurrence matrix
         **/
        void setupOccurrence(const unsigned int nSamples);

        /// @brief      Get the name of the component
        const std::string & getComponentName() const { return componentName; }

        /// @brief      Get the name of the prognoser
        const std::string & getPrognoserName() const { return prognoserName; }

        /// @brief      Get the uniqueid of the component
        const std::string & getUniqueId() const { return uniqueId; }

        /// @brief      Set the name of the component
        void setComponentName(const std::string & compName) { componentName = compName; }

        /// @brief      Set the name of the prognoser
        void setPrognoserName(const std::string & progName) { prognoserName = progName; }

        /// @brief      Set the uniqueid of the component
        void setUniqueId(const std::string & uniqueIdIn) { uniqueId = uniqueIdIn; }

        // *-----------------------*
        // |        Events         |
        // *-----------------------*

        /** @brief      Add an event to the progDataStruct
         *  @param      name The name of the event
         *  @param      desc OPTIONAL: A description of the event
         **/
        inline void addEvent(const std::string & name, const std::string & desc = "") {
            events.addNew(name);  // Add to events object
        }

        /** @brief      Add multiple events to the progDataStruct
         *  @param      names A vector of the names of the events
         **/
        void addEvents(const std::vector<std::string> & names);

        /** @brief      Lookup the events considered (ex: EOL, ...)
         *  @return     A vector of the events considered
         **/
        inline std::vector<std::string> getEventNames() const {
            return events.getLabels();
        }

        ProgEvents events;      ///< The event objects and associated data

        // *-----------------------*
        // |  System Trajectories  |
        // *-----------------------*

        /** @brief      Add an SystemTrajectory to the progDataStruct
         *  @param      name The name of the SystemTrajectory
         *  @param      desc OPTIONAL: A description of the SystemTrajectory
         **/
        inline void addSystemTrajectory(const std::string & name) {
            sysTrajectories.addNew(name);
        }

        /** @brief      Add multiple SystemTrajectories to the progDataStruct
         *  @param      names A vector of the names of the SystemTrajectories
         **/
        void addSystemTrajectories(const std::vector<std::string> & names);

        /** @brief      Lookup the SystemTrajectories considered (ex: SOH, ...)
         *  @return     A vector of the SystemTrajectories considered
         **/
        inline std::vector<std::string> getSystemTrajectoryNames() const {
            return sysTrajectories.getLabels();
        }

        DataPoints sysTrajectories;     ///< The SystemTrajectory objects and associated data

        // *-----------------------*
        // |       Internals       |
        // *-----------------------*

        /** @brief      Add an internal param to the progDataStruct
         *  @param      name The name of the internal param
         *  @param      value OPTIONAL The value that the internal param will be set to
         **/
        inline void addInternal(const std::string & name, const double value = NAN) {
            internals[name] = value;
        }

        /** @brief      Add multiple internal params to the progDataStruct
         *  @param      names A vector of the names of the internal params
         **/
        void addInternals(const std::vector<std::string> & names);

        /** @brief      Lookup the Intrenal param names
         *  @return     A vector of the Intrenal param names
         **/
        std::vector<std::string> getInternalNames() const;

        std::map<std::string, double> internals;  ///< Map of the internal params

    private:
        std::string componentName;
        std::string prognoserName;
        std::string uniqueId;       ///< Meta data for progData

        std::vector<double> times;  ///< Timesconsidered
    };
}

#endif  // PCOE_PROGDATA_H
