// Copyright (c) 2015-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_FACTORY_H
#define PCOE_FACTORY_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "GSAPConfigMap.h"

namespace PCOE {
    /**
     * Provides functionality used by derived classes to track types and
     * construct new objects.
     *
     * @tparam TBase      The type shared by all members that will be registered
     *                    with the factory.
     * @tparam CtorParams A variable number of type parameters specifying the
     *                    number and type of arguments required by constructors
     *                    for this type. All types registered with this factory
     *                    must either have a constructor taking exactly these
     *                    arguments or supply their own creation function that
     *                    is capable of constructing the type from these
     *                    parameters.
     *
     * @example @code
     * Factory<Model> factory;
     * fatory.Register("Battery", Factory<Model>::Create<BatteryModel>);
     * std::unique_ptr<BatteryModel> model = factory.Create("Battery");
     * @endcode
     *
     * @author Chris Teubert
     * @author Jason Watkins
     * @since 1.0
     **/
    template <class TBase, typename... CtorParams>
    class Factory {
    public:
        /**
         * The owned pointer type returned by the @{code Create} method and the
         * @{code create_fn} function wrapper.
         **/
        using unique_ptr = std::unique_ptr<TBase>;

        /**
         * A function wrapper whose target is capable of constructing a type
         * derived from @{code TBase} and returning it as an owned pointer.
         **/
        using create_fn = std::function<unique_ptr(CtorParams&&...)>;

        /**
         * Registers a new concrete type with the factory using an implicit
         * creation function. The implicit function will attempt to call the
         * constructor of @{code TDerived} with the parameters passed to the
         * @{code Create} method. Therefore, types registered using this method
         * must have a constructor whose parameters exactly match the parameters
         * of @{code CtorParams}.
         *
         * @tparam TDerived The concrete type being registered. TDerived must be
         *                  a class that derives from @{code TBase}.
         *
         * @param name The name of the new type. This name must be unique among
         *             the types registered to a given factory.
         */
        template <class TDerived>
        void Register(const std::string& name) {
            registered[name] = Factory::Create<TDerived>;
        }

        /**
         * Registers a new concrete type with the factory using an explicit
         * creation function.
         *
         * @param name The name of the new type. This name must be unique among
         *             the types registered to a given factory.
         * @param fn   A function that returns a new instance of the type being
         *             registered each time it is called.
         */
        void Register(const std::string& name, create_fn fn) {
            registered[name] = fn;
        }

        /**
         * Constructs and returns a new instance of the concrete type previously
         * registered with the specified name.
         *
         * @param name The name of the concrete type to create.
         * @param   config    Parameter map of the contents of the config
         * parameter
         * @return     An owned pointer to the newly created instance.
         */
        unique_ptr Create(const std::string& name, CtorParams&&... args) const {
            return registered.at(name)(std::forward<CtorParams>(args)...);
        }

    private:
        /**
         * @brief   Function to make a component of type2. This function also configures
         *          the newly created component as specified in the same map.
         * @param   config The component config map
         * @return  The newly created and configured component
         *
         * @note    Generally this is used for Registration. Examples:
         *          Factory::Register("type", Create<type>);
         */
        template <class TDerived>
        static unique_ptr Create(CtorParams&&... args) {
            return unique_ptr(new TDerived(std::forward<CtorParams>(args)...));
        }

    private:
        std::unordered_map<std::string, create_fn> registered;
    };
}

#endif
