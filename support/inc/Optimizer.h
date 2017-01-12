/**  Optimizer - Header
 *   @file      Optimizer Interface Class
 *   @ingroup   GPIC++
 *   @defgroup  Optimizer     Optimizer
 *
 *   @brief     Optimizer Interface Class - Class defining the Optimizer interface
 *
 *   @author    Matthew Daigle
 *   @version   0.1.0
 *
 *   @pre       N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: August 10, 2016
 *
 *   @copyright Copyright (c) 2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_Optimizer_H
#define PCOE_Optimizer_H

#include <vector>
#include "ThreadSafeLog.h"

namespace PCOE {
    class Optimizer {
    public:
        Optimizer() : log(Log::Instance()) {}
        
        //virtual ~Optimizer() = default;
        
        /** @brief    Find x that minimizes evaluation function given starting x0
         *  @param    x0 Starting point of minimization
         *  @param    evaluate Solution evaluation function
         *  @param    xOptimal Optimized value
         **/
        //virtual void minimize(const std::vector<double> & x0,
        //                      double(*evaluate)(std::vector<double>),
        //                      std::vector<double> & xOptimal);

    protected:
        Log &log;                       ///> Logger (Defined in ThreadSafeLog.h)
    };
}
#endif  // PCOE_Optimizer_H
