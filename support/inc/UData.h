/**  UData Structure- Header
 *   @file      Uncertain Data Structure Classes
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *   @defgroup  UData   Uncertain Data
 *
 *   @brief     Uncertain Data Structure Classes - Classes used for storing,
 *              distributing, and manipulation data with uncertainty
 *
 *   @author    Chris Teubert <christopher.a.teubert@nasa.gov>
 *   @author    Jason Watkins <jason-watkins@outlook.com>
 *   @version   0.2.0
 *   @date      March 3, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#ifndef PCOE_UDATA_H
#define PCOE_UDATA_H

#include <chrono> // For last updated
#include <cmath>
#include <vector>

#include "UDataInterfaces.h" // The interfaces

using namespace PCOE;

namespace PCOE {
    // *------------------------*
    // |        CONSTANTS       |
    // *------------------------*

    /** @enum       UTYPE
     *  @brief      Uncertainty Type- The set of uncertainty types supported by ProgDatas
     */
    enum class UType {
        Point,
        MeanSD,
        MeanCovar,
        Samples,
        WSamples,
        Percentiles = WSamples,
        WeightedSamples = WSamples,
        UnweightedSamples = Samples,
    };

    /** @enum       DIST
     *  @brief      Distribution Type
     */
    enum DIST_TYPE {
        DIST_UNKNOWN,
        DIST_GAUSSIAN,
        DIST_NORMAL,
        DIST_EXPONENTIAL,
        DIST_UNIFORM,
    };

    // *------------------------*
    // |        Classes         |
    // *------------------------*

    class UDatum;

    /** @class      UData
     *  @brief      A class for representing and manipulating data with uncertainty
     *  @see        UTYPE
     **/
    class UData {
    public:
        //*------------------------------*
        //|            Types             |
        //*------------------------------*
        struct Proxy;
        struct ConstProxy;
        using clock = std::chrono::steady_clock;
        using time_point = clock::time_point;
        using difference_type = std::vector<double>::difference_type;
        using size_type = std::vector<double>::size_type;
        class iterator;
        using const_iterator = std::vector<double>::const_iterator;
        using reverse_iterator = std::vector<double>::reverse_iterator;
        using const_reverse_iterator = std::vector<double>::const_reverse_iterator;

        //*------------------------------*
        //|        Constructors          |
        //*------------------------------*

        /** @brief Constructs a new instance of UData using the UT_POINT
         *         uncertainty type.
         **/
        UData();

        /** @brief Constructs a new instance of UData using the specified
         *         uncertainty type.
         *
         *  @param uType The uncertainty type to use.
         **/
        explicit UData(const UType uType);

        /** @brief Constructs a new instance of UData with the same properties
         *         as the given UData object.
         *
         *  @param other The UData object to copy.
         **/
        UData(const UData& other);

        /** @brief Constructs a new instance of UData by taking the data of the
         *         given UData object.
         *
         *  @param other The UData object to take data from.
         **/
        UData(UData&& other);

        /** @brief Copies the data of another UData object, overwriting the
         *         current UData object.
         *
         *  @param other The UData object to copy.
         *  @returns     A reference to the current UData object.
         **/
        UData& operator=(UData other);

        /** @brief Releases resources used by the current UData object. */
        ~UData();

        /** @brief Swaps the data contained by two UData objects.
         *
         *  @param a The first object to swap.
         *  @param b The second object to swap.
         **/
        friend void swap(UData& a, UData& b);

        //*------------------------------*
        //|         Comparison           |
        //*------------------------------*

        /** @brief      Uncertain data equals operator
         *  @param      other       The other UData type to be compared
         *  @return     If the two UData types are equal
         *
         *  @example    if (u == s) // u and s are of type UData
         **/
        bool operator==(const UData& other) const;

        /** @brief      Uncertain data not equals operator
         *  @param      other       The other UData type to be compared
         *  @return     If the two UData types are not equal
         *
         *  @example    if (u != s) // u and s are of type UData
         **/
        bool operator!=(const UData& other) const;

        //*------------------------------*
        //|         Properties           |
        //*------------------------------*

        /** @brief Sets the distribution type of the current object. */
        inline void dist(const DIST_TYPE value) {
            m_dist = value;
        }

        /** @brief Gets the distribution type of the current object. */
        inline DIST_TYPE dist() {
            return m_dist;
        }

        /** @brief Sets the number of points. */
        void npoints(const size_type value);

        /** @brief Gets the number of points. */
        inline size_type npoints() const {
            return m_npoints;
        }

        /** @brief Gets the size of the data vector. */
        inline std::vector<double>::size_type size() const {
            return m_data.size();
        }

        /** @brief Set the type of uncertainty to be used. */
        void uncertainty(const UType value);

        /** @brief Gets the type of uncertainty to be used. */
        inline UType uncertainty() const {
            return m_uncertainty;
        }

        /** @brief Gets the time that the current object was last updated. */
        inline time_point updated() const {
            return m_updated;
        }

        /** @brief Marks the current UData as invalid. */
        inline void invalidate() {
            m_valid = false;
        }

        /** @brief Gets the validity of the current object. */
        inline bool valid() const {
            return m_valid && !std::isnan(m_interface->get(0, m_data));
        }

        /** @brief Gets the validity of the current object. */
        inline bool operator!() {
            return !valid();
        }

        //*------------------------------*
        //|          Iterators           |
        //*------------------------------*

        /** @brief Returns an iterator to the first data element. */
        inline iterator begin() noexcept {
            return iterator(this, m_data.begin());
        }

        /** @brief Returns an iterator to the first data element. */
        inline const_iterator begin() const noexcept {
            return m_data.begin();
        }

        /** @brief Returns an iterator to the first data element. */
        inline const_iterator cbegin() const noexcept {
            return m_data.cbegin();
        }

        /** @brief Returns an iterator to the element following the last data
         *         element.
         **/
        inline iterator end() noexcept {
            return iterator(this, m_data.end());
        }

        /** @brief Returns an iterator to the element following the last data
         *         element.
         **/
        inline const_iterator end() const noexcept {
            return m_data.end();
        }

        /** @brief Returns an iterator to the element following the last data
         *         element.
         **/
        inline const_iterator cend() const noexcept {
            return m_data.cend();
        }

        /** @brief Returns a reverse iterator to the last data element. */
        inline reverse_iterator rbegin() noexcept {
            return m_data.rbegin();
        }

        /** @brief Returns a reverse iterator to the last data element. */
        inline const_reverse_iterator rbegin() const noexcept {
            return m_data.rbegin();
        }

        /** @brief Returns a reverse iterator to the last data element. */
        inline const_reverse_iterator crbegin() const noexcept {
            return m_data.crbegin();
        }

        /** @brief Returns a reverse iterator to the element before the first
         *         data element.
         **/
        inline reverse_iterator rend() noexcept {
            return m_data.rend();
        }

        /** @brief Returns a reverse iterator to the element before the first
         *         data element.
         **/
        inline const_reverse_iterator rend() const noexcept {
            return m_data.rend();
        }

        /** @brief Returns a reverse iterator to the element before the first
         *         data element.
         **/
        inline const_reverse_iterator crend() const noexcept {
            return m_data.crend();
        }

        //*------------------------------*
        //|        Access Double         |
        //*------------------------------*

        /** @brief      Uncertain data element access operator
         *  @param      key     Access a specific element of the Uncertain Data Vector
         *
         *  @example    dataVec[1] = 1.2;
         *  @example    double tmp = dataVec[3];
         **/
        Proxy operator[](const size_type key);
        ConstProxy operator[](const size_type key) const;

        /** @brief Get a value in the current object's data.
         *
         *  @param key The index of the data elmeent to get. Should be a value
         *             obtained from one of the helper methods in the
         *             UDataInterfaces.h header. If this parameter is omitted,
         *             the first data element is returned.
         *  @returns   The requested data element.
         **/
        inline double get(const size_type key = 0) const {
            return m_interface->get(key, m_data);
        }

        /** @brief Set a value in the current object's data.
         *
         *  @param key   The index of the data elmeent to set. Should be a
         *               value obtained from one of the helper methods in the
         *               UDataInterfaces.h header.
         *  @param value The value to set.
         **/
        void set(const size_type key, const double value);

        /** @brief Set the first element in the current object's data.
         *
         *  @param value The value to set.
         **/
        inline void set(const double value) {
            set(0, value);
        }

        //*------------------------------*
        //|         Access Pair          |
        //*------------------------------*

        /** @brief Get a pair of values in the current object's data.
         *
         *  @param key The index of the first data elmeent to get. Should be a
         *             value obtained from one of the helper methods in the
         *             UDataInterfaces.h header. If this parameter is omitted,
         *             a pair starting at the first element is returned.
         *  @returns   The requested data element pair.
         **/
        std::pair<double, double> getPair(const size_type key = 0) const;

        /** @brief Set a pair of values in the current object's data.
         *
         *  @param key   The index of the first elmeent to set. Should be a
         *               value obtained from one of the helper methods in the
         *               UDataInterfaces.h header.
         *  @param value The values to set.
         **/
        void setPair(const size_type key, const std::pair<double, double>& value);

        /** @brief Set a pair of values in the current object's data, starting
         *         at the first value.
         *
         *  @param value The values to set.
         **/
        inline void setPair(const std::pair<double, double>& value) {
            setPair(0, value);
        }

        //*------------------------------*
        //|        Access Vector         |
        //*------------------------------*

        /** @brief Get values in the current object's data.
         *
         *  @param key The index of the first data elmeent to get. Should be a
         *             value obtained from one of the helper methods in the
         *             UDataInterfaces.h header. If this parameter is omitted,
         *             data starting at the first element is returned.
         *  @returns   The requested data elements.
         **/
        std::vector<double> getVec(const size_type key = 0) const;

        /** @brief Set values in the current object's data.
         *
         *  @param key   The index of the first elmeent to set. Should be a
         *               value obtained from one of the helper methods in the
         *               UDataInterfaces.h header.
         *  @param value The values to set.
         **/
        void setVec(const size_type key, const std::vector<double>& value);

        /** @brief Set values in the current object's data, starting at the
         *         first value.
         *
         *  @param value The values to set.
         **/
        inline void setVec(const std::vector<double>& value) {
            setVec(0, value);
        }

        /** @brief Set values in the current object's data.
         *
         *  @remarks Same as setVec(key, value)
         *
         *  @param key   The index of the first elmeent to set. Should be a
         *               value obtained from one of the helper methods in the
         *               UDataInterfaces.h header.
         *  @param value The values to set.
         **/
        inline void set(const size_type key, const std::vector<double>& value) {
            setVec(key, value);
        }

        /** @brief Set values in the current object's data, starting at the
         *         first value.
         *
         *  @remarks Same as setVec(value)
         *
         *  @param value The values to set.
         **/
        inline void set(const std::vector<double>& value) {
            setVec(value);
        }

        /** @brief Set a pair of values in the current object's data.
         *
         *  @remarks Same as setPair(key, value)
         *
         *  @param key   The index of the first elmeent to set. Should be a
         *               value obtained from one of the helper methods in the
         *               UDataInterfaces.h header.
         *  @param value The values to set.
         **/
        inline void set(const size_type key, const std::pair<double, double>& value) {
            setPair(key, value);
        }

        /** @brief Set a pair of values in the current object's data, starting
         *         at the first value.
         *
         *  @remarks Same as setPair(value)
         *
         *  @param value The values to set.
         **/
        inline void set(const std::pair<double, double>& value) {
            setPair(value);
        }

    private:
        //*------------------------------*
        //|             Data             |
        //*------------------------------*
        std::vector<double> m_data;
        DIST_TYPE m_dist;
        UDataInterface* m_interface;
        size_type m_npoints;
        UType m_uncertainty;
        time_point m_updated;
        bool m_valid;

    public:
        struct ConstProxy {
            friend class UData;

        public:
            inline operator double() {
                return data->get(i);
            }
            inline operator std::vector<double>() {
                return data->getVec(i);
            }
            inline operator std::pair<double, double>() {
                return data->getPair(i);
            }

        protected:
            ConstProxy(const UData* source, size_type index) : data(source), i(index) {}

            const UData* data;
            size_type i;
        };

        struct Proxy final : public ConstProxy {
            friend class UData;

        public:
            inline double operator=(double value) {
                mut_data->set(i, value);
                return value;
            }
            inline std::vector<double> operator=(std::vector<double> value) {
                mut_data->set(i, value);
                return value;
            }
            inline std::pair<double, double> operator=(std::pair<double, double> value) {
                mut_data->set(i, value);
                return value;
            }

        protected:
            Proxy(UData* source, size_type index) : ConstProxy(source, index), mut_data(source) {}

            UData* mut_data;
        };

        class iterator {
            friend class UData;

        public:
            using difference_type = std::vector<double>::iterator::difference_type;

            inline Proxy operator*() {
                return Proxy(source, static_cast<size_type>(base - source->m_data.begin()));
            }

            inline iterator& operator++() {
                ++base;
                return *this;
            }

            inline iterator operator++(int) {
                iterator tmp = *this;
                ++tmp;
                return tmp;
            }

            inline iterator& operator--() {
                --base;
                return *this;
            }

            inline iterator operator--(int) {
                iterator tmp = *this;
                --tmp;
                return tmp;
            }

            inline iterator& operator+=(difference_type n) {
                base += n;
                return *this;
            }

            inline iterator operator+(difference_type n) {
                return iterator(source, base + n);
            }

            inline friend iterator operator+(difference_type n, const iterator& i) {
                return iterator(i.source, i.base + n);
            }

            inline iterator& operator-=(difference_type n) {
                base -= n;
                return *this;
            }

            inline iterator operator-(difference_type n) {
                return iterator(source, base - n);
            }

            inline friend iterator operator-(difference_type n, const iterator& i) {
                return iterator(i.source, i.base - n);
            }

            inline difference_type operator-(const iterator& it) {
                return it.base - base;
            }

            inline Proxy operator[](difference_type n) {
                return *(*this + n);
            }

            inline bool operator==(const iterator& i) {
                return base == i.base;
            }

            inline bool operator!=(const iterator& i) {
                return !(*this == i);
            }

            inline bool operator<(const iterator& i) {
                return base < i.base;
            }

            inline bool operator<=(const iterator& i) {
                return base <= i.base;
            }

            inline bool operator>(const iterator& i) {
                return base > i.base;
            }

            inline bool operator>=(const iterator& i) {
                return base >= i.base;
            }

        private:
            iterator(UData* s, std::vector<double>::iterator b) : source(s), base(b) {}

            UData* source;
            std::vector<double>::iterator base;
        };
    };
}

#endif // PCOE_UDATA_H
