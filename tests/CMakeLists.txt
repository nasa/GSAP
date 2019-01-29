cmake_minimum_required(VERSION 2.8)

project(gsap-tests)

include(${CMAKE_CURRENT_SOURCE_DIR}/../compiler_flags.cmake)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

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

# Templates - we compile these as part of the test to make sure they don't break
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../templates/
    ${CMAKE_CURRENT_SOURCE_DIR}/build/templates
)
