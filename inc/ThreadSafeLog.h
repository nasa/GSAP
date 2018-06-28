/** ThreadSafeLog - Header
 *  @brief      A light singleton logger with multiple levels designed to be used in a multi-threaded environment. Logging messages is designed to be similar to use of printf. Designed with code re-use in mind.
 *
 *              As a singleton, the logger instance can be obtained by using the Log::Instance() static function
 *                  Log & logger = Log::Instance();
 *
 *              The log can then be initialized with the program name, version, and a note (if applicable) using the Initialize function. For example:
 *                  logger.Initialize("Example Program", 
                        "1.2.1",  
                        "If you have technical issues with the plugin, please report them by \nemailing Christopher Teubert (christopher.a.teubert@nasa.gov).");
 *
 *              You can set the verbosity limit for messages:
 *                  logger.SetVerbostiy(LOG_INFO);
 *
 *              To use the log you can then use the WriteLine and FormatLine functions. These functions take the logging level (described below), the tag (Some identifier for that piece of code: Could be a class name), and the message. For example
 *                  logger.WriteLine(LOG_FATAL, "ClassName", "Error Description");
 *                  logger.FormatLine(LOG_WARNING, "ClassName", "Key %s not recognized. Did you mean %s? Ignoring key", key, otherKey);
 *                  logger.LogVerbatim("Some text");
 *
 *              At the end of use the log can be closed using the following:
 *                  logger.close();
 *
 *              To create a log going to a different file, pass it into the Instance function like so:
 *                  Log & logger2 = Log::Instance("LogFile.txt");
 *
 *  @example
 *      Log & logger = Log::Instance();
 *      Log & logger2 = Log::Instance("LogFile.txt");
 *      logger.Initialize("Example Program", "1.2.1",
            "If you have technical issues with the plugin, please report them by \nemailing Christopher Teubert (christopher.a.teubert@nasa.gov).");
 *      logger.Initialize("Example Program- Log2", "1.2.1",
 "If you have technical issues with the plugin, please report them by \nemailing Christopher Teubert (christopher.a.teubert@nasa.gov).");
 *      logger.WriteLine(LOG_FATAL, "ClassName", "Error Description");
 *      logger2.FormatLine(LOG_WARNING, "ClassName", "Key %s not recognized. Did you mean %s? Ignoring key", key, otherKey);
 *      logger.close();
 *      logger2.close()
 *
 *  @author Jason Watkins
 *  @author Chris Teubert
 *
 *  @note
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: December 4, 2015
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 **/

#ifndef PCOE_THREADSAFELOG_H
#define PCOE_THREADSAFELOG_H
// *--------------------------------------*
// |             Header Files             |
// *--------------------------------------*

#include <memory>
#include <mutex>
#include <string>

namespace PCOE {
    // *--------------------------------------*
    // |           Consts and Enums           |
    // *--------------------------------------*

    // LOG_VERBOSITY determines the level of logging throughout
    //     OFF: No logging at all will be performed.
    //   FATAL: Critical errors that would normally result in termination of the program.
    //   ERROR: All errors not covered by FATAL
    //    WARN: Potentially, but not definitely, incorrect behavior
    //    INFO: Information about normal actions taken by the plugin.
    //   DEBUG: More verbose information usefull for debugging.
    //   TRACE: Log all the things!
    enum LOG_VERBOSITY {
        LOG_OFF = 0,
        LOG_FATAL = 1,
        LOG_ERROR = 2,
        LOG_WARN = 3,
        LOG_INFO = 4,
        LOG_DEBUG = 5,
        LOG_TRACE = 6
    };

    extern LOG_VERBOSITY LOG_LEVEL;

    // *--------------------------------------*
    // |              Log Class               |
    // *--------------------------------------*

    /**
     *  \details Provides functions to write lines to a generic log file.
     *  \author Jason Watkins, Chris Teubert
     *  \version 2.1
     *  \since 1.0
     *  \date Intial Version: 2015-04-09
     *  \date Last Updated: 2016-04-22
     **/
    class Log {
    private:
        /**
         *  @brief Constructs a log and opens the associated file.
         *
         *  @param path The path at which the log file will be created.
         **/
        explicit Log(const std::string& path);
    public:

        /**
        *  @brief Constructs a copy of the Log that shares a file and
        *         syncronization with the copied Log.
        **/
        Log(const Log& src) = default;

        /**
        *  @brief Constructs a Log by taking the resources of @p src.
        **/
        Log(Log&& src) = default;

        /**
        *  @brief Assigns copies of the resources of @p src to the current Log.
        **/
        Log& operator=(const Log& src) = default;

        /**
        *  @brief Assigns the resources of @p src to the current log.
        **/
        Log& operator=(Log&& src) = default;

        /**
         *  @brief  Get an instance of the default log, written to log.txt
         **/
        static Log& Instance();

        /**
         *  @brief  Get an instance of a custom logger- to a different file
         *  @param  file    The file where the log will be written to
         **/
        static Log& Instance(const std::string & file);

        /**
        *  @brief  Set the verbosity limit for the file. Anything below the limit set will not be logged
        *  @param  verbosity   The new verbosity limit for the file
        **/
        static void SetVerbosity(const LOG_VERBOSITY verbosity);

        /**
         *  @brief  Writes a formatted header to the log
         *  @param  programName     The name of the program writing to the log (or some other identifying name)
         *  @param  versionNumber   Version of the program (Ex: 1.0.1)
         *  @param  note            Any note to be written to the end of the file
         **/
        void Initialize(const std::string & programName, const std::string & versionNumber, const std::string & note) const;

        /**
         *  @brief  Closes the log file
         **/
        void Close();

        /**
         *  @brief  Writes the string pointed to by format, followed by a line
         *          terminator to the log file. If format contains format
         *          specifiers, additional arguments following format will be formatted
         *          and inserted in the resulting string, replacing their respective
         *          specifiers.
         *
         *  @param  level           The verbosity level of the message (from the LOG_VERBOSITY enum
         *  @param  tag             An identifier for who is writing to the log (usually class or file name)
         *  @param  format          The format string appropriate for consumption by sprintf, followed by
         *                          the parameters inserted into the string
         *
         *  @remarks Note that Visual C++ silently fails va_start when the last non-varargs
         *           argument is a reference, so we need a value-type format here.
         *  @note   Note that format cannot be reference- vastart has undefined behavior for reference type
         **/
        void FormatLine(const LOG_VERBOSITY level, const std::string& tag, const std::string format, ...) const;

        /** @brief  Writes the specified string value, followed by a line terminator
         *          to the log file.
         *
         *  @param  level           The verbosity level of the message (from the LOG_VERBOSITY enum
         *  @param  tag             An identifier for who is writing to the log (usually class or file name)
         *  @param  value           The string to write.
         **/
        void WriteLine(const LOG_VERBOSITY level, const std::string& tag, const std::string& value) const;

        /**
         *  @brief  Log text verbatim (without changing anything) to the file
         *  @param  text            The text to be logged
         **/
        void LogVerbatim(const std::string & text) const;

    private:
        std::shared_ptr<std::FILE> fd;
        mutable std::shared_ptr<std::mutex> m;
    };
}

#endif // PCOE_THREADSAFELOG_H
