/**  Common Prognoser - Body
*   @class     CommonPrognoser CommonPrognoser.h
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
*   @version   0.1.0
*
*   @pre       Prognostic Configuration File and Prognoster Configuration Files
*
*      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
*      Created: November 11, 2015
*
*   @copyright Copyright (c) 2013-2016 United States Government as represented by
*     the Administrator of the National Aeronautics and Space Administration.
*     All Rights Reserved.
*/

#include <sys/stat.h>       // For file exists in loadHistory
#include <cmath>
#include <thread>           // For thread sleep
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <chrono>

#include "CommonPrognoser.h"
#include "SharedLib.h"
#include "CommManager.h"
#include "GSAPConfigMap.h"

namespace PCOE {
   // DEFAULTS
   const std::string   DEFAULT_INTERVAL_DELAY = "500";  // ms
   const unsigned int  DEFAULT_SAVE_INTERVAL = 60;   // loops

   // CONFIGURATION KEYS
   const std::string   TYPE_KEY = "type";
   const std::string   NAME_KEY = "name";
   const std::string   ID_KEY = "id";
   const std::string   HIST_PATH_KEY = "histPath";
   const std::string   TAG_KEY = "inTags";
   const std::string   RESET_HIST_KEY = "resetHist";
   const std::string   INTERVAL_DELAY_KEY="intervalDelay";

   const std::string   IMPORT_KEY = "importConfig";

   std::string         MODULE_NAME;

   CommonPrognoser::CommonPrognoser(GSAPConfigMap & configParams)
       : Thread(), comm(CommManager::instance()),
       saveInterval(DEFAULT_SAVE_INTERVAL),
       usingPlaybackData(false) {
       if (configParams.includes(IMPORT_KEY)) {
           for (auto && file : configParams[IMPORT_KEY]) {
               log.FormatLine(LOG_DEBUG, MODULE_NAME, "Reading configuration file %s", file.c_str());
               configParams.loadFile(file);
           }
       }
       configParams.checkRequiredParams({ NAME_KEY, ID_KEY, TYPE_KEY });

       // Handle Required configs
       results.setPrognoserName(configParams.at(TYPE_KEY)[0]);
       results.setComponentName(configParams.at(NAME_KEY)[0]);
       results.setUniqueId(configParams.at(ID_KEY)[0]);

       // Fill in Defaults
       if (!configParams.includes(INTERVAL_DELAY_KEY)) {
           configParams.set(INTERVAL_DELAY_KEY,
               DEFAULT_INTERVAL_DELAY);
       }
       loopInterval = std::stoi((configParams.at(INTERVAL_DELAY_KEY)[0]).c_str());

       if (!configParams.includes(HIST_PATH_KEY)) {
           configParams.set(HIST_PATH_KEY,
               ".");
       }
       if (!configParams.includes(RESET_HIST_KEY)) {
           configParams.set(RESET_HIST_KEY, "false");
       }

       // HANDLE TAGS
       if (configParams.includes(TAG_KEY)) {
           for (auto & it : configParams.at(TAG_KEY)) {
               size_t pos = it.find_first_of(':');
               std::string commonName = it.substr(0, pos);
               std::string tagName = it.substr(pos + 1, it.length() - pos + 1);
               comm.registerKey(tagName);
           }
       }
       comm.registerProgData(configParams.at(NAME_KEY)[0], &results);

       histFileName = configParams.at(HIST_PATH_KEY)[0] + PATH_SEPARATOR \
           + results.getPrognoserName() + "_" \
           + results.getUniqueId() + ".txt";
       moduleName = results.getComponentName() + " " + results.getPrognoserName() + " Prognoser";
       MODULE_NAME = moduleName + "-Common";
       log.WriteLine(LOG_DEBUG, MODULE_NAME, "Read configuration file");

       // Handle History file
       if (configParams.at(RESET_HIST_KEY)[0].compare("true") == 0) {
           // Reset History flag has been set
           resetHistory();
       }
       enable();
   }

