/**  UData Interfaces- Body
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

#include <cmath> // For NAN
#include <cfloat> // For 0 representation
#include <limits>

#include "UDataInterfaces.h"

namespace PCOE {
    // *------------------------*
    // |   Abstract Interface   |
    // *------------------------*

    double & UDataInterface::get(const std::vector<double>::size_type key, std::vector<double> &data) {
        return data.at(key);
    }

    double UDataInterface::get(const std::vector<double>::size_type key, const std::vector<double> &data) const {
        return data.at(key);
    }

    void UDataInterface::set(const std::vector<double>::size_type key, const double value, std::vector<double> &data) {
        data.at(key) = value;
    }

    std::vector<double> UDataInterface::getVec(const std::vector<double>::size_type key, const std::vector<double> &data) const {
        std::vector<double> returnValue;
        for (auto i = key; i < data.size(); i++) {
            returnValue.push_back(data[i]);
        }
        return returnValue;
    }


    void UDataInterface::setVec(const std::vector<double>::size_type key, const std::vector<double> dataIn, std::vector<double> &data) {
        std::vector<double>::const_iterator i2 = dataIn.cbegin();
        for (std::vector<double>::size_type i = key; i2 != dataIn.end() && i < data.size(); i++, i2++) {
            data[i] = *i2;
        }
    }

    std::pair<double, double> UDataInterface::getPair(const std::vector<double>::size_type key, const std::vector<double> &data) const {
        std::pair<double, double> result(NAN, NAN);
        if (data.size() > key) {
            result.first = data[key];
        }
        if (data.size() > key + 1) {
            result.second = data[key + 1];
        }

        return result;
    }

    void UDataInterface::setPair(const std::vector<double>::size_type key, const std::pair<double, double> dataIn, std::vector<double> &data) {
        if (data.size() > key) {
            data[key] = dataIn.first;
        }
        if (data.size() > key + 1) {
            data[key + 1] = dataIn.second;
        }

    }

    // *------------------------*
    // |   Derived Interfaces   |
    // *------------------------*

    void PointInterface::setup(const std::vector<double>::size_type, std::vector<double> &data) {
        data.resize(1, NAN);  // Resizes vector to size 1, filling with NAN
    }

    void MeanSDInterface::setup(const std::vector<double>::size_type, std::vector<double> &data) {
        data.resize(2, NAN);
    }

    void MeanSDInterface::set(const std::vector<double>::size_type key, const double value, std::vector<double> &data) {
        if (key == 1 && value < std::numeric_limits<double>::epsilon()) {
            UDataInterface::set(key, DBL_MIN, data);
        }
        else {
            UDataInterface::set(key, value, data);
        }
    }


    void MeanCovarInterface::setup(const std::vector<double>::size_type nPoints, std::vector<double> &data) {
        data.resize(nPoints + 1, NAN);
    }

    void SamplesInterface::setup(const std::vector<double>::size_type nPoints, std::vector<double> &data) {
        data.resize(nPoints, NAN);
    }

    void WSamplesInterface::setup(const std::vector<double>::size_type nPoints, std::vector<double> &data) {
        data.resize(nPoints * 2, NAN);
    }
}
