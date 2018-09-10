/**  UData Structure- Body
 *   @file      Uncertain Data Structure Classes
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *   @ingroup   UData
 *
 *   @brief     Uncertain Data Structure Classes - Classes used for storing,
 *              distributing, and manipulation data with uncertainty
 *
 *   @author    Chris Teubert <christopher.a.teubert@nasa.gov>
 *   @author    Jason Watkins <jason-watkins@outlook.com>
 *   @version   0.2.0
 *   @date      March 3, 2016
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#include <chrono>
#include <cmath>
#include <stdexcept>
#include <unordered_map>

#include "UData.h"

namespace PCOE {
    static UDataInterface* getInterface(const UType ut) {
        switch (ut) {
        case UType::Point:
            return new PointInterface();
        case UType::MeanSD:
            return new MeanSDInterface();
        case UType::MeanCovar:
            return new MeanCovarInterface();
        case UType::Samples:
            return new SamplesInterface();
        case UType::WSamples:
            return new WSamplesInterface();
        default:
            throw std::domain_error("Invalid UTYPE");
        }
    }

    //*------------------------------*
    //|        Constructors          |
    //*------------------------------*

    UData::UData() : UData(UType::Point) {}

    UData::UData(const UType ut)
        : m_data(),
          m_dist(DIST_UNKNOWN),
          m_interface(nullptr),
          m_npoints(1),
          m_uncertainty(ut),
          m_updated(),
          m_valid(false) {
        // Set Uncertainty. Initializes uType and interface
        uncertainty(ut);
    }

    UData::UData(const UData& other) : m_interface(nullptr) {
        m_dist = other.m_dist;
        m_data = other.m_data;
        m_uncertainty = other.m_uncertainty;
        m_npoints = other.m_npoints;
        m_valid = other.m_valid;
        m_updated = other.m_updated;
        uncertainty(m_uncertainty);
    }

    UData::UData(UData&& other) : UData() {
        swap(*this, other);
    }

    UData& UData::operator=(UData other) {
        swap(*this, other);
        return *this;
    }

    UData::~UData() {
        delete m_interface;
    }

    void swap(UData& a, UData& b) {
        using std::swap;
        swap(a.m_dist, b.m_dist);
        swap(a.m_data, b.m_data);
        swap(a.m_uncertainty, b.m_uncertainty);
        swap(a.m_npoints, b.m_npoints);
        swap(a.m_interface, b.m_interface);
        swap(a.m_valid, b.m_valid);
        swap(a.m_updated, b.m_updated);
    }

    //*------------------------------*
    //|         Comparison           |
    //*------------------------------*

    bool UData::operator==(const UData& other) const {
        // Check Type
        if (m_dist != other.m_dist || m_uncertainty != other.m_uncertainty ||
            m_npoints != other.m_npoints || m_valid != other.m_valid) {
            return false;
        }
        if (m_data.size() != other.m_data.size()) {
            return false;
        }
        if (m_data == other.m_data) {
            return true;
        }

        // The last two conditions should guarantee that by this point we have
        // non-empty vectors of the same size. Because vectors are intially
        // filled with NaNs, we need to do a final check to see if both
        // objects contain nothing except NaNs. This is necessary because NaN
        // does not compare equal to itself, causing the equality check above
        // to fail.
        for (std::vector<double>::size_type i = 0; i < m_data.size(); i++) {
            if (!std::isnan(m_data[i]) || !std::isnan(other.m_data[i])) {
                return false;
            }
        }
        return true;
    }

    bool UData::operator!=(const UData& other) const {
        return !(other == *this);
    }

    //*------------------------------*
    //|         Properties           |
    //*------------------------------*

    void UData::npoints(const size_type value) {
        m_npoints = value;
        m_interface->setup(m_npoints, m_data);
    }

    void UData::uncertainty(const UType value) {
        delete m_interface;

        m_uncertainty = value;
        m_interface = getInterface(value);
        m_interface->setup(m_npoints, m_data);
    }

    //*------------------------------*
    //|        Access Double         |
    //*------------------------------*

    UData::Proxy UData::operator[](const size_type key) {
        if (key >= m_data.size()) {
            throw std::out_of_range("UData: key out of range");
        }
        return Proxy(this, key);
    }

    UData::ConstProxy UData::operator[](const size_type key) const {
        if (key >= m_data.size()) {
            throw std::out_of_range("UData: key out of range");
        }
        return ConstProxy(this, key);
    }

    void UData::set(const size_type key, const double value) {
        using namespace std::chrono;
        m_interface->set(key, value, m_data);
        m_updated = static_cast<time_ticks>(
            time_point_cast<microseconds>(clock::now()).time_since_epoch().count());
        m_valid = true;
    }

    //*------------------------------*
    //|         Access Pair          |
    //*------------------------------*

    std::pair<double, double> UData::getPair(const size_type key) const {
        if (size() - key < 2) {
            throw std::out_of_range("Not enough elements after the specified key");
        }
        return m_interface->getPair(key, m_data);
    }

    void UData::setPair(const size_type key, const std::pair<double, double>& value) {
        using namespace std::chrono;
        m_interface->setPair(key, value, m_data);
        m_updated = static_cast<time_ticks>(
            time_point_cast<microseconds>(clock::now()).time_since_epoch().count());
        m_valid = true;
    }

    //*------------------------------*
    //|        Access Vector         |
    //*------------------------------*

    std::vector<double> UData::getVec(const size_type key) const {
        return m_interface->getVec(key, m_data);
    }

    void UData::setVec(const size_type key, const std::vector<double>& value) {
        using namespace std::chrono;
        m_interface->setVec(key, value, m_data);
        m_updated = static_cast<time_ticks>(
            time_point_cast<microseconds>(clock::now()).time_since_epoch().count());
        m_valid = true;
    }
}
