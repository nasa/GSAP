cmake_minimum_required(VERSION 2.8)

project(gsap-benchmark)

include(${CMAKE_CURRENT_SOURCE_DIR}/../compiler_flags.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(SRCS
    src/main.cpp
    src/main-async.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)


# GSAP lib
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/build/gsap
)

# Copy config and data files to bin directory
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data)
configure_file(data/data_const_load.csv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data COPYONLY)
configure_file(data/data_variable_load.csv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data COPYONLY)
configure_file(data/data_flight.csv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data COPYONLY)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cfg)
configure_file(cfg/example_async.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cfg COPYONLY)
configure_file(cfg/example_simple.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/cfg COPYONLY)


link_libraries(gsap)
add_executable(benchmark ${SRCS})
