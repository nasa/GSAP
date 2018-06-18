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

#include "CommonPrognoser.h"

namespace PCOE {
    class EmptyPrognoser : public CommonPrognoser {
      public:
        /** @brief      Example Prognoser Constructor
         *  @param      config Map of config parameters from the prognoser config file
         */
        EmptyPrognoser(GSAPConfigMap & config);

        /** @brief     Prognostic Monitor Step
         *
         *             Preform model updates. This is done every step where there is
         *             enough data. This is a required method in any component
         *             prognoser
         */
        void step();

        //*------------------------------------------------------*
        //|          Optional Methods- Uncomment to use          |
        //*------------------------------------------------------*

        /** @brief     check the validity of any input (sensor) data.
         *
         *             This could be as simple as bound checks or a complicated
         *             analysis. By default this is not done- making this step
         *             optional in the component prognoser implementation
         */
         // void checkInputValidity() {}

         /** @brief     check if there is enough new data to preform prognosis
          *  @return    if there is enough data
          *
          *             Check if the data exists and is new enough to be used for
          *             prognosis. If false is returned prognostics steps will be
          *             skipped. By default this returns true- making this step
          *             optional in the component prognoser implementation
          */
          // bool isEnoughData() {return true;}

          /** @brief     check the validity of any prognostics results.
           *
           *             This could be as simple as bound checks or a complicated
           *             analysis. By default this a simple bounds test on timeToEvent
           *             - making this step optional in the component prognoser
           *             implementation
           *             Default implemented in CommonPrognoser
           */
           // void checkResultValidity();
    };
}

#endif // PCOE_EMPTYPROGNOSER_H
