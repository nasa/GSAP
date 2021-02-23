//
//  CentrifugalPumpModelTest.cpp
//  templates
//
//  Created by Teubert, Christopher (ARC-TI) on 9/26/19.
//

#include "Test.h"
#include "ConfigMap.h"
#include "Models/CircuitBatteryModel.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace CircuitBatteryModelTests {
    
    CircuitBatteryModel setupModel() {
        ConfigMap m;
        
        return CircuitBatteryModel(m);
    }
    
    void testCV() {
        CircuitBatteryModel model = setupModel();
        auto u = model.getInputVector();
        u[0] = 2; // Default load of 2 amperes

        PrognosticsModel::state_type x0 = model.initialize(PrognosticsModel::input_type({}), PrognosticsModel::output_type({}));

        auto x = model.stateEqn(0, x0, u, 1);

        // Compare to results from Matlab Model
        Assert::AreEqual(18.95, x[0], 1e-2);
        Assert::AreEqual(7.8543e3, x[1], 1);
        Assert::AreEqual(2.0004, x[2], 1e-4);
        Assert::AreEqual(2.0004, x[3], 1e-4);

        auto z = model.outputEqn(0, x);
        Assert::AreEqual(18.95, z[0], 1e-2);
        Assert::AreEqual(4.0392, z[1], 1e-4);

        auto f = model.thresholdEqn(0, x);
        Assert::IsFalse(f[0]);
        Assert::IsFalse(f[1]);
        Assert::IsFalse(f[2]);
        Assert::IsFalse(f[3]);
    }
    
    void registerTests(PCOE::Test::TestContext& context) {
        context.AddTest("Model Test", testCV, "Circuit Battery Model");
    }
}
