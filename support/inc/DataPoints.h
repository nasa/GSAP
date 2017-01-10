//
//  DataPoints.h
//  unit
//
//  Created by Postell, Marcus J. (ARC-TI)[ARC-TI-Collaborative] on 7/25/16.
//  Copyright © 2016 NASA. All rights reserved.
//

#ifndef PCOE_DATAPOINTS_H
#define PCOE_DATAPOINTS_H

#include <unordered_map>

#include "DataPoint.h"
#include "ProgEvents.h"

namespace PCOE {
    /** @class      DataPoints
     *  @brief      Container class to store 'DataPoint's, allows operation on all DataPoint's at once
     **/
    class DataPoints : public PContainer {
    public:
        using iterator = std::unordered_map<std::string, DataPoint>::iterator;
        using const_iterator = std::unordered_map<std::string, DataPoint>::const_iterator;

        DataPoints() : PContainer() {}
        
        virtual ~DataPoints() = default;
        
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
        inline DataPoint& operator[](const std::string & name) {
            return dataPts[name];
        }
        inline DataPoint operator[](const std::string & name) const {
            return dataPts.at(name);
        }
        
        /** @brief      Add a new datapoint or progevent
         *  @param      name The name of the new element
         *  @param      description A description of the element
         **/
        void addNew(const std::string & name, const std::string & description);
        
        /** @brief      Get a list of the labels used in the DataPoints object (Events, SystemTrajectories, etc.)
         *  @return     Vector of labels
         **/
        std::vector<std::string> getLabels() const;
        
        /** @brief      Set the number of timestamps for which prognostic relevant prognostic data will be recorded
         *  @param      nTimesIn        Number of timestamps
         **/
        void setNTimes(const unsigned int nTimesIn);
        
        /** @brief      Get the size of the container
         *  @return     The size of the container (number of elements
         **/
        unsigned int size() const;
        
        bool includes(const std::string &key) const;

        inline iterator begin() { return dataPts.begin(); }

        inline iterator end() { return dataPts.end(); }

        inline const_iterator cbegin() const { return dataPts.cbegin(); }

        inline const_iterator cend() const { return dataPts.cend(); }
        
    private:
        /// @brief      Map of saved elements (Datapoint
        std::unordered_map<std::string, DataPoint> dataPts;
    };
}



#endif /// PCOE_DATAPOINTS_H
