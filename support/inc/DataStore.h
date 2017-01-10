/**  DataStore - Header
 *   @class     DataStore DataStore.h
 *
 *   @brief     DataStore- Map for storing and accessing incoming data from communicators
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 21, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_DATASTORE_H
#define PCOE_DATASTORE_H

#include <unordered_map>
#include <string>
#include "Datum.h"

namespace PCOE {
    /** @class  DataStore
     *  @brief  Class for storing incoming data from communicators
     *  @see    Datum
     **/
    class DataStore : public std::unordered_map<std::string, Datum<double> > {
    };
    class DataStoreString : public std::unordered_map<std::string, Datum<std::string> > {
    };
    
}
#endif  // PCOE_DATASTORE_H
