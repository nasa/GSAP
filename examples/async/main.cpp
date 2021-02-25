// TODO (JW): By its nature, this file is designed to be copied. It should
// be explicitely (un)licensed as public domain or CC0.
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
#include "ModelBasedAsyncPrognoserBuilder.h"

using namespace PCOE;

// This function is a quick and dirty reader for the example data files.
// For production-ready applications, a complete and well-tested CSV library
// should be used.
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

// The PredictionPrinter class subscribes to the battery EoD event message and
// prints each event as it is received.
class PredictionPrinter : public IMessageProcessor {
public:
    /**
     * Constructs a new prediction printer that subscribes to battery EoD
     * predictions for the specified source and on the specified message
     * bus.
     **/
    PredictionPrinter(MessageBus& bus, const std::string& src) : bus(bus) {
        bus.subscribe(this, src, MessageId::BatteryEod);
    }

    /**
     * Unsubscribes the prediction printer from the message bus.
     **/
    ~PredictionPrinter() {
        bus.unsubscribe(this);
    }

    /**
     * The message bus will call this function each time the predictor publishes
     * a new battery EoD prediction.
     **/
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

        // For this example, we will print the median EoD, which we get by
        // retrieving the raw samples, sorting them, and taking the middle
        // value.
        auto samples = eod_time.getVec();
        std::sort(samples.begin(), samples.end());
        double eod_median = samples.at(samples.size() / 2);

        // Finally, we print the number of milliseconds until EoD
        auto eod_dur = std::chrono::seconds(static_cast<unsigned long>(eod_median));
        auto now =  MessageClock::now();
        auto now_s = duration_cast<std::chrono::seconds>(now.time_since_epoch());
        std::cout << "Predicted median EoD: " << eod_dur.count() << " s (T- "
                  << (eod_dur-now_s).count() << " s)" << std::endl;
    }

private:
    MessageBus& bus;
};

// This example sets up a predictor to predict battery EoD using a Monte Carlo
// predictor and an Unscented Kalman Filter.
int main() {
    // The source string is a unique identifier for each thing that GSAP is
    // monitoring. This could be a batter serial number or any other unique
    // identifier for each component.
    std::string src = "sensor";

    // Read battery data from a file.
    auto data = read_file("data_const_load.csv", src);

    // Read the configuration from a file.
    ConfigMap config("example.cfg");

    // The message bus is the core of the asynchronous architecture. It
    // maintains a list of listeners who are listening for specific messages
    // and alerts those listeners when a message they are interested in is
    // received.
    MessageBus bus;

    // The printer is the first thing that subscribes to the message bus. Its
    // constructor tells the bus that it wants to know about any predictions
    // that are produced for the thing identified by `src`.
    PredictionPrinter printer(bus, src);

    // The builder uses configuration information and other methods to determine
    // the correct set of objects needed to perform prognostics.
    ModelBasedAsyncPrognoserBuilder builder(std::move(config));
    builder.setModelName("Battery");
    builder.setObserverName("UKF");
    builder.setPredictorName("MC");
    builder.setLoadEstimatorName("Const");

    // The build function call constructs all of the necessary objects to
    // perform prognostics using the specified parameters. These objects
    // are constructed and subscribed to the correct message types on the
    // message bus so that the user only needs to publish data and subscribe
    // to results.
    auto prognoser = builder.build(bus, src, "trajectory");

    // For each line of data in the example file, run a single prediction step.
    for (const auto& line : data) {
        // Sleep until the timestamp specified by the file. While the main
        // thread is sleeping, worker threads owned by the message bus are
        // processing messages and the prediction printer may be printing
        // the results.
        std::this_thread::sleep_until(line.front()->getTimestamp());

        // Publish all of the data in the line. This will trigger the components
        // contructed by the builder to run a prediction, ultimately triggering
        // the prediction printer to print the result.
        std::cout << "Publishing sensor data" << std::endl;
        for (const auto& msg : line) {
            bus.publish(msg);
        }
    }

    // Before exiting, wait for the bus to finish processing all messages to
    // make sure we see all predictions.
    bus.waitAll();
    return 0;
}
