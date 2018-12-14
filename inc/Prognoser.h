/**  Common Prognoser - Header
 *   @class     Prognoser Prognoser.h
 *   @ingroup   GPIC++
 *   @ingroup   ProgLib
 *
 *   @brief     Common Prognoser Class- Inherited by Component Prognosers
 *
 *   The purpose of this class is to:
 *      1. handle all the common operations (getting/sending data, saving results,
 *         handling prognoser history file, etc.) and,
 *      2. setup the structure of the component prognoser with virtual functions.
 *   As a design choice- we try to offload as much of the process to the common
 *   prognoser as possible to simplify integrating additional prognosers.
 *
 *   @author    Chris Teubert
 *   @version   1.2.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_Prognoser_H
#define PCOE_Prognoser_H

#include <map>

#include "ConfigMap.h"
#include "DataStore.h"
#include "Messages/MessageId.h"
#include "Predictors/Predictor.h" // SHOULDNT BE NEEDED FOR PROGNOSER- MOVE TO OWN FILE

namespace PCOE {
    class Prognoser {
    public:
        /** @brief      Common Prognoser Constructor
         *  @param      config Map of configuration parameters in the prognoser configuration
         *              file
         *
         *  The common prognoser constructor is called from the component prognoser
         *  constructor. It is passed in any configuration parameters in the prognoser
         *  configuration file
         */
		Prognoser(ConfigMap& ) {};
		
		Prognoser() = default;

        //*-------------------------------------------------------*
        //|     Virtual methods - Used by Component Prognoser     |
        //*-------------------------------------------------------*

        /** @brief     Prognostic Step
         *
         *             Preform model updates. This is done every step where there is
         *             enough data. This is a required method in any component
         *             prognoser
         */
        virtual Prediction step(std::map<MessageId, Datum<double> > data) = 0;
		
	protected:
		Log & log = Log::Instance();
    };
}
#endif // PCOE_Prognoser_H
