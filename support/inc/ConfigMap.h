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
    class ConfigMap : public std::unordered_map<std::string, std::vector<std::string>> {
    public:
        ConfigMap() = default;
        ConfigMap(const std::string& filename);
        ConfigMap(const int argc, char* argv[]);

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

        /** @function   set
         *  @brief      Set a parameter to a string
         *  @param      key     Identifier for the parameter
         *  @param      value   Value that the parameter should be set to
         *
         *  @example    config.set("example","Text");
         **/
        void set(const std::string& key, const std::string& value);

        /** @function   includes
         *  @brief      Check if a specific key is included in the map. This
         *              function is deprecated and will be removed in a future
         *              version of GSAP.
         *  @param      key     Identifier to be checked
         *  @return     If the key exists (bool)
         **/
        inline bool includes(const std::string& key) const {
            return containsKey(key);
        }

        inline bool includes(std::initializer_list<std::string> list) const {
            return containsAllKeys(list);
        }

        /** @function containsKey
         *  @brief Check if the specified key is contained in the map.
         *
         *  @param key A string that may be used as a key in the map.
         *  @return    true if the key exists; othwerwise, false.
         **/
        inline bool containsKey(const std::string& key) const {
            return find(key) != end();
        }

        /**
         * @brief Checks whether all keys contained in the specified list are
         *        contained in the map.
         *
         * @param list The list of keys to check.
         * @return true if all keys in the list are contained in the map;
         *         otherwise, false.
         **/
        bool containsAllKeys(std::initializer_list<std::string> list) const;

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
    };
}

#endif // PCOE_CONFIGMAP_H
