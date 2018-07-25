cmake_minimum_required(VERSION 2.8)

project(GSAP)

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

set(GSAP_INSTALL_BIN_DIR bin)
set(GSAP_INSTALL_LIB_DIR lib)
set(GSAP_INSTALL_INCLUDE_DIR include)
set(GSAP_INSTALL_EXAMPLE_DIR example)

# This hootenanny is to give the user a better default value for the
# install path in the GUI window. It seems we can't do this directly
# with SET for built-in variables like CMAKE_INSTALL_PREFIX.
set(INSTALL_DIRECTORY "${PROJECT_BINARY_DIR}/install" CACHE PATH
    "Directory to install to.")
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIRECTORY} CACHE INTERNAL
    "Install path prefix, prepended onto install directories." FORCE)

# Build option for enabling compiler and linker flags for gathering coverage data
# In command line, set option this way:
# cmake -DCoverage=ON <build folder>
option(Coverage "Coverage" OFF)

include(compiler_flags.cmake)

# Check command line to see if OpenMP is to be used.
option(UseOpenMP "UseOpenMP" FALSE)

# Build with OpenMP if desired, and package can be found.
if (UseOpenMP)
    message(STATUS "Attempting to find OpenMP package...")
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D USING_OPENMP")
        message(STATUS "Added OpenMP to buildsystem")
    else()
        message(FATAL_ERROR "Command line asked for OpenMP, but package couldn't be found!")
    endif()
endif()

set (HEADERS
    inc/ConfigMap.h
    inc/ConstLoadEstimator.h
    inc/Contracts.h
    inc/DataPoint.h
    inc/DataPoints.h
    inc/DataStore.h
    inc/Datum.h
    inc/Exceptions.h
    inc/Factory.h
    inc/GaussianVariable.h
    inc/GSAPConfigMap.h
    inc/LoadEstimator.h
    inc/LoadEstimatorFactory.h
    inc/Matrix.h
    inc/Messages/DoubleMessage.h
    inc/Messages/IMessageProcessor.h
    inc/Messages/IMessagePublisher.h
    inc/Messages/Message.h
    inc/Messages/MessageBus.h
    inc/Messages/MessageClock.h
    inc/Messages/MessageId.h
    inc/Messages/MessageWatcher.h
    inc/Messages/ProgEventMessage.h
    inc/Messages/UDataMessage.h
    inc/Messages/UDataVecMessage.h
    inc/Model.h
    inc/MovingAverageLoadEstimator.h
    inc/Observers/Observer.h
    inc/Observers/ObserverFactory.h
    inc/Observers/ParticleFilter.h
    inc/Observers/UnscentedKalmanFilter.h
    inc/Predictors/MonteCarloPredictor.h
    inc/Predictors/Predictor.h
    inc/Predictors/PredictorFactory.h
    inc/PContainer.h
    inc/ProgEvent.h
    inc/PrognosticsModel.h
    inc/PrognosticsModelFactory.h
    inc/Singleton.h
    inc/StatisticalTools.h
    inc/TCPSocket.h
    inc/Thread.h
    inc/ThreadSafeLog.h
    inc/UData.h
    inc/UDataInterfaces.h
    inc/UDPSocket.h
    inc/BenchmarkTimer.h
    inc/TCPSocket.h
    inc/UDPSocket.h
    inc/TCPServer.h
    inc/BatteryModel.h
    inc/Communicator.h
    inc/Prognoser.h
    inc/CommunicatorFactory.h
    inc/ModelBasedPrognoser.h
    inc/ModelFactory.h
    inc/ProgManager.h
    inc/PrognoserFactory.h
    inc/RandomCommunicator.h
    inc/PlaybackCommunicator.h
    inc/RecorderCommunicator.h
    inc/STDINCommunicator.h
    inc/StringUtils.h)

set(SRCS
    src/ConfigMap.cpp
    src/ConstLoadEstimator.cpp
    src/DataPoint.cpp
    src/DataPoints.cpp
    src/GaussianVariable.cpp
    src/GSAPConfigMap.cpp
    src/LoadEstimator.cpp
    src/Matrix.cpp
    src/Messages/Message.cpp
    src/Messages/MessageBus.cpp
    src/Messages/MessageId.cpp
    src/MovingAverageLoadEstimator.cpp
    src/Observers/ParticleFilter.cpp
    src/Observers/UnscentedKalmanFilter.cpp
    src/PContainer.cpp
    src/Predictors/MonteCarloPredictor.cpp
    src/ProgEvent.cpp
    src/StatisticalTools.cpp
    src/TCPSocket.cpp
    src/Thread.cpp
    src/ThreadSafeLog.cpp
    src/UData.cpp
    src/UDataInterfaces.cpp
    src/UDPSocket.cpp
    src/TCPServer.cpp
    src/BatteryModel.cpp
    src/Communicator.cpp
    src/Prognoser.cpp
    src/ModelBasedPrognoser.cpp
    src/ProgManager.cpp
    src/RandomCommunicator.cpp
    src/PlaybackCommunicator.cpp
    src/RecorderCommunicator.cpp
    src/STDINCommunicator.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)
add_library(gsap ${HEADERS} ${SRCS})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Test/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/templates/)

install(TARGETS gsap
  LIBRARY DESTINATION ${GSAP_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${GSAP_INSTALL_LIB_DIR})
install(FILES ${HEADERS} DESTINATION ${GSAP_INSTALL_INCLUDE_DIR})

install(FILES compiler_flags.cmake DESTINATION ${GSAP_INSTALL_EXAMPLE_DIR} )
