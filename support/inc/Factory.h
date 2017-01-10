/**  Factory - Header
 *   @class     Factory Factory.h
 *   @ingroup   GPIC++
 *   @ingroup   Support
 *
 *   @brief     Abstract Template Factory Class - Factory pattern
 *   Handles the creation of new components. Specific Factorys inherit from this
 *
 *   @author    Chris Teubert
 *   @version   0.1.0
 *
 *   @pre       Prognostic Configuration File and Prognoster Configuration Files
 *
 *      Contact: Chris Teubert (Christopher.a.teubert@nasa.gov)
 *      Created: March 18, 2016
 *
 *   @copyright Copyright (c) 2013-2016 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#ifndef PCOE_FACTORY_H
#define PCOE_FACTORY_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "GSAPConfigMap.h"

namespace PCOE {
    /**
     *  @class      Factory
     *  @brief      Abstract Template Factory Class - Factory pattern
     *   Handles the creation of new components. Specific Factorys inherit from this
     *
     *  @example    class ExampleFactory: public Factory<Example> { ...
     **/
    template<class Type>
    class Factory {
    public:
        using unique_ptr = std::unique_ptr<Type>;

        /**
         * @brief   Function to register a new component to be build
         * @param   name        The name of the component
         * @param   constructor A constructor function to create a new component
         *                      This is typically the Create(ConfigMap &) member function
         * @example Factory::Register("example",Factory.Create(cMap));
         * @return  If the component was created
         */
        bool Register(std::string name, std::function<unique_ptr(GSAPConfigMap&)> constructor) {
            registered[name] = constructor;
            return true;
        }

        /**
         * @brief   Function to make a component of the type specified.
         *          This function also configures the newly created
         *          component as specified in the same file.
         * @param   name        The name of the component
         * @param   paramMap    Parameter map of the contents of the config parameter
         * @return  The newly created and configured component
         */
        unique_ptr Create(std::string name, GSAPConfigMap & paramMap) {
            return registered.at(name)(paramMap);
        }

        /**
         * @brief   Function to make a component of the type specified in the
         *          configuration file. This function also configures the newly created
         *          component as specified in the same file.
         * @param   filename The component config file
         * @return  The newly created and configured component
         */
        unique_ptr Create(std::string filename) {
            GSAPConfigMap configValues(filename);

            const std::string& typeName = configValues.at("type")[0];
            std::function<unique_ptr(GSAPConfigMap&)> typeCtor = registered.at(typeName);
            if (!typeCtor) {
                throw std::domain_error("Undefined type " + typeName);
            }
            return typeCtor(configValues);
        }

        /**
         * @brief   Function to make a component of type2. This function also configures
         *          the newly created component as specified in the same map.
         * @param   config The component config map
         * @return  The newly created and configured component
         *
         * @note    Generally this is used for Registration. Examples:
         *          Factory::Register("type", Create<type>);
         */
        template <class type2>
        static unique_ptr Create(GSAPConfigMap & config) {
            return unique_ptr(new type2(config));
        }

    private:
        std::unordered_map<std::string, std::function<unique_ptr(GSAPConfigMap&)>> registered;  ///< Registered components
    };
}

#endif // PCOE_FACTORY_H
