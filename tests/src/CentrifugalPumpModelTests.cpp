//
//  CentrifugalPumpModelTest.cpp
//  templates
//
//  Created by Teubert, Christopher (ARC-TI) on 9/26/19.
//

#include "Test.h"
#include "ConfigMap.h"
#include "Models/CentrifugalPumpModel.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace CentrifugalPumpModelTests {
    
    CentrifugalPumpModel setupModel() {
        ConfigMap m;
        m.set("CentrifugalPump.v.w", 0);
        m.set("CentrifugalPump.v.Q", 0);
        m.set("CentrifugalPump.v.Tt", 0);
        m.set("CentrifugalPump.v.Tr", 0);
        m.set("CentrifugalPump.v.To", 0);
        m.set("CentrifugalPump.v.A", 0);
        m.set("CentrifugalPump.v.rThrust", 0);
        m.set("CentrifugalPump.v.rRadial", 0);
        
        return CentrifugalPumpModel(m);
    }
    
    void testCV() {
        CentrifugalPumpModel model = setupModel();
        auto u = model.getInputVector();
        u[0] = 290;
        u[1] = 471.239;
        u[2] = 928654;
        u[3] = 239179;
        u[4] = 376.991;
        
        auto & x0struct = model.parameters.x0;
        PrognosticsModel::state_type x0({x0struct.A, x0struct.Q, x0struct.To, x0struct.Tr,  x0struct.Tt,  x0struct.rRadial, x0struct.rThrust, x0struct.w, x0struct.wA, x0struct.wRadial, x0struct.wThrust});
        
        auto x = model.stateEqn(0, x0, u, model.parameters.sampleTime);
        
        // Compare to results from Matlab Model
        Assert::AreEqual(12.7084, x[0], 1e-4);
        Assert::AreEqual(0.0174, x[1], 1e-4);
        Assert::AreEqual(290, x[2], 1e-4);
        Assert::AreEqual(290.1066, x[3], 1e-4);
        Assert::AreEqual(290.0273, x[4], 1e-4);
        Assert::AreEqual(1.8000e-06, x[5], 1e-7);
        Assert::AreEqual(1.4000e-06, x[6], 1e-7);
        Assert::AreEqual(372.6896, x[7], 1e-4);
        Assert::AreEqual(0, x[8], 1e-7);
        Assert::AreEqual(0, x[9], 1e-7);
        Assert::AreEqual(0, x[10], 1e-7);
        
        auto z = model.outputEqn(0, x);
        Assert::AreEqual(0.0174, z[0], 1e-4);
        Assert::AreEqual(290, z[1], 1e-4);
        Assert::AreEqual(290.1066, z[2], 1e-4);
        Assert::AreEqual(290.0273, z[3], 1e-4);
        Assert::AreEqual(372.6896, z[4], 1e-4);
        
        auto f = model.thresholdEqn(0, x);
        Assert::IsFalse(f[0]);
        Assert::IsFalse(f[1]);
        Assert::IsFalse(f[2]);
        Assert::IsFalse(f[3]);
    }
    
    void registerTests(PCOE::Test::TestContext& context) {
        context.AddTest("Model Test", testCV, "Centrifugal Pump Model");
    }
}
