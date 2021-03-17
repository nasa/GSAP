// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.

#include <chrono>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

#include "PrognoserFactory.h"
#include "Test.h"

namespace SyncIntegrationTests {
    using namespace PCOE;
    
    const size_t ITERATIONS = 10;
    
    // This function is a quick and dirty reader for the example data files.
    // For production-ready applications, a complete and well-tested CSV library
    // should be used.
    std::vector<std::map<MessageId, Datum<double>>> read_file(const std::string& filename) {
        using namespace std::chrono;
        
        std::ifstream file(filename);
        if (file.fail()) {
            std::cerr << "Unable to open data file" << std::endl;
        }
        // Skip header line
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        auto now = system_clock::now();
        
        std::vector<std::map<MessageId, Datum<double>>> result;
        while (file.good()) {
            std::map<MessageId, Datum<double>> data;
            std::string line;
            std::getline(file, line);
            if (line.empty()) {
                continue;
            }
            std::stringstream line_stream(line);
            std::string cell;
            std::getline(line_stream, cell, ',');
            double file_time = std::stod(cell);
            auto timestamp = now + milliseconds(static_cast<unsigned>(file_time * 1000));
            
            std::getline(line_stream, cell, ',');
            Datum<double> power(std::stod(cell));
            power.setTime(timestamp);
            
            std::getline(line_stream, cell, ',');
            Datum<double> temperature(std::stod(cell));
            temperature.setTime(timestamp);
            
            std::getline(line_stream, cell, ',');
            Datum<double> voltage(std::stod(cell));
            voltage.setTime(timestamp);
            
            data.insert({MessageId::Watts, power});
            data.insert({MessageId::Centigrade, temperature});
            data.insert({MessageId::Volts, voltage});
            result.push_back(data);
        }
        return result;
    }
    
    ConfigMap buildBaseMap() {
        ConfigMap config;
        config.set("model", "Battery");
        config.set("predictor", "MC");
        config.set("Predictor.loadEstimator", "Const");
        config.set("Predictor.SampleCount", 50);
        config.set("Predictor.Horizon", 10000);
        config.set("Model.ProcessNoise", {"1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5"});
        config.set("LoadEstimator.Loading", 8);
        return config;
    }
    
    ConfigMap addUKFConfig(ConfigMap config) {
        config.set("observer", "UKF");
        config.set("Observer.Q", {"1e-10", "0", "0", "0", "0", "0", "0", "0", "0", "1e-10", "0", "0", "0", "0", "0", "0", "0", "0", "1e-10", "0", "0", "0", "0", "0", "0", "0", "0", "1e-10", "0", "0", "0", "0", "0", "0", "0", "0", "1e-10", "0", "0", "0", "0", "0", "0", "0", "0", "1e-10", "0", "0", "0", "0", "0", "0", "0", "0", "1e-10", "0", "0", "0", "0", "0", "0", "0", "0", "1e-10"});
        config.set("Observer.R", {"1e-2", "0", "0", "1e-2"});
        return config;
    }
    
    ConfigMap addPFConfig(ConfigMap config) {
        config.set("observer", "PF");
        config.set("Observer.ParticleCount", "200");
        config.set("Observer.ProcessNoise", {"1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5", "1e-5"});
        config.set("Observer.SensorNoise", {"1e-5", "1e-5"});
        config.set("Observer.MinEffective", "100");
        return config;
    }
    
    void run_test(ConfigMap&& config) {
        using namespace std::chrono;
        // Read battery data from a file.
        auto data = read_file("data_const_load.csv");
        
        // Contruct a new prognoser using the prognoser factory. The prognoser
        // will automatically construct an appropriate model, observer and predictor
        // based on the values specified in the config.
        auto prognoser = PrognoserFactory::instance().Create("ModelBasedPrognoser", config);
        size_t counter = 0;
        
        // For each line of data in the example file, run a single prediction step.
        for (const auto& line : data) {
            if (counter++ > ITERATIONS) break;
            
            // Get a new prediction
            auto prediction = prognoser->step(line);
            
            if (prediction.getEvents().size() == 0) {
                // Get the event for battery EoD. The first line of data is used to initialize the observer,
                // so the first prediction won't have any events.
                if (counter <= 1) continue;
                else {
                    Test::Assert::Fail("No events generated");
                }
            }
            auto eod_event = prediction.getEvents().front();
            
            // The time of event is a `UData` structure, which represents a data
            // point while maintaining uncertainty. For the MonteCarlo predictor
            // used by this example, the uncertainty is captured by storing the
            // result of each particle used in the prediction.
            UData eod_time = eod_event.getTOE();
            if (eod_time.uncertainty() != UType::Samples) {
                Test::Assert::Fail("Unexpected uncertainty type for EoD prediction");
            }
            
            // For this example, we will check the median EoD.
            auto samples = eod_time.getVec();
            std::sort(samples.begin(), samples.end());
            double eod_median = samples.at(samples.size() / 2);
            auto now =  MessageClock::now();
            auto now_s = duration_cast<std::chrono::seconds>(now.time_since_epoch());
            auto relative_time = (eod_median-now_s.count());
            
            Test::Assert::IsTrue(relative_time < 4500 && relative_time > 2500, "Relative time should be between 2500-4500");
        }
    }
    
    void sync_UKF() {
        run_test(addUKFConfig(buildBaseMap()));
    }
    
    void sync_PF() {
        run_test(addPFConfig(buildBaseMap()));
    }
    
    void registerTests(PCOE::Test::TestContext& context) {
        context.AddTest("sync UKF", sync_UKF, "Integration");
        context.AddTest("sync PF", sync_PF, "Integration");
    }
}
