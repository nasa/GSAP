/**  Prognostic Events Class- Header
 *   @file      Prognostic Events Class Definition
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Events Class is a container for Prognostics Event objects
 *
 *   @note      In many cases there will be only one event (End of life).
 *
 *   @author    Marcus Postell
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: July 25, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#ifndef ProgEvents_h
#define ProgEvents_h

#include <unordered_map>

#include "PContainer.h"
#include "ProgEvent.h"

namespace PCOE {
    /** @class      ProgEvents
     *  @brief      Container class to store 'ProgEvents's, allows operation on
     *              all ProgEvent's at once
     **/
    class ProgEvents : public PContainer {
    public:
        using iterator = std::unordered_map<std::string, ProgEvent>::iterator;
        using const_iterator = std::unordered_map<std::string, ProgEvent>::const_iterator;

        ProgEvents() : PContainer() {} ///< Standard Constructor

        virtual ~ProgEvents() = default;

        void setNSamples(const unsigned int nSamples);

        /** @brief      Set the uncertaintyType to be used (from the UTYPE enum)
         *  @param      uncertType      The uncertainty type to be used
         *  @see        UTYPE
         **/
        void setUncertainty(const UType uncertType);

        /** @brief      Access operator for DataPoints by name
         *  @param      name Name of desired element
         *  @return     Desired element
         **/
        inline ProgEvent& operator[](const std::string& name) {
            return dataPts[name];
        }
        inline ProgEvent operator[](const std::string& name) const {
            return dataPts.at(name);
        }
        /** @brief      Add a new datapoint or progevent
         *  @param      name The name of the new element
         *  @param      description A description of the element
         **/
        void addNew(const std::string& name);

        /** @brief      Get a list of the labels used in the DataPoints object (Events,
         *SystemTrajectories, etc.)
         *  @return     Vector of labels
         **/
        std::vector<std::string> getLabels() const;

        /** @brief      Set the number of timestamps for which prognostic relevant prognostic data
         *will be recorded
         *  @param      nTimesIn        Number of timestamps
         **/
        void setNTimes(const unsigned int nTimesIn);

        /** @brief      Get the size of the container
         *  @return     The size of the container (number of elements
         **/
        unsigned int size() const;

        bool includes(const std::string& key) const;

        inline iterator begin() {
            return dataPts.begin();
        }

        inline iterator end() {
            return dataPts.end();
        }

        inline const_iterator cbegin() const {
            return dataPts.cbegin();
        }

        inline const_iterator cend() const {
            return dataPts.cend();
        }

    private:
        /// @brief      Map of saved elements (ProgEvent)
        std::unordered_map<std::string, ProgEvent> dataPts; // Note: Data duplication (name, name)
    };
}

#endif // PCOE_PROGEVENTS_H
