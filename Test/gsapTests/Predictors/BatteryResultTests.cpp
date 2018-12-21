// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <numeric>

#include "MockClasses.h"
#include "Test.h"

#include "ConfigMap.h"
#include "Loading/ConstLoadEstimator.h"
#include "Models/BatteryModel.h"
#include "Observers/UnscentedKalmanFilter.h"
#include "Predictors/MonteCarloPredictor.h"
#include "Trajectory/TrajectoryService.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace BatteryResultTests {
    void testConstLoad() {
        ConfigMap config;
        config.set("Observer.Q",
                   {"1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    ""
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    ""
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "0",
                    "1e-10"});
        config.set("Observer.R", {"1e-2", "0", "0", "1e-2"});
        config.set("Predictor.SampleCount", "100");
        config.set("Predictor.Horizon", "10000");
        // Note (JW): This value is actually required by the MC predictor, not the model
        config.set("Model.ProcessNoise",
                   {"1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5"});
        config.set("LoadEstimator.Loading", "8");

        TrajectoryService trajectoryService;
        BatteryModel model(config);
        ConstLoadEstimator loadEstimator(config);
        UnscentedKalmanFilter observer(model, config);
        MonteCarloPredictor predictor(model, loadEstimator, trajectoryService, config);

        // Data taken from the PlaybackScn2Constant.txt file from the example
        double t = 0.0;
        auto u = model.getInputVector();
        u[0] = 0.0; // Watts
        auto z = model.getOutputVector();
        z[0] = 20.00; // Volts
        z[1] = 4.10; // Degrees C
        auto x = model.initialize(u, z);
        observer.initialize(0.0, x, u);

        // Note (JW): The expected values for all of these are derived
        // emperically from running the this test with a "known good" version of
        // GSAP. It's quite possible that changes to GSAP could cause subtle
        // differences that aren't actually bugs. Consult your local PhD in the
        // case of confusing failures, especially if your deltas are small.
        t = 1.0;
        u[0] = 8.00;
        z[0] = 18.74;
        z[1] = 4.05;
        observer.step(t, u, z);
        auto x_est = observer.getStateEstimate();
        Assert::AreEqual(293.15, x_est[0].get(0), 1e-6, "t=1, x_est[0], mean");
        Assert::AreEqual(2e-10, x_est[0].get(1), 1e-5, "t=1, x_est[0], covariance 1");
        Assert::AreEqual(2.42999e-23, x_est[0].get(2), 1e-5, "t=1, x_est[0], covariance 2");
        Assert::AreEqual(3.47238e-23, x_est[0].get(3), 1e-5, "t=1, x_est[0], covariance 3");
        Assert::AreEqual(3.33107e-23, x_est[0].get(4), 1e-5, "t=1, x_est[0], covariance 4");
        Assert::AreEqual(-2.0441e-28, x_est[0].get(5), 1e-5, "t=1, x_est[0], covariance 5");
        Assert::AreEqual(4.08682e-27, x_est[0].get(6), 1e-5, "t=1, x_est[0], covariance 6");
        Assert::AreEqual(2.10201e-25, x_est[0].get(7), 1e-5, "t=1, x_est[0], covariance 7");
        Assert::AreEqual(3.31192e-26, x_est[0].get(8), 1e-5, "t=1, x_est[0], covariance 8");

        Assert::AreEqual(3.48341e-10, x_est[1].get(0), 1e-5, "t=1, x_est[1], mean");
        Assert::AreEqual(2.42999e-23, x_est[1].get(1), 1e-5, "t=1, x_est[1], covariance 1");
        Assert::AreEqual(1.69841e-10, x_est[1].get(2), 1e-5, "t=1, x_est[1], covariance 2");
        Assert::AreEqual(-6.97013e-19, x_est[1].get(3), 1e-5, "t=1, x_est[1], covariance 3");
        Assert::AreEqual(-6.68648e-19, x_est[1].get(4), 1e-5, "t=1, x_est[1], covariance 4");
        Assert::AreEqual(4.10313e-24, x_est[1].get(5), 1e-5, "t=1, x_est[1], covariance 5");
        Assert::AreEqual(4.76841e-23, x_est[1].get(6), 1e-5, "t=1, x_est[1], covariance 6");
        Assert::AreEqual(-6.83673e-23, x_est[1].get(7), 1e-5, "t=1, x_est[1], covariance 7");
        Assert::AreEqual(-7.94524e-22, x_est[1].get(8), 1e-5, "t=1, x_est[1], covariance 8");

        Assert::AreEqual(4.97769e-10, x_est[2].get(0), 1e-5, "t=1, x_est[2], mean");
        Assert::AreEqual(3.47238e-23, x_est[2].get(1), 1e-5, "t=1, x_est[2], covariance 1");
        Assert::AreEqual(-6.97013e-19, x_est[2].get(2), 1e-5, "t=1, x_est[2], covariance 2");
        Assert::AreEqual(1.998e-10, x_est[2].get(3), 1e-5, "t=1, x_est[2], covariance 3");
        Assert::AreEqual(-9.55478e-19, x_est[2].get(4), 1e-5, "t=1, x_est[2], covariance 4");
        Assert::AreEqual(5.86326e-24, x_est[2].get(5), 1e-5, "t=1, x_est[2], covariance 5");
        Assert::AreEqual(6.81392e-23, x_est[2].get(6), 1e-5, "t=1, x_est[2], covariance 6");
        Assert::AreEqual(-9.7695e-23, x_est[2].get(7), 1e-5, "t=1, x_est[2], covariance 7");
        Assert::AreEqual(-1.13535e-21, x_est[2].get(8), 1e-5, "t=1, x_est[2], covariance 8");

        Assert::AreEqual(4.77512e-10, x_est[3].get(0), 1e-5, "t=1, x_est[3], mean");
        Assert::AreEqual(3.33107e-23, x_est[3].get(1), 1e-5, "t=1, x_est[3], covariance 1");
        Assert::AreEqual(-6.68648e-19, x_est[3].get(2), 1e-5, "t=1, x_est[3], covariance 2");
        Assert::AreEqual(-9.55478e-19, x_est[3].get(3), 1e-5, "t=1, x_est[3], covariance 3");
        Assert::AreEqual(1.95739e-10, x_est[3].get(4), 1e-5, "t=1, x_est[3], covariance 4");
        Assert::AreEqual(5.62465e-24, x_est[3].get(5), 1e-5, "t=1, x_est[3], covariance 5");
        Assert::AreEqual(6.53662e-23, x_est[3].get(6), 1e-5, "t=1, x_est[3], covariance 6");
        Assert::AreEqual(-9.37192e-23, x_est[3].get(7), 1e-5, "t=1, x_est[3], covariance 7");
        Assert::AreEqual(-1.08915e-21, x_est[3].get(8), 1e-5, "t=1, x_est[3], covariance 8");

        Assert::AreEqual(6273.42, x_est[4].get(0), 1e-5, "t=1, x_est[4], mean");
        Assert::AreEqual(-2.0441e-28, x_est[4].get(1), 1e-5, "t=1, x_est[4], covariance 1");
        Assert::AreEqual(4.10313e-24, x_est[4].get(2), 1e-5, "t=1, x_est[4], covariance 2");
        Assert::AreEqual(5.86326e-24, x_est[4].get(3), 1e-5, "t=1, x_est[4], covariance 3");
        Assert::AreEqual(5.62465e-24, x_est[4].get(4), 1e-5, "t=1, x_est[4], covariance 4");
        Assert::AreEqual(1.98929e-10, x_est[4].get(5), 1e-5, "t=1, x_est[4], covariance 5");
        Assert::AreEqual(7.42001e-12, x_est[4].get(6), 1e-5, "t=1, x_est[4], covariance 6");
        Assert::AreEqual(5.75105e-28, x_est[4].get(7), 1e-5, "t=1, x_est[4], covariance 7");
        Assert::AreEqual(6.68352e-27, x_est[4].get(8), 1e-5, "t=1, x_est[4], covariance 8");

        Assert::AreEqual(697.047, x_est[5].get(0), 1e-2, "t=1, x_est[5], mean");
        Assert::AreEqual(4.08682e-27, x_est[5].get(1), 1e-5, "t=1, x_est[5], covariance 1");
        Assert::AreEqual(4.76841e-23, x_est[5].get(2), 1e-5, "t=1, x_est[5], covariance 2");
        Assert::AreEqual(6.81392e-23, x_est[5].get(3), 1e-5, "t=1, x_est[5], covariance 3");
        Assert::AreEqual(6.53662e-23, x_est[5].get(4), 1e-5, "t=1, x_est[5], covariance 4");
        Assert::AreEqual(7.42001e-12, x_est[5].get(5), 1e-5, "t=1, x_est[5], covariance 5");
        Assert::AreEqual(1.86231e-10, x_est[5].get(6), 1e-5, "t=1, x_est[5], covariance 6");
        Assert::AreEqual(4.20274e-25, x_est[5].get(7), 1e-5, "t=1, x_est[5], covariance 7");
        Assert::AreEqual(6.4747e-26, x_est[5].get(8), 1e-5, "t=1, x_est[5], covariance 8");

        Assert::AreEqual(5126.58, x_est[6].get(0), 1e-5, "t=1, x_est[6], mean");
        Assert::AreEqual(2.10201e-25, x_est[6].get(1), 1e-5, "t=1, x_est[6], covariance 1");
        Assert::AreEqual(-6.83673e-23, x_est[6].get(2), 1e-5, "t=1, x_est[6], covariance 2");
        Assert::AreEqual(-9.7695e-23, x_est[6].get(3), 1e-5, "t=1, x_est[6], covariance 3");
        Assert::AreEqual(-9.37192e-23, x_est[6].get(4), 1e-5, "t=1, x_est[6], covariance 4");
        Assert::AreEqual(5.75105e-28, x_est[6].get(5), 1e-5, "t=1, x_est[6], covariance 5");
        Assert::AreEqual(4.20274e-25, x_est[6].get(6), 1e-5, "t=1, x_est[6], covariance 6");
        Assert::AreEqual(1.98929e-10, x_est[6].get(7), 1e-5, "t=1, x_est[6], covariance 7");
        Assert::AreEqual(7.42001e-12, x_est[6].get(8), 1e-5, "t=1, x_est[6], covariance 8");

        Assert::AreEqual(569.62, x_est[7].get(0), 1e-5, "t=1, x_est[7], mean");
        Assert::AreEqual(3.31192e-26, x_est[7].get(1), 1e-5, "t=1, x_est[7], covariance 1");
        Assert::AreEqual(-7.94524e-22, x_est[7].get(2), 1e-5, "t=1, x_est[7], covariance 2");
        Assert::AreEqual(-1.13535e-21, x_est[7].get(3), 1e-5, "t=1, x_est[7], covariance 3");
        Assert::AreEqual(-1.08915e-21, x_est[7].get(4), 1e-5, "t=1, x_est[7], covariance 4");
        Assert::AreEqual(6.68352e-27, x_est[7].get(5), 1e-5, "t=1, x_est[7], covariance 5");
        Assert::AreEqual(6.4747e-26, x_est[7].get(6), 1e-5, "t=1, x_est[7], covariance 6");
        Assert::AreEqual(7.42001e-12, x_est[7].get(7), 1e-5, "t=1, x_est[7], covariance 7");
        Assert::AreEqual(1.86231e-10, x_est[7].get(8), 1e-5, "t=1, x_est[7], covariance 8");
        auto prediction = predictor.predict(t, x_est);
        auto event = prediction.getEvents().front().getTOE().getVec();
        std::sort(event.begin(), event.end());
        Assert::IsTrue(event.front() < 2500, "Lowest event time");
        Assert::IsTrue(event.back() > 2750, "Highest event time floor");
        Assert::IsTrue(event.back() < 3000, "Highest event time ceiling");
        double mean = std::accumulate(event.begin(), event.end(), 0.0) / event.size();
        Assert::AreEqual(mean, 2700, 100, "Mean ToE");

        t = 2.0;
        u[0] = 8.00;
        z[0] = 18.68;
        z[1] = 4.03;
        observer.step(t, u, z);
        x_est = observer.getStateEstimate();

        Assert::AreEqual(293.15, x_est[0].get(0), 1e-5, "t=1, x_est[0], mean");
        Assert::AreEqual(2e-10, x_est[0].get(1), 1e-5, "t=1, x_est[0], covariance 1");
        Assert::AreEqual(-3.18892e-17, x_est[0].get(2), 1e-5, "t=1, x_est[0], covariance 2");
        Assert::AreEqual(2.56604e-17, x_est[0].get(3), 1e-5, "t=1, x_est[0], covariance 3");
        Assert::AreEqual(2.15026e-17, x_est[0].get(4), 1e-5, "t=1, x_est[0], covariance 4");
        Assert::AreEqual(-1.99078e-28, x_est[0].get(5), 1e-5, "t=1, x_est[0], covariance 5");
        Assert::AreEqual(1.65602e-15, x_est[0].get(6), 1e-5, "t=1, x_est[0], covariance 6");
        Assert::AreEqual(-4.83955e-26, x_est[0].get(7), 1e-5, "t=1, x_est[0], covariance 7");
        Assert::AreEqual(-1.65602e-15, x_est[0].get(8), 1e-5, "t=1, x_est[0], covariance 8");

        Assert::AreEqual(0.0375768, x_est[1].get(0), 1e-5, "t=1, x_est[1], mean");
        Assert::AreEqual(-3.18892e-17, x_est[1].get(1), 1e-5, "t=1, x_est[1], covariance 1");
        Assert::AreEqual(1.71398e-10, x_est[1].get(2), 1e-5, "t=1, x_est[1], covariance 2");
        Assert::AreEqual(9.16579e-13, x_est[1].get(3), 1e-5, "t=1, x_est[1], covariance 3");
        Assert::AreEqual(8.98124e-13, x_est[1].get(4), 1e-5, "t=1, x_est[1], covariance 4");
        Assert::AreEqual(-5.1835e-18, x_est[1].get(5), 1e-5, "t=1, x_est[1], covariance 5");
        Assert::AreEqual(-4.1083e-11, x_est[1].get(6), 1e-5, "t=1, x_est[1], covariance 6");
        Assert::AreEqual(8.63688e-17, x_est[1].get(7), 1e-5, "t=1, x_est[1], covariance 7");
        Assert::AreEqual(4.10841e-11, x_est[1].get(8), 1e-5, "t=1, x_est[1], covariance 8");

        Assert::AreEqual(7.53376e-05, x_est[2].get(0), 1e-5, "t=1, x_est[2], mean");
        Assert::AreEqual(2.56604e-17, x_est[2].get(1), 1e-5, "t=1, x_est[2], covariance 1");
        Assert::AreEqual(9.16579e-13, x_est[2].get(2), 1e-5, "t=1, x_est[2], covariance 2");
        Assert::AreEqual(1.99803e-10, x_est[2].get(3), 1e-5, "t=1, x_est[2], covariance 3");
        Assert::AreEqual(2.62552e-15, x_est[2].get(4), 1e-5, "t=1, x_est[2], covariance 4");
        Assert::AreEqual(4.59869e-20, x_est[2].get(5), 1e-5, "t=1, x_est[2], covariance 5");
        Assert::AreEqual(-4.75476e-11, x_est[2].get(6), 1e-5, "t=1, x_est[2], covariance 6");
        Assert::AreEqual(1.04722e-19, x_est[2].get(7), 1e-5, "t=1, x_est[2], covariance 7");
        Assert::AreEqual(4.75476e-11, x_est[2].get(8), 1e-5, "t=1, x_est[2], covariance 8");

        Assert::AreEqual(2.76897e-05, x_est[3].get(0), 1e-5, "t=1, x_est[3], mean");
        Assert::AreEqual(2.15026e-17, x_est[3].get(1), 1e-5, "t=1, x_est[3], covariance 1");
        Assert::AreEqual(8.98124e-13, x_est[3].get(2), 1e-5, "t=1, x_est[3], covariance 2");
        Assert::AreEqual(2.62552e-15, x_est[3].get(3), 1e-5, "t=1, x_est[3], covariance 3");
        Assert::AreEqual(1.95742e-10, x_est[3].get(4), 1e-5, "t=1, x_est[3], covariance 4");
        Assert::AreEqual(-8.70144e-21, x_est[3].get(5), 1e-5, "t=1, x_est[3], covariance 5");
        Assert::AreEqual(-4.65707e-11, x_est[3].get(6), 1e-5, "t=1, x_est[3], covariance 6");
        Assert::AreEqual(-2.29958e-19, x_est[3].get(7), 1e-5, "t=1, x_est[3], covariance 7");
        Assert::AreEqual(4.65707e-11, x_est[3].get(8), 1e-5, "t=1, x_est[3], covariance 8");

        Assert::AreEqual(6273.42, x_est[4].get(0), 1e-5, "t=1, x_est[4], mean");
        Assert::AreEqual(-1.99078e-28, x_est[4].get(1), 1e-5, "t=1, x_est[4], covariance 1");
        Assert::AreEqual(-5.1835e-18, x_est[4].get(2), 1e-5, "t=1, x_est[4], covariance 2");
        Assert::AreEqual(4.59869e-20, x_est[4].get(3), 1e-5, "t=1, x_est[4], covariance 3");
        Assert::AreEqual(-8.70144e-21, x_est[4].get(4), 1e-5, "t=1, x_est[4], covariance 4");
        Assert::AreEqual(1.98929e-10, x_est[4].get(5), 1e-5, "t=1, x_est[4], covariance 5");
        Assert::AreEqual(7.42027e-12, x_est[4].get(6), 1e-5, "t=1, x_est[4], covariance 6");
        Assert::AreEqual(5.82811e-28, x_est[4].get(7), 1e-5, "t=1, x_est[4], covariance 7");
        Assert::AreEqual(-2.69183e-16, x_est[4].get(8), 1e-5, "t=1, x_est[4], covariance 8");

        Assert::AreEqual(695.095, x_est[5].get(0), 1e-2, "t=1, x_est[5], mean");
        Assert::AreEqual(1.65602e-15, x_est[5].get(1), 1e-5, "t=1, x_est[5], covariance 1");
        Assert::AreEqual(-4.1083e-11, x_est[5].get(2), 1e-5, "t=1, x_est[5], covariance 2");
        Assert::AreEqual(-4.75476e-11, x_est[5].get(3), 1e-5, "t=1, x_est[5], covariance 3");
        Assert::AreEqual(-4.65707e-11, x_est[5].get(4), 1e-5, "t=1, x_est[5], covariance 4");
        Assert::AreEqual(7.42027e-12, x_est[5].get(5), 1e-5, "t=1, x_est[5], covariance 5");
        Assert::AreEqual(2.54192e-10, x_est[5].get(6), 1e-5, "t=1, x_est[5], covariance 6");
        Assert::AreEqual(-4.48494e-15, x_est[5].get(7), 1e-5, "t=1, x_est[5], covariance 7");
        Assert::AreEqual(-6.80163e-11, x_est[5].get(8), 1e-5, "t=1, x_est[5], covariance 8");

        Assert::AreEqual(5126.58, x_est[6].get(0), 1e-5, "t=1, x_est[6], mean");
        Assert::AreEqual(-4.83955e-26, x_est[6].get(1), 1e-5, "t=1, x_est[6], covariance 1");
        Assert::AreEqual(8.63688e-17, x_est[6].get(2), 1e-5, "t=1, x_est[6], covariance 2");
        Assert::AreEqual(1.04722e-19, x_est[6].get(3), 1e-5, "t=1, x_est[6], covariance 3");
        Assert::AreEqual(-2.29958e-19, x_est[6].get(4), 1e-5, "t=1, x_est[6], covariance 4");
        Assert::AreEqual(5.82811e-28, x_est[6].get(5), 1e-5, "t=1, x_est[6], covariance 5");
        Assert::AreEqual(-4.48494e-15, x_est[6].get(6), 1e-5, "t=1, x_est[6], covariance 6");
        Assert::AreEqual(1.98929e-10, x_est[6].get(7), 1e-5, "t=1, x_est[6], covariance 7");
        Assert::AreEqual(7.42449e-12, x_est[6].get(8), 1e-5, "t=1, x_est[6], covariance 8");

        Assert::AreEqual(571.571, x_est[7].get(0), 1e-2, "t=1, x_est[7], mean");
        Assert::AreEqual(-1.65602e-15, x_est[7].get(1), 1e-5, "t=1, x_est[7], covariance 1");
        Assert::AreEqual(4.10841e-11, x_est[7].get(2), 1e-5, "t=1, x_est[7], covariance 2");
        Assert::AreEqual(4.75476e-11, x_est[7].get(3), 1e-5, "t=1, x_est[7], covariance 3");
        Assert::AreEqual(4.65707e-11, x_est[7].get(4), 1e-5, "t=1, x_est[7], covariance 4");
        Assert::AreEqual(-2.69183e-16, x_est[7].get(5), 1e-5, "t=1, x_est[7], covariance 5");
        Assert::AreEqual(-6.80163e-11, x_est[7].get(6), 1e-5, "t=1, x_est[7], covariance 6");
        Assert::AreEqual(7.42449e-12, x_est[7].get(7), 1e-5, "t=1, x_est[7], covariance 7");
        Assert::AreEqual(2.54302e-10, x_est[7].get(8), 1e-5, "t=1, x_est[7], covariance 8");

        prediction = predictor.predict(t, x_est);
        event = prediction.getEvents().front().getTOE().getVec();
        std::sort(event.begin(), event.end());
        Assert::IsTrue(event.front() < 2500, "Lowest event time");
        Assert::IsTrue(event.back() > 2750, "Highest event time floor");
        Assert::IsTrue(event.back() < 3000, "Highest event time ceiling");
        mean = std::accumulate(event.begin(), event.end(), 0.0) / event.size();
        Assert::AreEqual(mean, 2700, 100, "Mean ToE");
    }

    void registerTests(TestContext& context) {
        context.AddTest("Const Load Prediction", testConstLoad, "Battery Predictor Results");
    }
}
