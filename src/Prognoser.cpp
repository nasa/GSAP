/**  Common Prognoser - Body
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

#include <chrono>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h> // For file exists in loadHistory
#include <thread> // For thread sleep
#include <vector>

#include "ConfigMap.h"
#include "Prognoser.h"
#include "StringUtils.h"

namespace PCOE {
    // DEFAULTS
    const unsigned int DEFAULT_INTERVAL_DELAY = 100; // ms
    const unsigned int DEFAULT_SAVE_INTERVAL = 60; // loops
    const bool DEFAULT_SAVE_ENABLE = false;

    // CONFIGURATION KEYS
    const std::string TYPE_KEY = "type";
    const std::string NAME_KEY = "name";
    const std::string ID_KEY = "id";
    const std::string HIST_PATH_KEY = "histPath";
    const std::string TAG_KEY = "inTags";
    const std::string RESET_HIST_KEY = "resetHist";
    const std::string INTERVAL_DELAY_KEY = "intervalDelay";
    const std::string SAVE_INTERVAL_KEY = "saveInterval";
    const std::string SAVE_ENABLE_KEY = "saveEnable";

    std::string MODULE_NAME;

    Prognoser::Prognoser(ConfigMap& configParams)
        : Thread(),
          loopInterval(DEFAULT_INTERVAL_DELAY),
          saveInterval(DEFAULT_SAVE_INTERVAL),
          saveEnabled(DEFAULT_SAVE_ENABLE) {
        requireKeys(configParams, {NAME_KEY, ID_KEY, TYPE_KEY});

        // Fill in Defaults
        if (configParams.hasKey(INTERVAL_DELAY_KEY)) {
            loopInterval = static_cast<unsigned int>(configParams.getInt32(INTERVAL_DELAY_KEY));
        }

        if (configParams.hasKey(SAVE_INTERVAL_KEY)) {
            saveInterval = static_cast<unsigned int>(configParams.getInt32(SAVE_INTERVAL_KEY));
        }

        if (configParams.hasKey(SAVE_ENABLE_KEY)) {
            saveEnabled = (configParams.getVector(SAVE_ENABLE_KEY)[0].compare("true") == 0) ||
                          (configParams.getVector(SAVE_ENABLE_KEY)[0].compare("0") == 0);
        }

        if (!configParams.hasKey(HIST_PATH_KEY)) {
            configParams.set(HIST_PATH_KEY, ".");
        }
        if (!configParams.hasKey(RESET_HIST_KEY)) {
            configParams.set(RESET_HIST_KEY, "false");
        }

        // Process tags
        //
        // For each tag in (local:global) format, add a function to the lookup table to get the
        // value by the global name when requesting it by the local name. This handles the local
        // to global conversion.
        if (configParams.hasKey(TAG_KEY)) {
            for (auto& it : configParams.getVector(TAG_KEY)) {
                size_t pos = it.find_first_of(':');
                std::string commonName = it.substr(0, pos);
                std::string tagName = it.substr(pos + 1, it.length() - pos + 1);
                log.FormatLine(LOG_TRACE,
                               MODULE_NAME,
                               "Registering tag common=%s, tag=%s",
                               commonName.c_str(),
                               tagName.c_str());
            }
        }

        histFileName = configParams.getVector(HIST_PATH_KEY)[0] + PATH_SEPARATOR + MODULE_NAME =
                           moduleName + "-Common";
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Read configuration file");

        // Handle History file
        if (configParams.getVector(RESET_HIST_KEY)[0].compare("true") == 0) {
            // Reset History flag has been set
            resetHistory();
        }
        enable();
    }

    //*----------------------------------------------*
    //|           Main Prognostics Thread            |
    //*----------------------------------------------*
    void Prognoser::run() {
        unsigned long loopCounter = 0;

        try {
            loadHistory(); // Load prognoser history file
            // @note(CT): Cannot be in constructor because
            // derived will not exist yet at that point
        }
        catch (...) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Error loading prognoser history- skipping");
        }

        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting Prognostics Loop");
        while (getState() != ThreadState::Stopped) {
            log.FormatLine(LOG_TRACE, MODULE_NAME, "Loop %i", loopCounter);
            if (getState() == ThreadState::Started) {
                try {
                    // Run Cycle
                    checkInputValidity(); // SOMETIMES FAILS HERE
                    if (isEnoughData()) {
                        log.WriteLine(LOG_TRACE,
                                      MODULE_NAME,
                                      "Has enough data- starting monitor step");
                        step();
                    }
                    checkResultValidity();
                }
                catch (std::system_error& ex) {
                    log.WriteLine(LOG_ERROR, MODULE_NAME, "Error in Prognoser Loop- Skipping Step");
                    log.WriteLine(LOG_ERROR, MODULE_NAME, std::string("    ") + ex.what());
                    log.WriteLine(LOG_ERROR,
                                  MODULE_NAME,
                                  std::string("    EC: ") + ex.code().message());
                }
                catch (std::exception& ex) {
                    log.WriteLine(LOG_ERROR, MODULE_NAME, "Error in Prognoser Loop- Skipping Step");
                    log.WriteLine(LOG_ERROR, MODULE_NAME, std::string("    ") + ex.what());
                }
                catch (...) {
                    /// @todo(CT): Display more information
                    log.WriteLine(LOG_ERROR, MODULE_NAME, "Error in Prognoser Loop- Skipping Step");
                }
                if (saveEnabled && 0 == loopCounter % saveInterval) {
                    saveState();
                }
            } // End if(started)

            log.WriteLine(LOG_TRACE, MODULE_NAME, "Waiting");
            if (getState() == ThreadState::Stopped) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(loopInterval));
            loopCounter++;
        } // End While(not stopped)

        /// Cleanup activities
        log.WriteLine(LOG_INFO, MODULE_NAME, "Cleaning Up");
        saveState(); // Save final state
    }

    //*----------------------------------------------*
    //|              Support Functions               |
    //*----------------------------------------------*

    void Prognoser::checkResultValidity() {
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Checking Result Validity");
    }

    inline void saveMap(std::fstream& fdHist, const std::map<std::string, double>& valueMap) {
        for (auto& it : valueMap) {
            fdHist << it.first << ":" << it.second << ",";
        }
    }

    void Prognoser::saveState() const {
        using namespace std::chrono;
        // @todo(CT): Make more efficient- right now it copies every udata vector

        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Saving state to file");
        std::fstream fdHist;
        fdHist.open(histFileName, std::fstream::out);
        if (!fdHist.is_open()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Could not open history file to save state");
            return;
        }

        // Print previous lines
        for (auto& it : histStr) {
            fdHist << it << '\n';
        }

        // Time
        fdHist << "time:"
               << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        // @todo: Refactor writing to hist file for event-driven architecture

        fdHist.close();
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Finished saving state to file");
    }

    // unsigned int getTimeIndex(const double value, ProgData &pd) {
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

    void Prognoser::loadHistory() {
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Loading history from file");
        struct stat buf;
        if (stat(histFileName.c_str(), &buf) == -1) {
            log.FormatLine(LOG_INFO,
                           MODULE_NAME,
                           "Prognostic history file %s does not exist yet",
                           histFileName.c_str());
            return;
        }

        std::fstream fdHist;
        fdHist.open(histFileName, std::fstream::in);
        if (!fdHist.is_open()) {
            log.FormatLine(LOG_WARN,
                           MODULE_NAME,
                           "Prognostic history file %s could not be opened",
                           histFileName.c_str());
            return;
        }

        std::string line;
        while (std::getline(fdHist, line)) {
            if (!line.empty()) {
                histStr.push_back(line);
            }
        }

        if (histStr.size() == 0) {
            log.FormatLine(LOG_WARN,
                           MODULE_NAME,
                           "Prognostic history file %s was empty",
                           histFileName.c_str());
            return;
        }

        // Initialize Model
        std::vector<std::string> entries;
        std::string entry;
        std::istringstream ss(histStr.back());
        std::vector<double> timeVec;

        while (std::getline(ss, entry, ':')) {
            switch (entry[0]) {
            case 't': // time
                std::getline(ss, entry, ',');
                break;
                // @todo: Refactor for event-driven architecture
                // {
                // case 'e': // Event
                //     std::string eventName, subEntry, value;
                //     std::istringstream sss(entry);

                //     std::getline(sss, eventName, '[');
                //     std::getline(sss, eventName, ']');
                //     if (!lastState.events.includes(eventName)) {
                //         lastState.addEvent(eventName);
                //     }
                //     ProgEvent& theEvent = lastState.events[eventName];

                //     std::getline(sss, subEntry, '[');
                //     char identifier = subEntry[1];
                //     std::getline(sss, subEntry, ']');
                //     std::getline(ss, entry, ','); // Get the value

                //     switch (identifier) {
                //         {
                //         case 'T':
                //             std::string type;
                //             std::getline(sss, type, '(');
                //             std::getline(sss, type, ')');
                //             theEvent.getTOE().uncertainty(static_cast<UType>(std::stoi(type)));
                //             if (std::stoul(subEntry) >= theEvent.getTOE().npoints()) {
                //                 theEvent.getTOE().npoints(
                //                     static_cast<unsigned int>(std::stoul(subEntry) + 1));
                //             }
                //             theEvent.getTOE()[std::stoul(subEntry)] = std::stod(entry);

                //             break;
                //         }

                //     default:
                //         log.WriteLine(LOG_ERROR,
                //                       MODULE_NAME,
                //                       "Unknown Event parameter in history file");
                //         break;
                //     }
                //     break;
                // }

                // {
                // case 's': // System Trajectories
                //     std::string trajName, timeStampStr, uIndex, type;
                //     std::istringstream sss(entry);

                //     std::getline(sss, trajName, '[');
                //     std::getline(sss, trajName, ']');
                //     if (!lastState.sysTrajectories.includes(trajName)) {
                //         lastState.addSystemTrajectory(trajName);
                //     }
                //     DataPoint& theTraj = lastState.sysTrajectories[trajName];

                //     std::getline(sss, timeStampStr, '[');
                //     std::getline(sss, timeStampStr, ']');
                //     std::getline(sss, uIndex, '[');
                //     std::getline(sss, uIndex, ']');
                //     std::getline(sss, type, '(');
                //     std::getline(sss, type, ')');
                //     std::getline(ss, entry, ',');

                //     if (entry.empty() || type.empty() || uIndex.empty()) {
                //                        log.FormatLine(LOG_WARN,
                //                                       MODULE_NAME,
                //                                       "Found element of improper format: %s.
                //                                       Skipping", entry.c_str());
                //                        break;
                //     }
                //     theTraj.setUncertainty(static_cast<UType>(std::stoi(type)));
                //     double value = std::stod(entry);
                //     unsigned int sampleIndex = static_cast<unsigned int>(std::stoul(uIndex));

                //     if (sampleIndex >= theTraj[0].size()) {
                //         theTraj[0].npoints(sampleIndex + 1);
                //         /// @todo(CT): resize above for efficiency
                //     }
                //     theTraj[0][sampleIndex] = value;

                //     break;
                // }

                {
                case 'i': // Internal
                    std::string internalName;
                    std::istringstream sss(entry);

                    std::getline(sss, internalName, '[');
                    std::getline(sss, internalName, ']');
                    std::getline(ss, entry, ',');

                    break;
                }

            default: // Unknown
                log.FormatLine(LOG_WARN,
                               MODULE_NAME,
                               "Unknown parameter found in history file - %s",
                               entry.c_str());
                std::getline(ss, entry, ',');
                break;
            }
        }

        log.WriteLine(LOG_TRACE, MODULE_NAME, "Finished loading history from file");
    }

    Datum<double> Prognoser::getValue(MessageId key) {
        log.FormatLine(LOG_TRACE, MODULE_NAME, "Getting lookup function for key %ull", key);
        std::function<Datum<double>()> fn = lookup[key];
        log.FormatLine(LOG_TRACE, MODULE_NAME, "Getting value for key %ull", key);
        Datum<double> result = fn();
        log.FormatLine(LOG_TRACE, MODULE_NAME, "Getting value ", result.get());
        return result;
    }

    inline void Prognoser::resetHistory() const {
        using std::chrono::seconds;
        using std::chrono::system_clock;
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Resetting History");

        char numstr[21] = {0}; // enough to hold all numbers up to 64-bits
        long long int time = system_clock::now().time_since_epoch() / seconds(1);
        snprintf(numstr, 21, "%lld", time);
        std::string newName = histFileName + "_old" + numstr;
        if (rename(histFileName.c_str(), newName.c_str())) {
            log.WriteLine(LOG_WARN, MODULE_NAME, "Could not rename history file");
        }
    }
}
