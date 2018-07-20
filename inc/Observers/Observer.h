// Copyright (c) 2016-2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_OBSERVER_H
#define PCOE_OBSERVER_H

#include <vector>

#include "Assert.h"
#include "Model.h"
#include "ThreadSafeLog.h"
#include "UData.h"

namespace PCOE {
    /**
     * Represents an object that observes a model's state.
     *
     * @author Mattew Daigle
     * @author Jason Watkins
     * @since 1.0
     **/
    class Observer {
    public:
        /**
         * Constructs a new observer in an uninitialized state.
         *
         * @param model A pointer to a valid model that will be used by the
         *              observer. The observer does not take ownership of the
         *              model.
         **/
        explicit Observer(const Model* model) : model(model) {
            Expect(model != nullptr, "Invalid model");
        }

        /**
         * Default destructor. A virtual default destructor is necessary to
         * ensure that resources in classes inherting from this class are
         * cleaned up, even if the destructor is called through a pointer of the
         * base class type.
         **/
        virtual ~Observer() = default;

        /**
         * When overriden in a derived class, performs implmentation-specific
         * inialization, including setting initial model states and start time.
         *
         * @param t0 Initial time
         * @param x0 Initial model state
         * @param u0 Initial model input
         **/
        virtual void initialize(double t0,
                                const Model::state_type& x0,
                                const Model::input_type& u0) = 0;

        /**
         * When overriden in a derived class, performs a single state estimation
         * with the given model inputs and outputs.
         *
         * @param t The time at which to make a prediction.
         * @param u The model input vector at time @{code timestamp}.
         * @param z The model output vector at time @{code timestamp}.
         **/
        virtual void step(double t, const Model::input_type& u, const Model::output_type& z) = 0;

        /**
         * When overriden in a derived class, returns the current mean state
         * estimate of the observer.
         *
         * @return The last calculated state estimate.
         **/
        virtual const Model::state_type& getStateMean() const = 0;

        /**
         * When overriden in a derived class, returns the current state estimate
         * of the observer, including uncertainty.
         *
         * @return The last calculated state estimate calcualted by the
         *         observer.
         **/
        virtual std::vector<UData> getStateEstimate() const = 0;

        /**
         * When overriden in a derived class, returns the current mean output
         * estimate of the observer.
         *
         * @return The last output estimate calcualted by the observer..
         **/
        virtual const Model::output_type& getOutputMean() const = 0;

        /**
         * Gets the last input used by the observer.
         **/
        inline const Model::input_type& getPrevInput() const {
            return uPrev;
        }

        /**
         * Gets a value indicating whether the observer has been initialized.
         * Initialization is required before calling the @{code step} method.
         *
         * @return true if the the observer has been initialized; otherwise,
         *         false.
         **/
        inline bool isInitialized() const {
            return initialized;
        }

    protected:
        bool initialized = false;

        double lastTime;

        const Model* model;

        Model::input_type uPrev;
    };
}
#endif
