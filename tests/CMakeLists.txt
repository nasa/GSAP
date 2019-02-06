cmake_minimum_required(VERSION 2.8)

project(gsap-tests)

include(${CMAKE_CURRENT_SOURCE_DIR}/../compiler_flags.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testresults/)

# Copy config files used to test config map to output directory
set(TEST_CONFIG_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cfg/)
file(MAKE_DIRECTORY ${TEST_CONFIG_DIRECTORY})
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg/Test.cfg
    ${TEST_CONFIG_DIRECTORY}
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg/Test2.cfg
    ${TEST_CONFIG_DIRECTORY}
    COPYONLY
)

set(HEADERS
    inc/MockClasses.h
    inc/Tank3.h
    inc/Test.h
    inc/TestPrognoser.h
)

set(SRCS
    src/ConfigMapTests.cpp
    src/DataStoreTests.cpp
    src/DPointsTests.cpp
    src/DPointTests.cpp
    src/DynamicArrayTests.cpp
    src/EventDrivenPrognoserTests.cpp
    src/GaussianVariableTests.cpp
    src/Loading/ConstLoadEstimatorTests.cpp
    src/Loading/GaussianLoadEstimatorTests.cpp
    src/Loading/LoadEstimatorTests.cpp
    src/Loading/MovingAverageLoadEstimatorTests.cpp
    src/main.cpp
    src/MatrixTests.cpp
    src/Messages/MessageBusTests.cpp
    src/Messages/MessageWatcherTests.cpp
    src/ModelBasedPrognoserTests.cpp
    src/ModelTests.cpp
    src/Observers/EventDrivenObserverTests.cpp
    src/Observers/ObserverTests.cpp
    src/Observers/ParticleFilterTests.cpp
    src/Predictors/BatteryResultTests.cpp
    src/Predictors/EventDrivenPredictorTests.cpp
    src/Predictors/PredictorTests.cpp
    src/StatisticalToolsTests.cpp
    src/SyncIntegrationTests.cpp
    src/Tank3.cpp
    src/TestPrognoser.cpp
    src/ThreadTests.cpp
    src/TrajectoryServiceTests.cpp
    src/UDataTests.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)

# GSAP lib
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/build/gsap
)

# Test Executable
link_libraries(gsap)
add_executable(tests ${HEADERS} ${SRCS})

# Compile all of the example code to make sure it's not totally broken
#Template library
set(TEMPLATE_SRCS
    ../templates/EmptyModel.cpp
    ../templates/EmptyObserver.cpp
    ../templates/EmptyPredictor.cpp
    ../templates/EmptyPrognoser.cpp
    ../templates/EmptyPrognosticsModel.cpp
)
add_library(templates ${TEMPLATE_SRCS})

add_executable(ex_simple ../examples/simple/main.cpp)
add_executable(ex_async ../examples/async/main.cpp)
