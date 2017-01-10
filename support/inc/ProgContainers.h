/**  Prognostic Container Structure- Header
 *   @file      Prognostic Container Classes
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Container Structure Classes - Classes used for handling access to a group of ProgEvents or DataPoints
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 3, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_PROGCONTAINERS_H
#define PCOE_PROGCONTAINERS_H

#include <vector>
#include <string>

#include "ProgEvent.h"
#include "DataPoint.h"

namespace PCOE {
    // *------------------------*
    // |        Classes         |
    // *------------------------*

    /// @brief      Abstract parent class for ProgEvents, DataPoints
    class PContainer {
    public:
        PContainer();  ///< Constructor- Initializes parameters

        virtual ~PContainer() = default;

        /** @brief      Get the uncertaintyType used (from the UTYPE enum)
         *  @return     The uncertainty type used
         *  @see        UTYPE
         **/
        UType getUncertainty() const;

        /** @brief      Get the number of prediction times calculated
         *  @return     The number of prediction times calculated
         **/
        unsigned int getNTimes() const;

        /** @brief      Get the number of samples calculated
         *  @return     The samples calculated (for weighted/unweighted samples
         **/
        unsigned int getNSamples() const;

        /** @brief      Set the number of samples to be calculated
         *  @param      nSamples The samples to be calculated (for weighted/unweighted samples
         **/
        virtual void setNSamples(const unsigned int nSamples) = 0;

        /** @brief      Set the uncertaintyType to be used (from the UTYPE enum)
         *  @param      uncertType      The uncertainty type to be used
         *  @see        UTYPE
         **/
        virtual void setUncertainty(const UType uncertType) = 0;

        /** @brief      Add a new datapoint or progevent
         *  @param      name The name of the new element
         *  @param      description A description of the element
         **/
        virtual void addNew(const std::string & name, const std::string & description = "") = 0;

        /** @brief      Get a list of the labels used in the DataPoints object (Events, SystemTrajectories, etc.)
         *  @return     Vector of labels
         **/
        virtual std::vector<std::string> getLabels() const = 0;

        /** @brief      Set the number of timestamps for which prognostic relevant prognostic data will be recorded
         *  @param      nTimesIn        Number of timestamps
         **/
        virtual void setNTimes(const unsigned int nTimesIn) = 0;

        /** @brief      Get the size of the container
         *  @return     The size of the container (number of elements
         **/
        virtual unsigned int size() const = 0;

    protected:
        unsigned int nTimes;  ///< Number of prediction timestamps considered

        UType uType;  ///< Uncertainty Type of the elements in Data (from the UNCERTAINTYTYPE enum)

        unsigned int nSamples;  ///< The number of samples used internally with UData objects
    };  // End PContainer definition
}

#endif  // PCOE_PROGCONTAINERS_H
