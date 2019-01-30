cmake_minimum_required(VERSION 2.8)

project(gsap-example)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../compiler_flags.cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(SRCS
    main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# GSAP lib
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
    ${CMAKE_CURRENT_SOURCE_DIR}/build/gsap
)

# Copy config and data files to bin directory
configure_file(data_const_load.csv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
configure_file(data_flight.csv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
configure_file(data_variable_load.csv ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
configure_file(example.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)

link_libraries(gsap)
add_executable(example ${SRCS})
