/**  Prognostic Meta Abstract Class- Header
 *   @file      Prognostic Meta Abstract Class
 *   @ingroup   GPIC++
 *   @ingroup   ProgData
 *
 *   @brief     Prognostic Meta data abstract class definition- the class included is used to store, access, and update prognostic container meta information.
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

#ifndef PCOE_PROGMETA_H
#define PCOE_PROGMETA_H

#include <string>

namespace PCOE {
    // *------------------------*
    // |        CONSTANTS       |
    // *------------------------*

    const unsigned int NOW = 0;  ///< Representing the current point in time

    // *------------------------*
    // |         Classes        |
    // *------------------------*

    /** @class      ProgMeta
     *  @brief      Virtual class for storing, accessing, and updating ProgMeta Data (such as name and description)
     */
    class ProgMeta {
    public:
        ProgMeta();     ///< Constructor- Set Defaults

        std::string getName() const;    ///< Get Parameter Name

        std::string getDesc() const;    ///< Get Parameter Description

        void setName(const std::string & nameIn);   ///< Set Parameter Name

        void setDesc(const std::string & descIn);   ///< Set Parameter Description

        /** @brief      Set both name and description
         *  @param      nameIn      The parameter name
         *  @param      descIn      The parameter description
         **/
        void setMeta(const std::string & nameIn, const std::string & descIn);

    private:
        std::string name;
        std::string description;
    };
}

#endif  //PCOE_PROGMETA_H
