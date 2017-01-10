/**  Prognostic Data Point Class- Header
 *   @file      Prognostic Data Point Class Definition
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Data Point Class used to handle storage, access, and updating of the data associated with a specific prognostic data point (Example: state of health). This data includes the following:
 *              1. Meta Information from ProgMeta
 *              2. The value of the data point at different times with uncertainty
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
 **/

#ifndef PCOE_DATAPOINT_H
#define PCOE_DATAPOINT_H

#include "ProgMeta.h"
#include "UData.h"

namespace PCOE {
    // *------------------------*
    // |         Classes        |
    // *------------------------*

    /** @class      DataPoint
     *  @brief      Used to store data for future inputs and prognostic sysTrajectories. Encapsulates data of 2 dimensions (time & uncertainty)
     */
    class DataPoint : public ProgMeta {
        friend class DataPoints;        // Container Class for DataPoint

    public:
        /// @brief      Constructor- Set defaults
        DataPoint();

        /** @brief      Set the number of timestamps for which prognostic relevant prognostic data will be recorded
         *  @param      nTimesIn        Number of timestamps
         *
         *  @see        getNumTimes
         **/
        void setNumTimes(const unsigned int nTimesIn);

        /** @brief      Get the number of timestamps for which prognostic relevant prognostic data will be recorded
         *  @return     The number of timestamps
         *
         *  @see        setNumTimes
         **/
        unsigned int getNumTimes() const;

        /** @brief      Set the uncertaintyType to be used for all timestamps (from the UTYPE enum)
         *  @param      uncertType      The Uncertainty type to use
         *
         *  @see        getUncertainty
         *  @see        UTYPE
         **/
        void setUncertainty(const UType uncertType);

        /** @brief      Get the uncertaintyType used for all timestamps (from the UTYPE enum)
         *  @return     The Uncertainty type used
         *
         *  @see        setUncertainty
         *  @see        UTYPE
         **/
        UType getUncertainty() const;

        /** @brief      Access operator by number
         *  @param      index       Index of element
         *  @return     Desired element
         **/
        UData & operator[](const std::size_t index);

        /** @brief      Get the number of points considered
         *  @return     The number of points considered
         *
         *  @note       Most users will not use this
         *  @see        setNPoints();
         **/
        unsigned int getNPoints() const;

    private:
        /// @brief Data to be stored in datapoint- 2-d: Time x Uncertainty
        std::vector< UData > data;

        UType uType;            ///< Uncertainty Type of the elements in Data (from the UNCERTAINTYTYPE enum)

        /** @brief      Set the number of points considered
         *  @param      nPoints     The number of points considered
         *
         *  @note       Used by container class
         *  @see        getNPoints();
         **/
        void setNPoints(const unsigned int nPoints);

        unsigned int nPoints;   ///< The number of points considered
    };
}

#endif // PCOE_DATAPOINT_H
