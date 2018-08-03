/**  Configuration Map - Header
 *   @class     ConfigMap ConfigMap.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Configuration Map- Map for loading accessing, and parsing configuration parameters
 * from a key:value1, value2, ... style file
 *
 *   @author    Chris Teubert <christopher.a.teubert@nasa.gov>
 *   @author    Jason Watkins <jason-watkins@outlook.com>
 *   @author    Julian Vu     <julianvu@outlook.com>
 *   @version   1.1.1
 *   @date      2016-06-22
 *
 *   @copyright Copyright (c) 2013-2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_CONFIGMAP_H
#define PCOE_CONFIGMAP_H

#include <string>
#include <unordered_map>
#include <vector>

namespace PCOE {
    class ConfigMap {
    public:
        ConfigMap() = default;
        ConfigMap(const std::string& filename);
        ConfigMap(const int argc, char* argv[]);

        /**
         * @brief       Gets string vector at given key
         * @param       key     The key from which the vector is retrieved
         * @return      The string vector corresponding to the given key
         */
        const std::vector<std::string>& getVector(const std::string& key) const;

        /**
         * @brief       Gets double at given key
         * @param       key     The key from which the vector is retrieved
         * @return      The double corresponding to the given key
         */
        double getDouble(const std::string& key) const;

        /**
         * @brief       Gets 64-bit unsigned integer at given key
         * @param       key     The key from which the vector is retrieved
         * @return      The 64-bit unsigned integer corresponding to the given key
         */
        std::uint64_t getUInt64(const std::string& key) const;

        /**
         * @brief       Gets 64-bit integer at given key
         * @param       key     The key from which the vector is retrieved
         * @return      The 64-bit integer corresponding to the given key
         */
        std::int64_t getInt64(const std::string& key) const;

        /**
         * @brief       Gets 32-bit unsigned integer at given key
         * @param       key     The key from which the vector is retrieved
         * @return      The 32-bit unsigned integer corresponding to the given key
         */
        std::uint32_t getUInt32(const std::string& key) const;

        /**
         * @brief       Gets 32-bit integer at given key
         * @param       key     The key from which the vector is retrieved
         * @return      The 32-bit integer corresponding to the given key
         */
        std::int32_t getInt32(const std::string& key) const;

        /**
         * @brief       Gets string at given key
         * @param       key     The key from which the vector is retrieved
         * @return      The string corresponding to the given key
         */
        const std::string& getString(const std::string& key) const;

        /** @function   set
         *  @brief      Set a parameter to a vector of strings
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example", std::vector({value1, value2});
         **/
        void set(const std::string& key, const std::vector<std::string>& vector);

        /** @function   set
         *  @brief      Set a parameter to an initializer list
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example", {value1, value2});
         **/
        void set(const std::string& key, const std::initializer_list<std::string> list);

        /** @function   set
         *  @brief      Set a parameter to a double
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example", 2.3);
         **/
        void set(const std::string& key, const double value);

        /** @function   set
         *  @brief      Set a parameter to a 64-bit unsigned int
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example", UINT64_MAX);
         **/
        void set(const std::string& key, const std::uint64_t value);

        /** @function   set
         *  @brief      Set a parameter to a 64-bit int
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example", INT64_MAX);
         **/
        void set(const std::string& key, const std::int64_t value);

        /** @function   set
         *  @brief      Set a parameter to a 32-bit unsigned int
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example", UINT_MAX);
         **/
        void set(const std::string& key, const std::uint32_t value);

        /** @function   set
         *  @brief      Set a parameter to a 32-bit int
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example", INT32_MAX);
         **/
        void set(const std::string& key, const std::int32_t value);

        /** @function   set
         *  @brief      Set a parameter to a string
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example","Text");
         **/
        void set(const std::string& key, const std::string& value);

        /** @function   loadFile
         *  @brief      Load the contents of a file into the configuration map
         *  @param      filename The name of a file located in one of the folders
         *                       added by a call to @see{ConfigMap::addSearchPath},
         *                       or the full path to a config file.
         *  @exception std::runtime_error If the file can't be found.
         **/
        void loadFile(const std::string& filename);

        /** @function   loadArguments
         *  @brief      Load the command line arguments into the configMap
         *  @param      argc    Number of Arguments
         *  @param      argv    Command Line Arguments
         **/
        void loadArguments(const int argc, char* argv[]);

        /** @function hasKey
         *  @brief Check if the specified key is contained in the map.
         *
         *  @param key A string that may be used as a key in the map.
         *  @return    true if the key exists; othwerwise, false.
         **/
        inline bool hasKey(const std::string& key) const {
            return map.find(key) != map.end();
        }

        /**
         * @brief Checks whether all keys contained in the specified list are
         *        contained in the map.
         *
         * @param list The list of keys to check.
         * @return true if all keys in the list are contained in the map;
         *         otherwise, false.
         **/
        bool hasKeys(std::initializer_list<std::string> list) const;

        /** @brief      Add a path in which to look for config files.
         *  @param path A string containing a directory which should be searched
         *              for configuration files.
         *  @exception std::runtime_error If path is not a valid directory.
         */
        static void addSearchPath(const std::string& path);

    private:
        /** @function   parseLine
         *  @brief      Parse a line, and add the result to the configmap
         *  @param      line        The line to be parsed
         **/
        void parseLine(const std::string& line);

        static std::vector<std::string> searchPaths;

        std::unordered_map<std::string, std::vector<std::string>> map;
    };

    /**
     * @brief       Check if all required keys (parameters) are present in the ConfigMap
     * @param map   ConfigMap from which to check against list
     * @param list  List of keys to check
     */
    void requireKeys(const ConfigMap& map, std::initializer_list<std::string> list);
}

#endif // PCOE_CONFIGMAP_H