   //*----------------------------------------------*
   //|           Main Prognostics Thread            |
   //*----------------------------------------------*
   void CommonPrognoser::run() {
       unsigned long loopCounter = 0;

       loadHistory();  // Load prognoser history file
       // @note(CT): Cannot be in constructor because
       // derived will not exist yet at that point

       log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting Prognostics Loop");
       while (getState() != ThreadState::Stopped) {
           log.FormatLine(LOG_TRACE, MODULE_NAME, "Loop %i", loopCounter);
           if (getState() == ThreadState::Started) {
               try {
                   // Run Cycle
                   checkInputValidity();  // SOMETIMES FAILS HERE
                   if (isEnoughData()) {
                       log.WriteLine(LOG_TRACE, MODULE_NAME,
                           "Has enough data- starting monitor step");
                       step();
                   }
                   checkResultValidity();
                   if (0 == loopCounter%saveInterval) {
                       saveState();
                   }
               } catch (...) {
                   /// @todo(CT): Display more information
                   log.WriteLine(LOG_ERROR, MODULE_NAME, "Error in Prognoser Loop- Skipping Step");
               }
           }  // End if(started)

           log.WriteLine(LOG_TRACE, MODULE_NAME, "Waiting");
           if (getState() == ThreadState::Stopped) {
               break;
           }
           std::this_thread::sleep_for(std::chrono::milliseconds(loopInterval));
           loopCounter++;
       }  // End While(not stopped)

       /// Cleanup activities
       log.WriteLine(LOG_INFO, MODULE_NAME, "Cleaning Up");
       saveState();  // Save final state
   }

   //*----------------------------------------------*
   //|              Support Functions               |
   //*----------------------------------------------*

   void CommonPrognoser::checkResultValidity() {
       log.WriteLine(LOG_TRACE, MODULE_NAME, "Checking Result Validity");
   }

   inline void saveMap(std::fstream& fdHist, const std::map<std::string, double>& valueMap) {
       for (auto & it : valueMap) {
           fdHist << it.first << ":" << it.second << ",";
       }
   }

   void CommonPrognoser::saveState() const {
       // @todo(CT): Make more efficient- right now it copies every udata vector

       log.WriteLine(LOG_DEBUG, MODULE_NAME, "Saving state to file");
       std::fstream fdHist;
       fdHist.open(histFileName, std::fstream::out);
       if (!fdHist.is_open()) {
           log.WriteLine(LOG_ERROR, MODULE_NAME,
               "Could not open history file to save state");
           return;
       }

       // Print previous lines
       for (auto & it : histStr) {
           fdHist << it << '\n';
       }

       // Time
       fdHist << "time:" << millisecondsNow();

       // For each Event
       for (auto && itEvents : results.getEventNames()) {
           const auto & event = results.events[itEvents];
           // timeOfEvent
           const auto && vec = event.timeOfEvent.getVec(0);
           std::stringstream tmp;
           tmp << ",e[" << itEvents;
           for (unsigned long it = 0; it < vec.size(); ++it) {
               fdHist << tmp.str() << "].TOE["\
                   << it << "](" << static_cast<int>(event.timeOfEvent.uncertainty()) \
                   << "):" << event.timeOfEvent.get();
           }

           // Occurrence
           fdHist << tmp.str() << "].pMat[T+0]:" \
               << static_cast<double>(event.probMatrix[0]);

           // OccurrenceMat (if used)
           if (!event.occurrenceMatrix[NOW].empty()) {
               for (unsigned int theSample = 0; theSample < event.occurrenceMatrix[0].size(); theSample++) {
                   fdHist << tmp.str() << "].oMat[T+0]" << theSample \
                       << "]:" << event.occurrenceMatrix[0][theSample];
               }
           }
       }

       // For System Trajectories
       for (auto & itOutputs : results.getSystemTrajectoryNames()) {
           const auto && vec = results.sysTrajectories[itOutputs][0].getVec(0);
           std::stringstream tmp;
           tmp << ",sTraj[" << itOutputs << "][T+0][";
           for (unsigned int it = 0; it < vec.size(); ++it) {
               fdHist << tmp.str() << it << "](" \
                   << static_cast<int>(results.sysTrajectories[itOutputs][0].uncertainty()) << "):"\
                   << results.sysTrajectories[itOutputs][0][it];
           }
       }

       // Internal State
       saveMap(fdHist, results.internals);

       fdHist.close();
       log.WriteLine(LOG_TRACE, MODULE_NAME, "Finished saving state to file");
   }

