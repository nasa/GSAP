// TODO (JW): By its nature, this file is designed to be copied. It should
// be explicitely (un)licensed as public domain or CC0.
#include <chrono>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

#include "PrognoserFactory.h"

using namespace PCOE;

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

// This example sets up a simple synchronos prognoser to predict battery end of
// discharge using a Monte Carlo predictor and an Unscented Kalman Filter.
int main() {
    // Read battery data from a file.
    auto data = read_file("data_const_load.csv");

    // Create a configuration from a file
    ConfigMap config("example.cfg");

    // Contruct a new prognoser using the prognoser factory. The prognoser
    // will automatically construct an appropriate model, observer and predictor
    // based on the values specified in the config.
    auto prognoser = PrognoserFactory::instance().Create("ModelBasedPrognoser", config);

    // For each line of data in the example file, run a single prediction step.
    for (const auto& line : data) {
        // Get a new prediction
        auto prediction = prognoser->step(line);

        // Get the event for battery EoD. The first line of data is used to initialize the observer,
        // so the first prediction won't have any events.
        if (prediction.getEvents().size() == 0) {
            continue;
        }
        auto eod_event = prediction.getEvents().front();

        // The time of event is a `UData` structure, which represents a data
        // point while maintaining uncertainty. For the MonteCarlo predictor
        // used by this example, the uncertainty is captured by storing the
        // result of each particle used in the prediction.
        UData eod_time = eod_event.getTOE();
        if (eod_time.uncertainty() != UType::Samples) {
            std::cerr << "Unexpected uncertainty type for EoD prediction" << std::endl;
            return 1;
        }

        // For this example, we will print the median EoD.
        auto samples = eod_time.getVec();
        std::sort(samples.begin(), samples.end());
        double eod_median = samples.at(samples.size() / 2);
        std::cout << eod_median << std::endl;
    }

    return 0;
}
