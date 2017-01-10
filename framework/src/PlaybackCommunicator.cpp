/**  Playback Communicator - Body
 *   @class     PlaybackCommunicator PlaybackCommunicator.h
 *   @ingroup   GPIC++
 *   @ingroup   commCollection
 *
 *   @brief     Playback Communicator class- playback input data from a csv file
 *
 *   @note      This class will look for the following optional configuration parameters:
 *                  file        Name of the file that will be played back (default RecordedMessages.csv)
 *
 *   @see        CommonCommunicator
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 27, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#include <iostream>
#include <sstream>
#include <string>
#include <chrono> // For timestamps

#include "Exceptions.h"
#include "PlaybackCommunicator.h"
#include "SharedLib.h"

namespace PCOE {
    // Defaults
    const std::string DEFAULT_FILE_NAME = "RecordedMessages.csv";
    const char DEFAULT_DELIM = ',';
    const bool DEFAULT_TIMESTAMP = true;

    // Configuration Keys
    const std::string FILE_KEY = "file";
    const std::string DELIM_KEY = "delim";
    const std::string TIMESTAMP_KEY = "timestampFromFile";

    // Log Parameters
    const std::string MODULE_NAME = "playbackComm";

    PlaybackCommunicator::PlaybackCommunicator(const ConfigMap & config) :
        delim(DEFAULT_DELIM),
        timestampFromFile(DEFAULT_TIMESTAMP) {
        std::string playbackFile = DEFAULT_FILE_NAME;

        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");
        // Read Configuration Map
        if (config.includes(FILE_KEY)) {
            playbackFile = config.at(FILE_KEY)[0];
            log.FormatLine(LOG_DEBUG, MODULE_NAME, "Configuring- Playback File Name %s",
                playbackFile.c_str());
        }

        if (config.includes(DELIM_KEY)) {
            log.WriteLine(LOG_DEBUG, MODULE_NAME, "Setting delimiter");

            if (config.at(DELIM_KEY)[0].compare("\\t") == 0) {
                delim = '\t';
            }
            else {
                delim = config.at(DELIM_KEY)[0].c_str()[0];
            }
        }

        if (config.includes(TIMESTAMP_KEY)) {
            log.WriteLine(LOG_TRACE, MODULE_NAME, "Timestamp key received");

            timestampFromFile = (config.at(TIMESTAMP_KEY)[0].compare("true") ||
                config.at(TIMESTAMP_KEY)[0].compare("1"));
        }

        log.FormatLine(LOG_INFO, MODULE_NAME,
            "Opening playback file %s", playbackFile.c_str());

        playbackStream.exceptions(std::ios_base::failbit);
        try {
            playbackStream.open(playbackFile);
        }
        catch (const std::ios_base::failure ex) {
            std::string msg =
                std::string("Error opening playback file: ") + ex.what();
            log.WriteLine(LOG_ERROR, MODULE_NAME, msg);
            throw;
        }

        // Read Header
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Reading Header");

        std::string s;
        while (s.substr(0, 9).compare("Timestamp") != 0 \
            && s.substr(0, 9).compare("TimeStamp") != 0) {
            if (!getline(playbackStream, s)) {
                log.WriteLine(LOG_ERROR, MODULE_NAME,
                    "Playback file not in proper format");
                throw FormatError("Playback file not in proper format");
            }
        }

        // Parse Header
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Parsing Header");

        std::istringstream ss(s);
        while (ss) {
            std::string s2;
            if (!getline(ss, s2, delim)) {
                break;
            }

            if (s2.substr(0, 7).compare(" pData-") == 0) {
                // Break line
                break;
            }

            if (s2.compare("Timestamp") != 0 && s2.compare(" Running Time") != 0) {
                // Ignored lines
                trim(s);
                header.push_back(s);
            }
        }
        log.FormatLine(LOG_TRACE, MODULE_NAME,
            "Registered %d parameters", header.size());
    }

    DataStore PlaybackCommunicator::read() {
        DataStore ds;
        // Read Line
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Getting New Line");

        std::string s;
        if (!getline(playbackStream, s)) {
            log.WriteLine(LOG_WARN, MODULE_NAME, "Reached end of file");

            return ds;
        }

        // Parse Line
        std::istringstream ss(s);
        std::string s2;

        if (!getline(ss, s2, delim)) {
            log.WriteLine(LOG_WARN, MODULE_NAME, "Line was empty");

            return ds;
        }

        // Otherwise- received timestamp
        static std::chrono::time_point<std::chrono::system_clock> startingTime = std::chrono::system_clock::now();

        const auto step = std::chrono::milliseconds(static_cast<unsigned long long>(std::stold(s2) * 1000));
        std::chrono::time_point<std::chrono::system_clock> theTime = startingTime + step;

        for (const auto & it : header) {
            if (!getline(ss, s2, delim)) {
                log.WriteLine(LOG_WARN, MODULE_NAME,
                    "parameter not present-reached end of line");
                return ds;
            }
            std::istringstream ss2(s2);
            std::string s3;
            if (!getline(ss2, s3, '(')) {
                ds[it] = std::stof(s2);
                if (timestampFromFile) {
                    ds[it].setTime(theTime);
                }
                continue;
            }
            ds[it] = std::stof(s3);
            if (timestampFromFile) {
                ds[it].setTime(theTime);
            }
        }

        return ds;
    }
    
    void PlaybackCommunicator::write(AllData dataIn) {
        (void) dataIn;
        throw std::domain_error("Write not supported");
    }

    PlaybackCommunicator::~PlaybackCommunicator() {
        log.WriteLine(LOG_INFO, MODULE_NAME, "Closing File");
        playbackStream.close();
    }
}
