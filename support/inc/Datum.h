/** Datum - Header
*   @class Datum Datum.h
*      
*   @brief Datum - Class for storing an individual point of data with timestamp- used with Datastodre
*   @author  Chris Teubert
*   @version 0.1.0
* 
*       Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
*       Created: July 15, 2016
*
*   @copyright Copyright (c) 2013-2016 United States Government as represented by
*    the Administrator of the National Aeronautics and Space Administration.
*    All Rights Reserved.
*/

#ifndef PCOE_DATUM_H
#define PCOE_DATUM_H

#include <unordered_map>
#include <string>
#include <chrono>  // Used for storing time (LastUpdated)
#include <math.h>  // For NAN

namespace PCOE {
    /** @class  Datum
     *  @brief  Class for storing an individual point of data with timestamp- used with Datastodre
     *  @see    DataStore
     **/
    template <class T>
    class Datum {
    public:
        typedef std::chrono::system_clock clock;
        typedef std::chrono::time_point<clock> time_point;
        typedef std::chrono::milliseconds::rep ms_rep;

        Datum();                    ///< Default Constructor
        Datum(const Datum<T> &);       ///< Copy Constructor
        Datum(const T value);  ///< Build from Value

        /** @brief      Function to copy using =
         *  @example    datum = otherDatum;
         **/
        Datum<T> & operator=(const Datum<T> & other);

        /** @brief      Function to set value using =
         *  @example    datum = 4.3;
         **/
        Datum<T> & operator=(const T data);

        operator T() const;    ///< Cast to double (same as .Get)

        void set(const T value);   ///< Set the value
        T get() const;             ///< Get the value

        /** getTime
         *  @brief      Get the time that it was last edited in milliseconds since epoch. This is
         *              updated to the current time whenever the value is changed using = or .set, or
         *              updated to the specified time when the setTime function is used.
         *  @return     The time that the datum was last updated in milliseconds since epoch
         *  @see        setTime
         **/
        ms_rep getTime() const;
        void setTime(time_point tp); ///< Set the time that it was last edited in milliseconds
        
        /** isSet
         *  @brief      Determine if value has been set
         *  @return     True if value has been set, false ohterwise
         **/
        bool isSet() const;

    private:
        T data;    ///< The data being stored
        time_point lastUpdated;  ///< When the data was last edited (0 if not set)
    };
    
    template <class T>
    Datum<T>::Datum() : data(NAN),
    lastUpdated(std::chrono::duration<int>(0)) {
    }
    
    template <class T>
    Datum<T>::Datum(const T value) {
        set(value);
    }
    
    template <class T>
    Datum<T>::Datum(const Datum<T> & other) {
        data = other.data;
        lastUpdated = other.lastUpdated;
    }
    
    template <class T>
    Datum<T> & Datum<T>::operator=(const Datum<T> & other) {
        data = other.data;
        lastUpdated = other.lastUpdated;
        return *this;
    }
    
    template <class T>
    Datum<T> & Datum<T>::operator=(const T dataIn) {
        data = dataIn;
        lastUpdated = std::chrono::system_clock::now();
        return *this;
    }
    
    template <class T>
    Datum<T>::operator T() const {
        return data;
    }
    
    template <class T>
    void Datum<T>::set(const T value) {
        data = value;
        lastUpdated = std::chrono::system_clock::now();
    }
    
    template <class T>
    T Datum<T>::get() const {
        return data;
    }
    
    template <class T>
    typename Datum<T>::ms_rep Datum<T>::getTime() const {
        using namespace std::chrono;
        return duration_cast<milliseconds>(lastUpdated.time_since_epoch()).count();
    }
    
    template <class T>
    void Datum<T>::setTime(typename Datum<T>::time_point lastUpdateIn) {
        lastUpdated = lastUpdateIn;
    }
    
    template <class T>
    bool Datum<T>::isSet() const {
        time_point zero(std::chrono::duration<int>(0));
        return lastUpdated != zero;
    }
}
#endif // PCOE_DATUM_H
