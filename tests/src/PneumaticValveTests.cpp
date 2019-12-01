//
//  PneumaticValveTests.cpp
//  tests
//
//  Created by Teubert, Christopher (ARC-TI) on 12/1/19.
//

#include "Test.h"
#include "ConfigMap.h"
#include "Models/PneumaticValveModel.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace PneumaticValveModelTests {
    PneumaticValveModel setupModel() {
        ConfigMap m;
        
        return PneumaticValveModel(m);
    }
    
    void testCV() {
        PneumaticValveModel model = setupModel();
        auto u = model.getInputVector();
        
        
        u[0] = 3.5e5; // left pressure (Pa)
        u[1] = 2.0e5; // right pressure (Pa)
        u[2] = 0; // top (open)
        u[3] = 1; // bottom (open)
        
        PrognosticsModel::state_type x0 = model.initialize(u, SystemModel::output_type({}));
        
        auto x = model.stateEqn(0, x0, u, 1);
        
        // Compare to results from Matlab Model
        Assert::AreEqual(4e5, x[0], 1);
        Assert::AreEqual(4e5, x[1], 1);
        Assert::AreEqual(0, x[2], 1e-4);
        Assert::AreEqual(4.8e4, x[4], 1);
        Assert::AreEqual(9.4456e-4, x[5], 1e-5);
        Assert::AreEqual(-0.4218, x[6], 1e-4);
        Assert::AreEqual(6e-6, x[7], 1e-7);
        Assert::AreEqual(0, x[8], 1e-4);
        Assert::AreEqual(0, x[9], 1e-4);
        Assert::AreEqual(0, x[10], 1e-4);
        Assert::AreEqual(0, x[11], 1e-4);
        Assert::AreEqual(0, x[12], 1e-4);
        Assert::AreEqual(0, x[13], 1e-4);
        Assert::AreEqual(0, x[14], 1e-4);
        
        // Switch to closed
        u[2] = 1;
        u[3] = 0;
        Assert::AreEqual(4e5, x[0], 1);
        Assert::AreEqual(4e5, x[1], 1);
        Assert::AreEqual(0, x[2], 1e-4);
        Assert::AreEqual(4.8e4, x[4], 1);
        Assert::AreEqual(0.07683, x[5], 1e-5);
        Assert::AreEqual(5.1994, x[6], 1e-4);
        Assert::AreEqual(6e-6, x[7], 1e-7);
        Assert::AreEqual(4923.4, x[8], 0.1);
        Assert::AreEqual(0, x[9], 1e-4);
        Assert::AreEqual(0, x[10], 1e-4);
        Assert::AreEqual(0, x[11], 1e-4);
        Assert::AreEqual(0, x[12], 1e-4);
        Assert::AreEqual(0, x[13], 1e-4);
        Assert::AreEqual(0, x[14], 1e-4);
        
        auto z = model.outputEqn(0, x);
        Assert::AreEqual(0, z[0], 1e-4);
        Assert::AreEqual(1, z[1], 1e-4);
        Assert::AreEqual(0, z[0], 1e-4);
        Assert::AreEqual(8.2418, z[1], 1e-4);
        Assert::AreEqual(403.8711, z[0], 1e-4);
        Assert::AreEqual(0, z[1], 1e-4);
        
        auto f = model.thresholdEqn(0, x);
        Assert::IsFalse(f[0]);
        Assert::IsFalse(f[1]);
        Assert::IsFalse(f[2]);
        Assert::IsFalse(f[3]);
        Assert::IsFalse(f[4]);
    }
    
    void registerTests(PCOE::Test::TestContext& context) {
        context.AddTest("Model Test", testCV, "Pneumatic Valve Model");
    }
}


