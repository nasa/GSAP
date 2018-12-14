/**  Empty Prognoser - Body
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
#include <vector>

#include "ConfigMap.h"
#include "DataPoint.h"
#include "EmptyPrognoser.h"
#include "ProgEvent.h"

namespace PCOE {
    std::string moduleName = "Empty Prognoser";
    EmptyPrognoser::EmptyPrognoser(ConfigMap & configMap) :
        Prognoser(configMap) {

        // Handle Configuration
        log.WriteLine(LOG_DEBUG, moduleName, "Configuring");
        // std::string a = configMap.at("ExampleParam");
    }

    Prediction EmptyPrognoser::step(std::map<MessageId, Datum<double> > data) {
	    // Run calculations
	    std::vector<ProgEvent> events;
	    std::vector<UData> state = {UData(1.0)}; // Example
	    UData toe = UData(124222.01); // Example time of event
	    events.push_back(ProgEvent(MessageId::TestEvent0, state, toe));
	    
	    std::vector<DataPoint> trajectories; // System Trajectories
	    DataPoint exampleTrajectory;
	    exampleTrajectory.setNumTimes(1); // Save only one time
	    exampleTrajectory[0] = UData(0.5);
	    trajectories.push_back(exampleTrajectory);
	    
	    Prediction pred(events, trajectories);
	    return pred;
	    
    }
}
