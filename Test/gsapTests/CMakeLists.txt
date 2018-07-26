set(HEADERS
    ConfigMapTests.h
    LoadTests.hpp
    DataStoreTests.h
    DPointsTests.h
    DPointTests.h
    MatrixTests.h
    Messages/MessageBusTests.h
    Messages/MessageWatcherTests.h
    ModelTests.h
    ObserverTests.h
    PEventTests.h
    PredictorTests.h
    ThreadTests.h
    UDataTests.h
    StatisticalToolsTests.h
    GaussianVariableTests.h
    ParticleFilterTests.h
    TCPSocketTests.h
    UDPSocketTests.h
    ProgManagerTests.h
)


set(SRCS
    ConfigMapTests.cpp
    LoadTests.cpp
    DataStoreTests.cpp
    DPointsTests.cpp
    DPointTests.cpp
    main.cpp
    MatrixTests.cpp
    Messages/MessageBusTests.cpp
    Messages/MessageWatcherTests.cpp
    ModelTests.cpp
    ObserverTests.cpp
    PEventTests.cpp
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
)

include_directories(${CMAKE_SOURCE_DIR}/inc/)
include_directories(${CMAKE_SOURCE_DIR}/test/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(testLib gsap)
add_executable(gsapTests ${HEADERS} ${SRCS})
