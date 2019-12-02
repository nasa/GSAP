//  Created by Sierra Paez, Gina K. (ARC-TI)[SGT, INC] on 10/21/19.
// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include "Test.h"

#include "Models/ECBatteryModel.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace ECBatteryModelTests {
    void validate() {
        const usize state_size = ECBatteryModel::state_size;
        const usize input_size = ECBatteryModel::input_size;
        const usize output_size = ECBatteryModel::output_size;
        ECBatteryModel Battery;
        
        std::array<f64, state_size> n{0, 0, 0};
        std::array<f64, output_size> v{0};
        std::vector<f64> U{10.131757927400000,
            10.5494620456000,
            10.2338801334000,
            226.024048000000,
            210.229040400000,
            203.966940240000,
            205.080275440000,
            203.937359720000,
            201.250222990000,
            196.585093530000
        };
        
        std::vector<f64> Z{12.5707000000000,
            12.4527044636137,
            12.4531653268256,
            11.9590594065535,
            11.9914731566215,
            12.0015259968927,
            11.9942582827677,
            11.9923269994519,
            11.9940962575289,
            12.0006185288799,
            11.9953191982333
        };
        
        f64 dt = 1.0;
        std::array<f64, input_size> u = {U[0]};
        std::array<f64, output_size> z = {Z[0]};
        std::array<f64, state_size> x = Battery.initialize(u, z);
        
        for (usize t = 1; t < U.size(); t++) {
            x = Battery.stateEqn(x, u, n, dt);
            u = {U[t]};
            z = Battery.outputEqn( x, u, v);
            
            Assert::AreEqual(Z[t], z[0], 1.0e-12);
        }
        
        std::array<bool, 1>  thresholdReached = Battery.thresholdEqn(x,u);
    }
    
    void RegisterTests(TestContext& context) {
        context.AddTest("validate", validate, "ECBatteryModelTests");
    }
    
}

