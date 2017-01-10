/**  UData Interfaces- Header
 *   @file      Uncertain Data Interface Classes
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *   @ingroup   UData
 *
 *   @brief     Uncertain Data Interface Classes - Classes used for controling the behavior of UData for different data types. Each type of uncertainty supported has its own interfaces that defines the behavior of that type. This file also includes functions and constants for identifying keys
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

#ifndef PCOE_UDATAINTERFACES_H
#define PCOE_UDATAINTERFACES_H

#include <vector>

namespace PCOE {
    // *------------------------*
    // |    Key Identifiers     |
    // *------------------------*
    //
    //  The below consts and functions aid with finding important index's for each data type

    /** @const      VALUE
     *  @brief      Used for UT_POINT data type to identify the point of interest
     *  @example    UData a = UData(UT_POINT);
     *              a[VALUE] = 4;
     **/
    const std::vector<double>::size_type VALUE = 0;

    /** @const      MEAN
     *  @brief      Used for MEANSD and MEANCOVAR data types to identify the mean
     *  @example    UData a = UData(MEANSD);
     *              a[MEAN] = 4;
     *  @example    UData b = UData(MEANCOVAR);
     *              b[MEAN] = 5;
     *
     *  @see        SD, DIST
     *  @see        COVAR
     **/
    const std::vector<double>::size_type MEAN = 0;

    /** @const      SD
     *  @brief      Used for MEANSD data type to identify the standard deviation
     *  @example    UData a = UData(MEANSD);
     *              a[SD] = 0.1;
     *
     *  @see        MEAN, DIST
     **/
    const std::vector<double>::size_type SD = 1;

    /** @const      COVAR
     *  @brief      Used for MEANCOVAR data type to identify the covariance with a specific point
     *  @example    UData a = UData(MEANCOVAR);
     *              a[COVAR(4)] = 0.1;  // Covariance with point 4
     *
     *  @see        MEAN
     **/
    inline std::vector<double>::size_type COVAR(const std::vector<double>::size_type num = 0) {
        return num + 1;
    }

    /** @const      SAMPLE
     *  @brief      Used for WSAMPLES data type to identify the value of a specific sample
     *  @example    UData a = UData(WSAMPLES);
     *              a[SAMPLE(4)] = 43;  // Value of Sample 4
     *
     *  @see        WEIGHT
     *  @see        PAIR
     **/
    inline std::vector<double>::size_type SAMPLE(const std::vector<double>::size_type num) {
        return num * 2;
    }


    /** @const      WEIGHT
     *  @brief      Used for WSAMPLES data type to identify the weight of a specific sample
     *  @example    UData a = UData(WSAMPLES);
     *              a[WEIGHT(4)] = 0.1;  // Weight of Sample 4
     *
     *  @see        SAMPLE
     *  @see        PAIR
     **/
    inline std::vector<double>::size_type WEIGHT(const std::vector<double>::size_type num) {
        return num * 2 + 1;
    }

    /** @const      PAIR
     *  @brief      Used for WSAMPLES data type to identify a specific sample (value and weight
     *  @example    UData a = UData(WSAMPLES);
     *              pair<double, double> b = a.getPair(PAIR(4));
     *              // Returns the weight and value for sample 4
     *
     *  @see        SAMPLE
     *  @see        WEIGHT
     **/
    inline std::vector<double>::size_type PAIR(const std::vector<double>::size_type num) {
        return num * 2;
    }

    /** @const      PVALUE
     *  @brief      Used for PERCENTILES data type to identify the value of a specific sample
     *  @example    UData a = UData(PERCENTILES);
     *              a[PVALUE(4)] = 43;  // Value of Sample 4
     *
     *  @see        PERCENTILE
     *  @see        PAIR
     **/
    inline std::vector<double>::size_type PVALUE(const std::vector<double>::size_type num) {
        return num * 2;
    }

    /** @const      PERCENTILE
     *  @brief      Used for PERCENTILES data type to identify the percentile of a specific sample
     *  @example    UData a = UData(PERCENTILES);
     *              a[PERCENTILE(4)] = 0.1;  // PErcentile of Sample 4
     *
     *  @see        PVALUE
     *  @see        PAIR
     **/
    inline std::vector<double>::size_type PERCENTILE(const std::vector<double>::size_type num) {
        return num * 2 + 1;
    }


    // *------------------------*
    // |   Abstract Interface   |
    // *------------------------*

    /** @class      UDataInterface
     *  @brief      An abstract class for uncertainty interfaces
     **/
    class UDataInterface {
    public:
        virtual ~UDataInterface() = default;

        /** setup
         *  @brief  Setup the data vector for a specific interface
         *
         *  @param  nPoints Number of points used (only relevent for certain interfaces)
         *  @param  data    The vector from the UData Object where the data is stored
         *
         *  @note   This method will be redifined by each interface
         **/
        virtual void setup(const std::vector<double>::size_type nPoints, std::vector<double> &data) = 0;

        /** get reference
         *  @brief  Get function that returns a reference to the desired element
         *
         *  @param  key     The index of the desired element
         *  @param  data    The vector from the UData Object where the data is stored
         *
         *  @return A reference to the value for that key
         *
         *  @note   This is usually used for the accessor [] so you can set the value- example data[1] = 1.5;
         **/
        virtual double & get(const std::vector<double>::size_type key, std::vector<double> &data);

        /** get value
         *  @brief  Get function that returns a value of the desired element
         *
         *  @param  key     The index of the desired element
         *  @param  data    The vector from the UData Object where the data is stored
         *
         *  @return The value of the data for that key
         **/
        virtual double get(const std::vector<double>::size_type key, const std::vector<double> &data) const;

        /** set value
         *  @brief  Set function for a specific value
         *
         *  @param  key     The index of the element to change
         *  @param  value   The value that the element will be set to
         *  @param  data    The vector from the UData Object where the data is stored
         **/
        virtual void set(const std::vector<double>::size_type key, const double value, std::vector<double> &data);

        /** get vector
         *  @brief  Get function that returns a vector of the values starting at the element corresponding to key
         *
         *  @param  key     The index of the starting element
         *  @param  data    The vector from the UData Object where the data is stored
         *
         *  @return A vector of the values starting at the one corresponding to key
         **/
        virtual std::vector<double> getVec(const std::vector<double>::size_type key, const std::vector<double> &data) const;

        /** set vector
         *  @brief  Set function for a vector- Will set value starting at the one corresponding to key
         *
         *  @param  key     The index of the element where the changes will begin
         *  @param  dataIn   The values that the elements will be set to
         *  @param  data    The vector from the UData Object where the data is stored
         **/
        virtual void setVec(const std::vector<double>::size_type key, const std::vector<double> dataIn, std::vector<double> &data);

        /** get pair
         *  @brief  Get function that returns a pair of the values starting at the element corresponding to key
         *
         *  @param  key     The index of the starting element
         *  @param  data    The vector from the UData Object where the data is stored
         *
         *  @return A pair of the values starting at the one corresponding to key
         **/
        virtual std::pair<double, double> getPair(const std::vector<double>::size_type key, const std::vector<double> &data) const;

        /** set pair
         *  @brief  Set function for a pair- Will set value starting at the one corresponding to key
         *
         *  @param  key     The index of the element where the changes will begin
         *  @param  value   The values that the elements will be set to
         *  @param  data    The vector from the UData Object where the data is stored
         **/
        virtual void setPair(const std::vector<double>::size_type key, const std::pair<double, double> value, std::vector<double> &data);
    };  // End Class definition for UInterface

    // *------------------------*
    // |   Derived Interfaces   |
    // *------------------------*

    /** @class      PointInterface
     *  @brief      A class defining the interface for UT_POINT uncertainty type
     *
     *  @note       PointInterface resizes data to size 1 (a single point)
     *              Uses VALUE const
     *
     *  @note       Data is of size 1. where data[0] is the single point
     *              Data = [Value]
     *
     *  @see        VALUE
     *  @see        UInterface
     **/
    class PointInterface final : public UDataInterface {
    public:
        /** setup
         *  @brief  Setup the data vector for a specific interface
         *
         *  @param  nPoints Number of points used (not used for UT_POINT uncertainty type)
         *  @param  data    The vector from the UData Object where the data is stored
         **/
        void setup(const std::vector<double>::size_type nPoints, std::vector<double> &data);
    };

    /** @class      MeanSDInterface
     *  @brief      A class defining the interface for MEANSD uncertainty type
     *
     *  @note       MeanSDInterface resizes data to size 2 (the mean and SD)
     *              data[0] is the mean, and data[1] is SD
     *              Uses MEAN and SD consts (defined above)
     *
     *  @note       Data is of size 2. With the following order
     *              Data = [Mean, SD]
     *
     *  @see        MEAN
     *  @see        SD
     *  @see        UInterface
     **/
    class MeanSDInterface final : public UDataInterface {
    public:
        /** setup
         *  @brief  Setup the data vector for a specific interface
         *
         *  @param  nPoints Number of points used (not used for MEANSD uncertainty type)
         *  @param  data    The vector from the UData Object where the data is stored
         **/
        void setup(const std::vector<double>::size_type nPoints, std::vector<double> &data);

        void set(const std::vector<double>::size_type key, const double value, std::vector<double> &data);
    };

    /** @class      MeanCovarInterface
     *  @brief      A class defining the interface for MEANCOVAR uncertainty type
     *
     *  @note       MeanCovarInterface resizes data to size 1+nPoints.
     *              data[0] is the mean, data[1] is the covariance with 0, data[2] is the covariance with 1, etc.
     *              Uses MEAN, COVAR() consts (defined above)
     *
     *  @note       Data is of size n+1. Where n is the number of values that covariance is calculated relative to.
     *              With the following order
     *              Data = [Mean, COVAR(0), Covar(1)...]
     *
     *  @see        MEAN
     *  @see        COVAR
     *  @see        UInterface
     **/
    class MeanCovarInterface final : public UDataInterface {
    public:
        /** setup
         *  @brief  Setup the data vector for a specific interface
         *
         *  @param  nPoints Number of points used- this is used to determine the size of the covariance vector
         *  @param  data    The vector from the UData Object where the data is stored
         **/
        void setup(const std::vector<double>::size_type nPoints, std::vector<double> &data);
    };

    /** @class      SamplesInterface
     *  @brief      A class defining the interface for SAMPLES (unweighted samples) uncertainty type
     *
     *  @note       Samples resizes data to size nPoints
     *              data[0] is for point 0, etc.
     *              Data = [UT_POINT(0), UT_POINT(1), ...]
     *
     *  @see        UInterface
     **/
    class SamplesInterface final : public UDataInterface {
    public:
        /** setup
         *  @brief  Setup the data vector for a specific interface
         *
         *  @param  nPoints Number of points used- this is used to determine the number of percentiles
         *  @param  data    The vector from the UData Object where the data is stored
         **/
        void setup(const std::vector<double>::size_type nPoints, std::vector<double> &data);
    };

    /** @class      WSamplesInterface
     *  @brief      A class defining the interface for WSAMPLES uncertainty type
     *
     *  @note       WSamples resizes data to size nPoints
     *              data[0] is the value for sample 0
     *              data[1] is the weight for sample 0
     *              data[2] is the value for sample 1...
     *              Uses SAMPLE, WEIGHT, PAIR const functions above
     *
     *  @see        SAMPLE
     *  @see        WEIGHT
     *  @see        PAIR
     *  @see        UInterface
     **/
    class WSamplesInterface final : public UDataInterface {
    public:
        /** setup
         *  @brief  Setup the data vector for a specific interface
         *
         *  @param  nPoints Number of points used- this is used to determine the number of weighted samples
         *  @param  data    The vector from the UData Object where the data is stored
         **/
        void setup(const std::vector<double>::size_type nPoints, std::vector<double> &data);
    };
}

#endif // PCOE_UDATAINTERFACES_H
