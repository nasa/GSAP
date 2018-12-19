/**  Event-Driven Prognoser Tests - Header
 *   @class     EventDrivenPrognoserTests EventDrivenPrognoserTests.h
 *
 *   @brief     Event-Driven Prognoser Test Class
 *
 *   Tests for event-driven prognoser class.
 *
 *   @author    Chris Teubert
 *   @version   1.2.0
 *
 *      Created: December 17, 2018
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <queue>
#include <thread>

#include "Datum.h"
#include "EventDrivenPrognoser.h"
#include "EventDrivenPrognoserTests.h"
#include "Messages/ProgEventMessage.h"
#include "Messages/ScalarMessage.h"
#include "MockClasses.h"
#include "ModelBasedEventDrivenPrognoserBuilder.h"
#include "Models/PrognosticsModelFactory.h"
#include "Observers/ObserverFactory.h"
#include "Predictors/PredictorFactory.h"
#include "Test.h"

using namespace PCOE;
using namespace PCOE::Test;

const std::string SENSOR_SRC = "test_sensor_src";
const std::string TRAJ_SRC = "test_traj_src";

class TestComm : public IMessageProcessor {
public:
    TestComm(MessageBus& bus) : bus(bus) {
        bus.subscribe(this, SENSOR_SRC);
    }

    void init(std::map<MessageId, Datum<double>> data) {
        Require(data.size() > 0, "Must include atleast one data point to publish");
        using std::chrono::milliseconds;
        time_point time = time_point::min();

        for (auto&& datapt : data) {
            double dataValue = datapt.second.get();
            time_point timept = time_point(milliseconds(datapt.second.getTime()));
            time = timept > time ? timept : time;
            ScalarMessage<double>* message =
                new ScalarMessage<double>(datapt.first, SENSOR_SRC, timept, dataValue);
            bus.publish(std::shared_ptr<Message>(message));
        }
    }

    ProgEvent publish(std::map<MessageId, Datum<double>> data) {
        Require(data.size() > 0, "Must include atleast one data point to publish");
        using std::chrono::milliseconds;
        time_point time = time_point::min();

        // Setup Messages
        std::vector<ScalarMessage<double>*> messages;
        for (auto&& datapt : data) {
            double dataValue = datapt.second.get();
            time_point timept = time_point(milliseconds(datapt.second.getTime()));
            time = timept > time ? timept : time;
            messages.push_back(
                new ScalarMessage<double>(datapt.first, SENSOR_SRC, timept, dataValue));
        }

        // Setup request
        Request request(time);
        unique_lock requests_lock(processMessageMut);
        requests.push(&request);
        // Publish Message
        for (auto message : messages) {
            bus.publish(std::shared_ptr<Message>(message));
        }
        requests_lock.unlock();

        unique_lock lock(request.mut);
        request.waiting = true;
        while (request.waiting) {
            request.condition.wait(lock);
        }

        // Process Result
        ProgEvent result = *currentResult;
        return result;
    }

    /**
     * Handles messages representing updates to the model inputs and
     * outputs. When sufficient new data is collected, automaticlly triggers
     * an observer step and publishes the result.
     *
     * @param message. The message to process.
     **/
    void processMessage(const std::shared_ptr<Message>& rawMessage) override {
        switch (rawMessage->getMessageId()) {
        case PCOE::MessageId::ModelStateVector:
            printf("Test\n");
            break;
            {
            case PCOE::MessageId::TestEvent0:
                lock_guard guard(processMessageMut); // Process one ProgEvent at once
                ProgEventMessage* message = dynamic_cast<ProgEventMessage*>(rawMessage.get());
                ProgEvent result = message->getValue();
                currentResult = &result;

                while (requests.size() > 0 && requests.front()->time <= message->getTimestamp()) {
                    Request* currentRequest = requests.front();
                    requests.pop();

                    unique_lock lock(currentRequest->mut);
                    currentRequest->waiting = false;
                    currentRequest->condition.notify_one();
                }
                break;
            }
        default:
            break;
        }
    }

private:
    using mutex = std::mutex;
    using unique_lock = std::unique_lock<mutex>;
    using lock_guard = std::lock_guard<mutex>;
    using time_point = ScalarMessage<double>::time_point;

    struct Request {
        Request(time_point time) : time(time) {}
        time_point time;
        mutex mut;
        bool waiting = false;
        std::condition_variable condition;
    };

    MessageBus& bus;
    ProgEvent* currentResult;
    mutex processMessageMut;

    std::queue<Request*> requests;
};

Datum<UData>::time_point addOneSecond(PCOE::Datum<UData>::ms_rep time) {
    auto a = std::chrono::milliseconds(time + PCOE::Datum<UData>::ms_rep(1000));
    return Datum<UData>::time_point(a);
}

void testEDPWithMockModel() {
    PrognosticsModelFactory::instance().Register<TestPrognosticsModel>("Mock");
    ObserverFactory::instance().Register<TestObserver>("Mock");
    PredictorFactory::instance().Register<TestPredictor>("Mock");
    ModelBasedEventDrivenPrognoserBuilder builder;
    builder.setModelName("Mock", true);
    builder.setObserverName("Mock");
    builder.setPredictorName("Mock");
    builder.setConfigParam("LoadEstimator.Loading", std::vector<std::string>({"1", "2"}));
    MessageBus bus(std::launch::async);
    EventDrivenPrognoser prognoser = builder.build(bus, SENSOR_SRC, TRAJ_SRC);

    // Initialize
    std::map<MessageId, Datum<double>> data;
    data[MessageId::TestInput0] = Datum<double>(1);
    data[MessageId::TestInput1] = Datum<double>(2);
    data[MessageId::TestOutput0] = Datum<double>(3);
    TestComm comm(bus);
    comm.init(data);

    // Step
    auto newTime = addOneSecond(data[MessageId::TestInput0].getTime());
    data[MessageId::TestInput0].setTime(newTime);
    data[MessageId::TestInput1].setTime(newTime);
    data[MessageId::TestOutput0].setTime(newTime);
    ProgEvent result = comm.publish(data);
    Assert::AreEqual(result.getState()[0].get(), 1, 1e-6);
    Assert::AreEqual(result.getStartTime().get(), 1.5, 1e-6);

    // TODO(CT): Test with Config map
    // ModelBasedPrognoser mbp(config);
    ConfigMap config;
    config.set("model", "Mock");
    config.set("observer", "Mock");
    config.set("predictor", "Mock");
    config.set("LoadEstimator.Loading", {"1", "2"});
    MessageBus bus2;
    EventDrivenPrognoser prognoser2 = builder.build(bus2, SENSOR_SRC, TRAJ_SRC);

    // Initialize
    std::map<MessageId, Datum<double>> data2;
    data2[MessageId::TestInput0] = Datum<double>(1);
    data2[MessageId::TestInput1] = Datum<double>(2);
    data2[MessageId::TestOutput0] = Datum<double>(3);
    TestComm comm2(bus2);
    comm2.init(data2);

    // Step
    auto newTime2 = addOneSecond(data2[MessageId::TestInput0].getTime());
    data2[MessageId::TestInput0].setTime(newTime2);
    data2[MessageId::TestInput1].setTime(newTime2);
    data2[MessageId::TestOutput0].setTime(newTime2);
    ProgEvent result2 = comm.publish(data2);
    Assert::AreEqual(result2.getState()[0].get(), 1, 1e-6);
    Assert::AreEqual(result2.getStartTime().get(), 1.5, 1e-6);
}
