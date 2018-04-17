/**  Recorder Communicator - Body
 *   @class     RecorderCommunicator RecorderCommunicator.h
 *   @ingroup   GPIC++
 *   @ingroup   commCollection
 *
 *   @brief     Recorder Communicator class- records all data to a file. All data is comma
 *              seperated.
 *              A Header line is written to the top of the file, then data is written
 *              (one line per timestep). The header line is in the following format:
 *                  Timestamp, [Data In], [Prognostic Results], time (in ms since epoch)
 *
 *              Each is described futher below:
 *                  Timestamp:              The current date and time
 *                  Data In:                Input data keys from the communicators
 *                  Prognostic Results:     The output of the prognosers (from the ProgData
 *                                          Structure)
 *                                          In the following format:
 *                                          pData-[%1].%2[%3]%0 where
 *                                          %1: Component Name
 *                                          %2: Element (either Events or sysTrajectories)
 *                                          %3: Identifier (Event or System Trajectory Name)
 *                                          For events the format of %0 above can be
 *                                          one of the following
 *                                          .TOE[%4](%u): Time of event element %4 in uData vector
 *                                              %u: Uncertainty Type
 *                                          .probMatrix[T+%5]: Probability of event at time %5
 *                                          .occurenceMatrix[T+%5][%6]: Occurance for time %5,
 *                                              sample %6
 *                                          For system trajectories the format of %0 above is the
 *                                              following:
 *                                          [T+%5][%4](%u): Occurance for time %5, element %4 in
 *                                              uData vector
 *                                              %u: Uncertainty Type
 *              Sensor data is then printed with a timestamp associated in the format "(t=%7)" in
 *              ms since epoch
 *              Prognostic Results are printed with a timestamp and validity in the format
 *              (v=%8, t=%7)
 *
 *   @note      This class will look for the following optional configuration parameters:
 *                  saveFile    File to which the data will be saved (default RecordedMessages.csv")
 *
 *   @see        CommonCommunicator
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 25, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "RecorderCommunicator.h"
#include "SharedLib.h"

namespace PCOE {
    // Defaults
    const std::string DEFAULT_FILE_NAME = "RecordedMessages.csv";
    const bool DEFAULT_WRITE_OCCUR = false;
    const bool DEFAULT_WRITE_PROB_OCCUR = false;
    const bool DEFAULT_WRITE_PREDICTIONS = false;
    const bool DEFAULT_WRITE_SYSTEM_TRAJ = true;

    // Configuration Keys
    const std::string FILE_KEY = "saveFile";
    const std::string PROB_OCCUR_KEY = "recordProbOccur";
    const std::string OCCUR_KEY = "recordOccurance";
    const std::string PREDICT_KEY = "recordPredictions";
    const std::string SYS_TRAJ_KEY = "recordSystemTrajectories";

    // Log Parameters
    const std::string MODULE_NAME = "RecorderComm";

    // Format Strings for writing to file
    const char toeFormatString[] = "pData-%s.Events[%s].TOE (%d), ";
    const char probFormatString[] = "pData-%s.Events[%s].probMatrix[T+%f], ";
    const char occFormatString[] = "pData-%s.Events[%s].occurrenceMatrix[T+%f], ";
    const char sysTrajFormatString[] = "pData-%s.sysTrajectories[%s][T+%f](%d), ";
    const char dataWithValidityAndTimeSpace[] = "%f (v=%i; t=%llii) ";
    // TODO (JW): duration representations are platform dependant, so %llu is not always correct
    const char dataWithTime[] = "%f (t=%llu), ";

    static void WriteTime(FILE* theFile) {
        using namespace std::chrono;

        system_clock::time_point now = system_clock::now();
        std::time_t now_tt = system_clock::to_time_t(now);
        system_clock::time_point now_sec = system_clock::from_time_t(now_tt);
        milliseconds ms = duration_cast<milliseconds>(now - now_sec);
        std::tm* tm = std::localtime(&now_tt);

        std::stringstream ss;
        ss << std::setfill('0');
        ss << std::setw(2) << tm->tm_hour << ":";
        ss << std::setw(2) << tm->tm_min << ":";
        ss << std::setw(2) << tm->tm_sec << ".";
        ss << std::setw(3) << ms.count() << ", ";

        std::fprintf(theFile, "%s", ss.str().c_str());
    }

    // --------------------------------------------------------------------------------------------

    RecorderCommunicator::RecorderCommunicator(const ConfigMap& config)
        : init(false),
          writeOccur(DEFAULT_WRITE_OCCUR),
          writeProbOccur(DEFAULT_WRITE_PROB_OCCUR),
          writePredictions(DEFAULT_WRITE_PREDICTIONS),
          writeSysTraj(DEFAULT_WRITE_SYSTEM_TRAJ) {
        // Configuration file to which the data is written (configurable)
        std::string configFile = DEFAULT_FILE_NAME;

        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");

        // Read Configuration Map
        if (config.includes(FILE_KEY)) {
            configFile = config.at(FILE_KEY)[0];
            log.FormatLine(LOG_DEBUG,
                           MODULE_NAME,
                           "Configuring recorder file to %s",
                           configFile.c_str());
        }

        if (config.includes(PROB_OCCUR_KEY)) {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Probability of occurence key present");

            if (config.at(PROB_OCCUR_KEY)[0] == "true" || config.at(PROB_OCCUR_KEY)[0] == "1") {
                writeProbOccur = true;
                log.WriteLine(LOG_DEBUG,
                              MODULE_NAME,
                              "Configuring to write probability of occurence");
            }
            else {
                writeProbOccur = false;
                log.WriteLine(LOG_DEBUG,
                              MODULE_NAME,
                              "Configuring to NOT write probability of occurence");
            }
        }

        if (config.includes(OCCUR_KEY)) {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Occurance Matrix key present");

            if (config.at(OCCUR_KEY)[0] == "true" || config.at(OCCUR_KEY)[0] == "1") {
                writeOccur = true;
                log.WriteLine(LOG_DEBUG, MODULE_NAME, "Configuring to write occurence matrix");
            }
            else {
                writeOccur = false;
                log.WriteLine(LOG_DEBUG, MODULE_NAME, "Configuring to NOT write occurence matrix");
            }
        }

        if (config.includes(PREDICT_KEY)) {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Prediction key present");

            if (config.at(PREDICT_KEY)[0] == "true" || config.at(PREDICT_KEY)[0] == "1") {
                writePredictions = true;
                log.WriteLine(LOG_DEBUG, MODULE_NAME, "Configuring to write predictions");
            }
            else {
                writePredictions = false;
                log.WriteLine(LOG_DEBUG, MODULE_NAME, "Configuring to write predictions");
            }
        }

        if (config.includes(SYS_TRAJ_KEY)) {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "System Trajectory key present");

            if (config.at(SYS_TRAJ_KEY)[0] == "true" || config.at(SYS_TRAJ_KEY)[0] == "1") {
                writeSysTraj = true;
                log.WriteLine(LOG_DEBUG, MODULE_NAME, "Configuring to write system trajectories");
            }
            else {
                writeSysTraj = false;
                log.WriteLine(LOG_DEBUG,
                              MODULE_NAME,
                              "Configuring to NOT write system trajectories");
            }
        }

        // Open File
        log.FormatLine(LOG_INFO, MODULE_NAME, "Opening data log file %s", configFile.c_str());
        theFile = std::fopen(configFile.c_str(), "w");
    }

    RecorderCommunicator::~RecorderCommunicator() {
        if (theFile) {
            // If the file is created, close
            log.WriteLine(LOG_DEBUG, MODULE_NAME, "Closing File");
            std::fclose(theFile);
            log.WriteLine(LOG_TRACE, MODULE_NAME, "File closed");
        }
    }

    DataStore RecorderCommunicator::read() {
        throw std::domain_error("Reading is not supported");
    }

    void RecorderCommunicator::write(AllData dataIn) {
        DataStore& data = dataIn.doubleDatastore;
        ProgDataMap& progDataMap = dataIn.progData;
        if (!init) {
            log.WriteLine(LOG_DEBUG, MODULE_NAME, "Printing Header");

            // If not initialized- Write Header
            fprintf(theFile, "\nTimeStamp, ");
            for (auto& itData : data) {
                fprintf(theFile, "%s, ", itData.first.c_str());
            }

            // Header for Prognostic Outputs
            for (auto& itPD : progDataMap) {
                const auto progName = itPD.first.c_str();
                const auto times = itPD.second->getTimes();

                // For each Event
                for (auto& itEvents : itPD.second->getEventNames()) {
                    const auto& event = itPD.second->events[itEvents];
                    const auto eventName = itEvents.c_str();
                    // timeOfEvent
                    fprintf(theFile,
                            toeFormatString,
                            progName,
                            eventName,
                            static_cast<int>(event.getTOE().uncertainty()));

                    // Probability of Occurrence
                    if (writeProbOccur) {
                        if (writePredictions) {
                            for (auto& theTime : times) {
                                fprintf(theFile, probFormatString, progName, eventName, theTime);
                            }
                        }
                        else {
                            fprintf(theFile, probFormatString, progName, eventName, times[0]);
                        }
                    }

                    // OccurrenceMat (if used)
                    if (writeOccur) {
                        if (!event.occurrenceMatrix[NOW].empty()) {
                            if (writePredictions) {
                                for (auto& theTime : times) {
                                    fprintf(theFile, occFormatString, progName, eventName, theTime);
                                }
                            }
                            else {
                                fprintf(theFile, occFormatString, progName, eventName, times[0]);
                            }
                        }
                    }
                } // end for each event

                // For System Trajectories
                if (writeSysTraj) {
                    for (auto& itOutputs : itPD.second->getSystemTrajectoryNames()) {
                        const auto outputName = itOutputs.c_str();
                        const auto uCert = static_cast<int>(
                            itPD.second->sysTrajectories[itOutputs][0].uncertainty());
                        if (writePredictions) {
                            for (auto& theTime : times) {
                                fprintf(theFile,
                                        sysTrajFormatString,
                                        progName,
                                        outputName,
                                        theTime,
                                        uCert);
                            }
                        }
                        else {
                            fprintf(theFile,
                                    sysTrajFormatString,
                                    progName,
                                    outputName,
                                    times[0],
                                    uCert);
                        }
                    } // end for each output
                }
            }
            // Header for timestamp
            fprintf(theFile, "Running Time\n");

            init = true;
            log.WriteLine(LOG_TRACE, MODULE_NAME, "End Print Header");
        }

        // Print Data
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Printing Data Snapshot");
        WriteTime(theFile); // Current Time

        // Print Input Data
        for (auto& itData : data) {
            fprintf(theFile, dataWithTime, itData.second.get(), itData.second.getTime());
        }

        // Print Prognostics Outputs
        for (auto& itPD : progDataMap) {
            const auto times = itPD.second->getTimes();

            // Write all only if writePredictions == true
            const auto nTimes = writePredictions ? times.size() : 1;

            // Events
            for (auto& itEvents : itPD.second->getEventNames()) {
                const auto& event = itPD.second->events[itEvents];

                // timeOfEvent
                const auto vec = event.getTOE().getVec(0);
                // @note(CT): For one character put is more efficient (doesn't have to scan for
                //      endstring character)
                fputc('[', theFile);

                // Below values do not change inside loop, so there are accessed here for efficiency
                const auto&& timeofEventLastUpdate = event.getTOE().updated();
                const auto&& timeofEventValidity = event.getTOE().valid();
                for (unsigned long it = 0; it < vec.size(); ++it) {
                    fprintf(theFile,
                            dataWithValidityAndTimeSpace,
                            vec[it],
                            timeofEventValidity,
                            timeofEventLastUpdate);
                }
                fprintf(theFile, "], ");

                // Probability of Occurrence
                if (writeProbOccur) {
                    for (unsigned int theTime = 0; theTime < nTimes; theTime++) {
                        fprintf(theFile, "%f, ", event.probMatrix[theTime]);
                    }
                }

                // OccurrenceMat (if used)
                if (writeOccur) {
                    if (!event.occurrenceMatrix[NOW].empty()) {
                        for (unsigned int theTime = 0; theTime < nTimes; theTime++) {
                            fputc('[', theFile);
                            for (const auto& theSample : event.occurrenceMatrix[theTime]) {
                                fprintf(theFile, "%s ", theSample ? "true" : "false");
                            }
                            fprintf(theFile, "], ");
                        }
                    }
                }
            }

            // System Trajectories
            if (writeSysTraj) {
                for (auto& itOutputs : itPD.second->getSystemTrajectoryNames()) {
                    for (unsigned int theTime = 0; theTime < nTimes; theTime++) {
                        const auto vec = itPD.second->sysTrajectories[itOutputs][theTime].getVec(0);
                        fputc('[', theFile); // for single character fputc is more efficient

                        // Calculate outside of loop for speed
                        const auto&& trajValidity =
                            itPD.second->sysTrajectories[itOutputs][theTime].valid();
                        const auto&& trajLastUpdate =
                            itPD.second->sysTrajectories[itOutputs][theTime].updated();
                        for (const auto& it : vec) {
                            fprintf(theFile,
                                    dataWithValidityAndTimeSpace,
                                    it,
                                    trajValidity,
                                    trajLastUpdate);
                        }
                        fprintf(theFile, "], ");
                    }
                }
            }
        }

        // Print Timestamp
        fprintf(theFile, "%llul\n", millisecondsNow());
        std::fflush(theFile); // Flush (update file)
        log.WriteLine(LOG_TRACE, MODULE_NAME, "End Print Line");
    }
}
