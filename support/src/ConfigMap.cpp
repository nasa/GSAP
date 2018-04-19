/**  Configuration Map - Body
 *   @class     ConfigMap ConfigMap.h
 *
 *   @brief     Configuration Map- Map for loading accessing, and parsing configuration parameters from a key:value1, value2, ... style file
 *
 *   @author    Chris Teubert <christopher.a.teubert@nasa.gov>
 *   @author    Jason Watkins <jason-watkins@outlook.com>
 *   @version   1.1.1
 *   @date      2016-06-22
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <string>
#include <vector>
#include <utility>  // For Pair
#include <sys/types.h>
#include <sys/stat.h>

#include "ConfigMap.h"

namespace PCOE {
    std::vector<std::string> ConfigMap::searchPaths;

    void trim(std::string& str)
    {
        using size_type = std::string::size_type;
        using diff_type = std::string::difference_type;
        if (str.empty()) {
            return;
        }

        const char* ptr = str.c_str();

        size_type front = 0;
        while (front < str.length() && std::isspace(int(ptr[front]))) {
            ++front;
        }

        size_type back = str.length();
        while (back > front && std::isspace(int(ptr[back - 1]))) {
            --back;
        }

        if (0 == front) {
            if (back < str.length()) {
                // Trailing whitespace only
                str.resize(back - front);
            }
            // No whitespace
        } else if (back <= front) {
            // All whitespace
            str.clear();
        } else {
            // Whitespace on both ends
            diff_type frontDiff = static_cast<diff_type>(front);
            diff_type backDiff = static_cast<diff_type>(back);
            str = std::string(str.begin() + frontDiff, str.begin() + backDiff);
        }
    }
    
    ConfigMap::ConfigMap(const std::string & filename) {
        loadFile(filename);
    }
    
    ConfigMap::ConfigMap(const int argc, char *argv[]) {
        loadArguments(argc, argv);
    }

    void ConfigMap::loadFile(const std::string & filename) {
        // Open file; First check the working directory, then try each path in searchPaths
        std::ifstream file(filename);
        for (auto i = searchPaths.cbegin(); !file.good() && i != searchPaths.cend(); ++i) {
            // Note that the values in search path are already sanatized to
            // ensure that they have a trailing '/'
            std::string path = *i + filename;
            file.open(path);
        }
        if (!file.good()) {
            // Could not find file
            throw std::ios_base::failure("Invalid Configuration file");
        }

        // Read the file one line at a time
        while (file) {
            std::string line;
            if (!getline(file, line)) {
                break; // End of File
            }
            parseLine(line);
        }
    }
    
    void ConfigMap::loadArguments(const int argc, char *argv[]) {
        for (auto i = 1; i < argc; i++) {
            if ((i < argc) && (argv[i][0] == '-')) {
                (*this)[argv[i]].push_back(argv[i+1]);
                i++;
            } else {
                (*this)["-NO_KEY"].push_back(argv[i]);
            }
        }
    }

    void ConfigMap::set(const std::string & key, const std::string & value) {
        (*this)[key] = { value };
    }
    
    bool ConfigMap::includes(std::initializer_list<std::string> list) const {
        for (auto & elem : list) {
            // Check if each element is there
            if (!includes(elem)) {
                return false;
            }
        }
        return true;
    }

    void ConfigMap::parseLine(const std::string & line) {
        using size_type = std::string::size_type;
        if (line.empty() || line[0] == '#' || line[0] == '\r' || line[0] == '\n' || line[0] == '/') {
            return; // Comment or empty line
        }
        size_type pos = line.find_first_of(':');
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid Configuration line, missing ':' character.");
        }

        std::pair<std::string, std::vector<std::string>> kv;
        kv.first = line.substr(0, pos);
        trim(kv.first);
        size_type endOfValue = pos;
        for (++pos; endOfValue != std::string::npos; pos = endOfValue + 1) {
            endOfValue = line.find(',', pos);
            // find will return npos if not ',' is found, but npos is defined to be
            // the maximum value that size_type can hold, so end-pos is still safe
            std::string s(line.substr(pos, endOfValue - pos));
            trim(s);
            if (!s.empty()) {
                kv.second.push_back(s);
            }
        }
        if (kv.first.compare("importConfig") == 0) {
            for (auto && file : kv.second) { // If this line is pointing to other files to load
                loadFile(file);
            }
        } else {
            insert(std::move(kv));
        }
    }

    void ConfigMap::addSearchPath(const std::string& path) {
        // Check if path exists and is a directory
        struct stat info;
        if (stat(path.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
            throw std::domain_error("'" + path + "' is not a valid directory.");
        }

        // Make sure path ends with a seperator so that we can concatenate it
        // directly with a filename later
        if (path.back() != '/' && path.back() != '\\') {
            searchPaths.push_back(path + "/");
        } else {
            searchPaths.push_back(path);
        }
    }
}
