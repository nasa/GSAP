set(HEADERS
    ConfigMapTests.h
    DataStoreTests.h
    DPointsTests.h
    DPointTests.h
    DynamicArrayTests.h
    Loading/LoadEstimatorTests.h
    Messages/MessageBusTests.h
    Messages/MessageWatcherTests.h
    MockClasses.h
    ModelTests.h
    Observers/EventDrivenObserverTests.h
    ObserverTests.h
    Predictors/EventDrivenPredictorTests.h
    PredictorTests.h
    ThreadTests.h
    UDataTests.h
    StatisticalToolsTests.h
    GaussianVariableTests.h
    ParticleFilterTests.h
    TCPSocketTests.h
    UDPSocketTests.h
    ProgManagerTests.h
    TrajectoryServiceTests.h
)


set(SRCS
    ConfigMapTests.cpp
    DataStoreTests.cpp
    DPointsTests.cpp
    DPointTests.cpp
    DynamicArrayTests.cpp
    Loading/ConstLoadEstimatorTests.cpp
    Loading/GaussianLoadEstimatorTests.cpp
    Loading/LoadEstimatorTests.cpp
    Loading/MovingAverageLoadEstimatorTests.cpp
    main.cpp
    MatrixTests.cpp
    Messages/MessageBusTests.cpp
    Messages/MessageWatcherTests.cpp
    ModelTests.cpp
    Observers/EventDrivenObserverTests.cpp
    ObserverTests.cpp
    Predictors/BatteryResultTests.cpp
    Predictors/EventDrivenPredictorTests.cpp
    PredictorTests.cpp
    ThreadTests.cpp
    UDataTests.cpp
    StatisticalToolsTests.cpp
    GaussianVariableTests.cpp
    ParticleFilterTests.cpp
    TCPSocketTests.cpp
    UDPSocketTests.cpp
    CommunicatorTests.cpp
    FrameworkTests.cpp
    main.cpp
    ProgManagerTests.cpp
    TrajectoryServiceTests.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/inc/)
include_directories(${CMAKE_SOURCE_DIR}/test/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(testLib gsap)
add_executable(gsapTests ${HEADERS} ${SRCS})