   //unsigned int getTimeIndex(const double value, ProgData &pd) {
   //    // Converts time value (T+X) to index T[I]
   //    static std::vector<double> timeVec;
   //
   //    for (unsigned int it = 0; it < timeVec.size(); ++it) {
   //        // If it exists
   //        if (std::abs(timeVec[it]-value) <= 1e-9) {
   //            return it;
   //        }
   //    }
   //    // New timestamp
   //    timeVec.push_back(value);
   //    pd.setPredictions(timeVec);
   //    return (unsigned int) timeVec.size()-1;
   //}

   void CommonPrognoser::loadHistory() {
       struct stat buf;
       if (stat(histFileName.c_str(), &buf) != -1) {
           // File exists
           std::fstream fdHist;
           fdHist.open(histFileName.c_str(), std::fstream::in);  // Used c str for C98 Compatability

           if (fdHist.is_open()) {
               // File was opened correctly
               log.FormatLine(LOG_INFO, MODULE_NAME, "Loading Prognostic history file %s for %s",
                   histFileName.c_str(), results.getComponentName().c_str());
               std::string line;

               while (getline(fdHist, line)) {
                   // get the data
                   if (!line.empty()) {
                       // ignore empty lines
                       histStr.push_back(line);  // Record line for saveState
                   }
               }

               if (histStr.size() != 0) {
                   // Atleast one line received
                   // Initialize Model
                   std::vector<std::string> enteries;
                   std::string entry;
                   std::istringstream ss(histStr.back());
                   ProgData lastState;
                   std::vector<double> timeVec;

                   while (std::getline(ss, entry, ':')) {
                       // While another key exists
                       switch (entry[0]) {
                       case 't':   // time
                           std::getline(ss, entry, ',');
                           break;

                           {case 'e':  // Event
                               std::string eventName, subEntry, value;
                               std::istringstream sss(entry);

                               std::getline(sss, eventName, '[');
                               std::getline(sss, eventName, ']');
                               if (!lastState.events.includes(eventName)) {
                                   lastState.addEvent(eventName);
                               }
                               ProgEvent & theEvent = lastState.events[eventName];

                               std::getline(sss, subEntry, '[');
                               char identifier = subEntry[1];
                               std::getline(sss, subEntry, ']');
                               std::getline(ss, entry, ',');  // Get the value

                               switch (identifier) {
                                   {case 'T':
                                       std::string type;
                                       std::getline(sss, type, '(');
                                       std::getline(sss, type, ')');
                                       theEvent.timeOfEvent.uncertainty(static_cast<UType>(std::stoi(type)));
                                       if (std::stoul(subEntry) >= theEvent.timeOfEvent.npoints()) {
                                           theEvent.timeOfEvent.npoints(static_cast<unsigned int>(std::stoul(subEntry) + 1));
                                       }
                                       theEvent.timeOfEvent[std::stoul(subEntry)] = std::stod(entry);

                                       break; }

                                   {case 'p':
                                       theEvent.probMatrix[0] = std::stod(entry);
                                       break; }

                                   {case 'o':
                                       std::getline(sss, subEntry, '[');
                                       std::getline(sss, subEntry, ']');

                                       if (std::stoul(subEntry) >= theEvent.occurrenceMatrix[0].size()) {
                                           theEvent.occurrenceMatrix[0].resize(std::stoul(subEntry) + 1);
                                           /// @todo(CT): resize above for efficiency
                                       }
                                       theEvent.occurrenceMatrix[0][std::stoul(subEntry)] = std::stoi(entry) != 0;
                                       break; }

                               default:
                                   log.WriteLine(LOG_ERROR, MODULE_NAME,
                                       "Unknown Event parameter in history file");
                                   break;
                               }
                               break; }

                           {case 's':  // System Trajectories
                               std::string trajName, timeStampStr, uIndex, type;
                               std::istringstream sss(entry);

                               std::getline(sss, trajName, '[');
                               std::getline(sss, trajName, ']');
                               if (!lastState.sysTrajectories.includes(trajName)) {
                                   lastState.addSystemTrajectory(trajName);
                               }
                               DataPoint & theTraj = lastState.sysTrajectories[trajName];

                               std::getline(sss, timeStampStr, '[');
                               std::getline(sss, timeStampStr, ']');
                               std::getline(sss, uIndex, '[');
                               std::getline(sss, uIndex, ']');
                               std::getline(sss, type, '(');
                               std::getline(sss, type, ')');
                               std::getline(ss, entry, ',');

                               if (entry.empty() || type.empty() || uIndex.empty()) {
                                   log.FormatLine(LOG_WARN, MODULE_NAME, "Found element of improper format: %s. Skipping", entry.c_str());
                                   break;
                               }
                               theTraj.setUncertainty(static_cast<UType>(std::stoi(type)));
                               double value = std::stod(entry);
                               unsigned int sampleIndex = static_cast<unsigned int>(std::stoul(uIndex));

                               if (sampleIndex >= theTraj[0].size()) {
                                   theTraj[0].npoints(sampleIndex + 1);
                                   /// @todo(CT): resize above for efficiency
                               }
                               theTraj[0][sampleIndex] = value;

                               break; }

                           {case 'i':  // Internal
                               std::string internalName;
                               std::istringstream sss(entry);

                               std::getline(sss, internalName, '[');
                               std::getline(sss, internalName, ']');
                               std::getline(ss, entry, ',');

                               lastState.internals[internalName] = std::stod(entry);
                               break; }

                       default:  // Unknown
                           log.FormatLine(LOG_WARN, MODULE_NAME,
                               "Unknown parameter found in history file - %s",
                               entry.c_str());
                           std::getline(ss, entry, ',');
                           break;
                       }
                   }

                   setHistory(lastState);
               }
               else {
                   // File was empty
                   log.FormatLine(LOG_WARN, MODULE_NAME,
                       "Prognostic history file %s was empty",
                       histFileName.c_str());
               }
           }
           else {
               // File not opened correctly
               log.FormatLine(LOG_WARN, MODULE_NAME,
                   "Prognostic history file %s could not be opened",
                   histFileName.c_str());
           }
       }
       else {
           log.FormatLine(LOG_INFO, MODULE_NAME,
               "Prognostic history file %s does not exist yet",
               histFileName.c_str());
       }

       log.WriteLine(LOG_TRACE, MODULE_NAME, "Loading history from file");
   }

   inline void CommonPrognoser::resetHistory() const {
       using std::chrono::system_clock;
       using std::chrono::seconds;
       log.WriteLine(LOG_TRACE, MODULE_NAME, "Resetting History");

       char numstr[21] = { 0 };  // enough to hold all numbers up to 64-bits
       long long int time = system_clock::now().time_since_epoch() / seconds(1);
       snprintf(numstr, 21, "%lld", time);
       std::string newName = histFileName + "_old" + numstr;
       if (rename(histFileName.c_str(), newName.c_str())) {
           log.WriteLine(LOG_WARN, MODULE_NAME, "Could not rename history file");
       }
   }
}
