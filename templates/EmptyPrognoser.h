/**  Empty Prognoser - Header
 *   @class     EmptyPrognoser EmptyPrognoser.h
 *   @ingroup   GPIC++
 *   @ingroup   ProgLib
 *
 *   @brief     Empty Prognoser Class
 *
 *   The purpose of this class is to serve as a template for creating future prognosers
 *
 *   @author    Chris Teubert
 *   @version   1.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: November 11, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */
#ifndef PCOE_EMPTYPROGNOSER_H
#define PCOE_EMPTYPROGNOSER_H

#include "Prognoser.h"

namespace PCOE {
    class EmptyPrognoser : public Prognoser {
    public:
        /** @brief      Example Prognoser Constructor
         *  @param      config Map of config parameters from the prognoser config file
         */
        EmptyPrognoser(ConfigMap& config);

		/** @brief     Prognostic Monitor Step
		 *
		 *             Preform model updates. This is done every step where there is
		 *             enough data. This is a required method in any component
		 *             prognoser
		 */
		Prediction step(std::map<MessageId, Datum<double> > data) override;
    };
}

#endif // PCOE_EMPTYPROGNOSER_H
