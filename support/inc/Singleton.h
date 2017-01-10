/**  Singleton- Header
 *   @file      Singleton superclass
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Singleton Superclass- Inherit from this to make a class a singleton
 *
 *   @author  Chris Teubert <christopher.a.teubert@nasa.gov>
 *   @author  Jason Watkins <jason-watkins@outlook.com>
 *   @version 0.1.1
 *   @date    2016-06-22
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#ifndef PCOE_SINGLETON_H
#define PCOE_SINGLETON_H

namespace PCOE {
    /** @Brief Provides a base implementation of the singleton pattern.
     *
     *  @remarks The instance of the singleton is default constructed value stored
     *           by the class, so derived types must have a default constructor.
     *
     *  @author  Chris Teubert <christopher.a.teubert@nasa.gov>
     *  @author  Jason Watkins <jason-watkins@outlook.com>
     *  @version 0.1.1
     *  @date    2016-06-22
     */
    template <class Type>
    class Singleton {
    public:
        /** Deleted copy constructor to prevent copying of the instance. */
        Singleton(const Singleton& other) = delete;
        /** Deleted move constructor to prevent moving of the instance. */
        Singleton(Singleton&& other) = delete;
        /** Deleted assignment operator to prevent copying of the instance. */
        Singleton& operator=(Singleton other) = delete;

        /** Returns a reference to an instance of the singleton. */
        static Type& instance() {
            static Type t;
            return t;
        }

    protected:
        /** @brief Protected default constructor to prevent construction of the
         *         base Singleton class.
         */
        Singleton() = default;
    };
}
#endif // PCOE_SINGLETON_H
