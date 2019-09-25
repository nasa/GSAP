// @copyright Copyright (c) 2013-2018 United States Government as represented by
// the Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <algorithm>
#include <cctype>
#include <climits>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <sys/stat.h>
#include <vector>

#include "ConfigMap.h"

namespace PCOE {
    std::vector<std::string> ConfigMap::searchPaths;

    const std::string IMPORT_KEY = "importConfig";

    void trim(std::string& str) {
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
        }
        else if (back <= front) {
            // All whitespace
            str.clear();
        }
        else {
            // Whitespace on both ends
            diff_type frontDiff = static_cast<diff_type>(front);
            diff_type backDiff = static_cast<diff_type>(back);
            str = std::string(str.begin() + frontDiff, str.begin() + backDiff);
        }
    }

    ConfigMap::ConfigMap(const std::string& filename) {
        loadFile(filename);
    }

    ConfigMap::ConfigMap(const int argc, char* argv[]) {
        loadArguments(argc, argv);
    }

    const std::vector<std::string>& ConfigMap::getVector(const std::string& key) const {
        return map.at(key);
    }

    std::vector<double> ConfigMap::getDoubleVector(const std::string& key) const {
        auto& strings = map.at(key);
        std::vector<double> doubles(strings.size());
        for (std::size_t i = 0; i < strings.size(); ++i) {
            doubles[i] = std::stod(strings[i]);
        }
        return doubles;
    }

    double ConfigMap::getDouble(const std::string& key) const {
        return std::stod(getString(key));
    }

    std::size_t ConfigMap::getSize(const std::string& key) const {
        return static_cast<std::size_t>(std::stoull(getString(key)));
    }

    std::uint64_t ConfigMap::getUInt64(const std::string& key) const {
        return std::stoull(getString(key));
    }

    std::int64_t ConfigMap::getInt64(const std::string& key) const {
        return std::stoll(getString(key));
    }

    std::uint32_t ConfigMap::getUInt32(const std::string& key) const {
        return std::stoul(getString(key));
    }

    std::int32_t ConfigMap::getInt32(const std::string& key) const {
        return std::stoi(getString(key));
    }

    const std::string& ConfigMap::getString(const std::string& key) const {
        if (map.at(key).size() > 1) {
            throw std::range_error("Cannot get " + key + " as it maps to a vector.");
        }
        else if (map.at(key).empty()) {
            throw std::out_of_range("");
        }
        return map.at(key)[0];
    }

    void ConfigMap::set(const std::string& key, const std::vector<std::string>& vector) {
        map[key] = vector;
    }

    void ConfigMap::set(const std::string& key, const std::initializer_list<std::string> list) {
        map[key] = list;
    }

    void ConfigMap::set(const std::string& key, const double value) {
        set(key, {std::to_string(value)});
    }

    void ConfigMap::set(const std::string& key, const std::uint64_t value) {
        set(key, {std::to_string(value)});
    }

    void ConfigMap::set(const std::string& key, const std::int64_t value) {
        set(key, {std::to_string(value)});
    }

    void ConfigMap::set(const std::string& key, const std::uint32_t value) {
        set(key, {std::to_string(value)});
    }

    void ConfigMap::set(const std::string& key, const std::int32_t value) {
        set(key, {std::to_string(value)});
    }

    void ConfigMap::set(const std::string& key, const std::string& value) {
        map[key] = {value};
    }

    void ConfigMap::loadFile(const std::string& filename) {
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

    // This function parses the command line arguments to the program and stores
    // them in the map. The function expects the exact arguments given to the
    // program's main, so it assumes that the first value in argv is the program
    // name.
    // At some point in the past there was a requirement that options provided
    // on the command-line be distinguishable from options provided in a config
    // file, so the leading dash in option names is retained.
    // The command line args consist of any number of mixed values and
    // key/value pairs. Key/value pairs take the form of "-KEY VALUE", while
    // bare values are any item in argv not directly preceded by a an item
    // starting with a dash.
    void ConfigMap::loadArguments(const int argc, char* argv[]) {
        for (auto i = 1; i < argc; i++) {
            std::string key = "-NO_KEY";
            if (argv[i][0] == '-') {
                key = argv[i];
                i += 1;
                if (!(i < argc)) {
                    throw std::runtime_error("Invalid argument");
                }
            }

            std::string value = argv[i];
            map[key].push_back(value);
        }
    }

    bool ConfigMap::hasKeys(std::initializer_list<std::string> list) const {
        for (auto& elem : list) {
            if (!hasKey(elem)) {
                return false;
            }
        }
        return true;
    }

    void ConfigMap::parseLine(const std::string& line) {
        using size_type = std::string::size_type;
        if (line.empty() || line[0] == '#' || line[0] == '\r' || line[0] == '\n' ||
            line[0] == '/') {
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
        // If the key is IMPORT_KEY, Import the specified files into this configuration map
        if (kv.first.compare(IMPORT_KEY) == 0) {
            for (auto&& file : kv.second) {
                loadFile(file);
            }
        }
        else {
            map.insert(std::move(kv));
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
        }
        else {
            searchPaths.push_back(path);
        }
    }

    void requireKeys(const ConfigMap& map, std::initializer_list<std::string> list) {
        std::string missingKeys;

        for (std::string key : list) {
            if (!map.hasKey(key)) {
                missingKeys += key + ", ";
            }
        }
        missingKeys = missingKeys.substr(0, missingKeys.size() - 2);

        if (!missingKeys.empty()) {
            throw std::range_error("Missing required configuration parameters: " + missingKeys);
        }
    }
}
