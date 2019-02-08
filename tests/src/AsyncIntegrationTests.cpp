// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <vector>

#include "Messages/MessageBus.h"
#include "Messages/ProgEventMessage.h"
#include "Messages/ScalarMessage.h"
#include "ModelBasedEventDrivenPrognoserBuilder.h"
#include "Test.h"

using namespace PCOE;

namespace AsyncIntegrationTests {
    const size_t ITERATIONS = 12;
    
    std::vector<std::vector<std::shared_ptr<DoubleMessage>>> read_file(const std::string& filename,
                                                                       const std::string& src) {
        using namespace std::chrono;
        std::ifstream file(filename);
        if (file.fail()) {
            std::cerr << "Unable to open data file" << std::endl;
        }
        // Skip header line
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        auto now = MessageClock::now();
        
        std::vector<std::vector<std::shared_ptr<DoubleMessage>>> result;
        while (file.good()) {
            std::vector<std::shared_ptr<DoubleMessage>> data;
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
            double power = std::stod(cell);
            
            std::getline(line_stream, cell, ',');
            double temperature = std::stod(cell);
            
            std::getline(line_stream, cell, ',');
            double voltage = std::stod(cell);
            
            data.push_back(std::make_shared<DoubleMessage>(MessageId::Watts, src, timestamp, power));
            data.push_back(
                           std::make_shared<DoubleMessage>(MessageId::Centigrade, src, timestamp, temperature));
            data.push_back(std::make_shared<DoubleMessage>(MessageId::Volts, src, timestamp, voltage));
            result.push_back(data);
        }
        return result;
    }
    
    class PredictionPrinter : public IMessageProcessor {
    public:
        PredictionPrinter(MessageBus& bus, const std::string& src) : bus(bus) {
            bus.subscribe(this, src, MessageId::BatteryEod);
        }
        
        ~PredictionPrinter() {
            bus.unsubscribe(this);
        }
        
        void processMessage(const std::shared_ptr<Message>& message) override {
            using namespace std::chrono;
            // The prediction printer only ever subscribes to the BatteryEoD message
            // id, which should always be a ProgEventMessage, so this should always
            // succeed.
            auto prediction_msg = dynamic_cast<ProgEventMessage*>(message.get());
            if (prediction_msg == nullptr) {
                std::cerr << "Failed to cast prediction message to expected type" << std::endl;
                std::exit(1);
            }
            
            // Get the event for battery EoD
            auto eod_event = prediction_msg->getValue();
            
            // The time of event is a `UData` structure, which represents a data
            // point while maintaining uncertainty. For the MonteCarlo predictor
            // used by this example, the uncertainty is captured by storing the
            // result of each particle used in the prediction.
            UData eod_time = eod_event.getTOE();
            if (eod_time.uncertainty() != UType::Samples) {
                std::cerr << "Unexpected uncertainty type for EoD prediction" << std::endl;
                return std::exit(1);
            }
            
            auto samples = eod_time.getVec();
            std::sort(samples.begin(), samples.end());
            double eod_median = samples.at(samples.size() / 2);
            auto now =  MessageClock::now();
            auto now_s = duration_cast<std::chrono::seconds>(now.time_since_epoch());
            auto relative_time = (eod_median-now_s.count());
            
            Test::Assert::IsTrue(relative_time < 4500 && relative_time > 2500, "Relative time should be between 2500-4500");
        }
        
    private:
        MessageBus& bus;
    };
    
    void run_test(ConfigMap config) {
        std::string src = "sensor";

        // Read battery data from a file.
        auto data = read_file("../../data/data_const_load.csv", src);

        MessageBus bus;

        PredictionPrinter printer(bus, src);

        ModelBasedEventDrivenPrognoserBuilder builder(config);
        builder.setModelName("Battery", true);
        builder.setObserverName(config.getString("observer"));
        builder.setPredictorName(config.getString("predictor"));
        builder.setLoadEstimatorName("Const");
        
        auto prognoser = builder.build(bus, src, "trajectory");
        
        size_t i = 0;
        for (const auto& line : data) {
            std::this_thread::sleep_until(line.front()->getTimestamp());
            
            for (const auto& msg : line) {
                bus.publish(msg);
            }
            if (i++ > ITERATIONS) {
                break;
            }
        }
        
        bus.waitAll();
    }
    
    ConfigMap buildBaseMap() {
        ConfigMap config;
        config.set("model", "Battery");
        config.set("predictor", "MC");
        config.set("Predictor.loadEstimator", "Const");
        config.set("Predictor.SampleCount", 100);
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
        config.set("Observer.ProcessNoise", {"1", "1", "1", "1", "1", "1", "1", "1"});
        config.set("Observer.SensorNoise", {"1", "1"});
        config.set("Observer.MinEffective", "100");
        return config;
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
