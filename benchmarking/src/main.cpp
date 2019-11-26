// TODO (JW): By its nature, this file is designed to be copied. It should
// be explicitely (un)licensed as public domain or CC0.
#include <chrono>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

#include "PrognoserFactory.h"

using namespace PCOE;
long start_s;

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
    start_s = duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

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
        auto last_timestamp = now + milliseconds(static_cast<unsigned>(file_time * 1000));

        std::getline(line_stream, cell, ',');
        Datum<double> power(std::stod(cell));
        power.setTime(last_timestamp);

        std::getline(line_stream, cell, ',');
        Datum<double> temperature(std::stod(cell));
        temperature.setTime(last_timestamp);

        std::getline(line_stream, cell, ',');
        Datum<double> voltage(std::stod(cell));
        voltage.setTime(last_timestamp);

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
    auto data = read_file("data/data_const_load.csv");

    // Create a configuration from a file
    ConfigMap config("cfg/example_simple.cfg");

    // Prepare output file
    std::ofstream file("simple_performance.csv");
    if (file.fail()) {
        std::cerr << "Unable to open data file" << std::endl;
    }
    file << "Mission Time (s), " <<  "RUL -2σ, " << "RUL -1σ, " <<  "RUL 𝝁, "  << "RUL +1σ, " << "RUL +2σ," << "True RUL" << std::endl;
    
    // Contruct a new prognoser using the prognoser factory. The prognoser
    // will automatically construct an appropriate model, observer and predictor
    // based on the values specified in the config.
    auto prognoser = PrognoserFactory::instance().Create("ModelBasedPrognoser", config);

    // For each line of data in the example file, run a single prediction step.
    for (const auto& line : data) {
        static int counter = 0;
        counter++;
        
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

        // For this example, we will print the median EoD.
        auto samples = eod_time.getVec();
        std::sort(samples.begin(), samples.end());
        double eod_median = samples.at(samples.size() / 2) - counter-start_s;
        double eod_1sd = samples.at(samples.size() * 0.841) - counter-start_s;
        double eod_m1sd = samples.at(samples.size() * 0.159) - counter-start_s;
        double eod_2sd = samples.at(samples.size() * 0.977) - counter-start_s;
        double eod_m2sd = samples.at(samples.size() * 0.023) - counter-start_s;
        
        file << counter << ',' << eod_m2sd << ',' << eod_m1sd << ','  << eod_median << ',' << eod_1sd << ',' << eod_2sd << ',' << 2820-counter << std::endl;
    }

    return 0;
}
