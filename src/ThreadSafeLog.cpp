/// Copyright (c) 2013-2018 United States Government as represented by the
/// Administrator of the National Aeronautics and Space Administration.
/// All Rights Reserved.

#include "ThreadSafeLog.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>

#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>

// Implementation note(JW): I initially wrote this class using C++ iostreams, but I couldn't find any
// way to implement FormatLine without adding in a call to sprintf. It therefore seems more
// efficient to me to just use C-style IO and call std::fprintf directly.

namespace PCOE {
    const static char DEFAULT_FILE_NAME[] = "Log.txt";

    LOG_VERBOSITY LOG_LEVEL = LOG_DEBUG;

    Log::Log(const std::string& path) : fd(), m(new std::mutex()) {
        std::FILE* ptr = std::fopen(path.c_str(), "w");
        if (ptr == nullptr) {
            throw std::range_error("Unable to open log file.");
        }
        fd = std::shared_ptr<std::FILE>(ptr, std::fclose);
    }

    Log& Log::Instance() {
        static Log instance(DEFAULT_FILE_NAME);

        return instance;
    }

    Log& Log::Instance(const std::string& path) {
        using map = std::unordered_map<std::string, Log>;
        static map logs;

        map::iterator i = logs.find(path);
        if (i == logs.end()) {
            std::FILE* ptr = std::fopen(path.c_str(), "w");
            if (ptr == nullptr) {
                throw std::range_error("Unable to open log file.");
            }
            std::pair<map::iterator, bool> result = logs.emplace(path, Log(path));
            i = result.first;
        }
        return (*i).second;
    }

    inline std::string WriteTime() {
        using namespace std::chrono;

        system_clock::time_point now = system_clock::now();
        std::time_t now_tt = system_clock::to_time_t(now);
        system_clock::time_point now_sec = system_clock::from_time_t(now_tt);
        milliseconds ms = duration_cast<milliseconds>(now - now_sec);
        std::tm * tm = std::localtime(&now_tt);
        // std::unique_ptr<std::tm> tm(std::localtime(&now_tt));

        std::stringstream ss;
        ss << std::setfill('0')
            << std::setw(2) << tm->tm_hour << ":"
            << std::setw(2) << tm->tm_min << ":"
            << std::setw(2) << tm->tm_sec << "."
            << std::setw(3) << ms.count() << "|";

        return ss.str();
    }

    inline std::string WriteLevel(const int level) {
        std::string str;
        switch (level) {
        case LOG_OFF:
            str = "  OFF|";
            break;
        case LOG_FATAL:
            str = "FATAL|";
            break;
        case LOG_ERROR:
            str = "ERROR|";
            break;
        case LOG_WARN:
            str = " WARN|";
            break;
        case LOG_INFO:
            str = " INFO|";
            break;
        case LOG_DEBUG:
            str = "DEBUG|";
            break;
        case LOG_TRACE:
            str = "TRACE|";
            break;
        default:
            str = "  UNK|";
            break;
        }
        return str;
    }

    void Log::Initialize(const std::string & name,
        const std::string & version = "N/A", const std::string & note = "") const {
        if (LOG_LEVEL == LOG_OFF) {
            return;
        }

        // Note: Mode "w" deletes an existing file with the same name. This means that we only
        //       ever get the log from the last run. This matches the way that X-Plane treats its
        //       log.
        if (fd) {
            std::time_t rawtime;
            std::tm* timeinfo;
            std::time(&rawtime);
            timeinfo = std::localtime(&rawtime);

            char timeStr[16] = { 0 };
            std::strftime(timeStr, 16, "%Y-%m-%d", timeinfo);

            std::lock_guard<std::mutex> guard(*m);
            std::fprintf(fd.get(), "%s [Version %s]\n", name.c_str(), version.c_str());
            std::fprintf(fd.get(), "Compiled %s %s\n", __DATE__, __TIME__);
            std::fprintf(fd.get(), "Copyright (c) 2013-2018 United States Government as represented by the\n");
            std::fprintf(fd.get(), "Administrator of the National Aeronautics and Space Administration.\n");
            std::fprintf(fd.get(), "All Rights Reserved.\n\n");

            std::fprintf(fd.get(), "This file contains debugging information about the %s.\n", name.c_str());
            std::fprintf(fd.get(), "%s\n\n", note.c_str());

            std::fprintf(fd.get(), "Log file generated on %s.\n", timeStr);
            std::fflush(fd.get());
        }
    }

    void Log::Close() {
        std::lock_guard<std::mutex> guard(*m);
        if (fd) {
            fd = nullptr;
        }
    }

    void Log::SetVerbosity(const LOG_VERBOSITY verbosity) {
        LOG_LEVEL = verbosity;
    }

    void Log::WriteLine(const LOG_VERBOSITY level, const std::string& tag,
        const std::string& value) const {
        if (level > LOG_LEVEL || !fd) {
            return;
        }

        std::string ss = WriteTime();
        std::string ssL = WriteLevel(level);

        std::lock_guard<std::mutex> guard(*m);
        std::fprintf(fd.get(), "%s%s%s|%s\n", ss.c_str(), ssL.c_str(), tag.c_str(), value.c_str());
        std::fflush(fd.get());
    }

    void Log::FormatLine(const LOG_VERBOSITY level, const std::string& tag,
        const std::string format, ...) const {
        va_list args;

        if (level > LOG_LEVEL || !fd) {
            return;
        }
        va_start(args, format);

        std::string ss = WriteTime();
        std::string ssL = WriteLevel(level);

        std::lock_guard<std::mutex> guard(*m);
        std::fprintf(fd.get(), "%s%s%s|", ss.c_str(), ssL.c_str(), tag.c_str());
        std::vfprintf(fd.get(), format.c_str(), args);
        std::fputc('\n', fd.get());
        std::fflush(fd.get());

        va_end(args);
    }

    void Log::LogVerbatim(const std::string & text) const {
        std::lock_guard<std::mutex> guard(*m);
        std::fprintf(fd.get(), "%s\n", text.c_str());
        std::fflush(fd.get());
    }
}
