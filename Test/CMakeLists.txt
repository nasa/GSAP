cmake_minimum_required(VERSION 2.8)

project(gsap-test)

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
    inc/Tank3.h
    inc/Test.h
    inc/TestPrognoser.h
)

set(SRCS
    src/Tank3.cpp
    src/TestPrognoser.cpp
)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)

# GSAP lib
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/build/gsap
)

# Test Library
add_library(testLib ${HEADERS} ${SRCS})

# Test Executables
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gsapTests/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/commCollectionTests/)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkPrognoser/)
# Benchmark Prognoser currently broken 

# Templates - we compile these as part of the test to make sure they don't break
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../templates/
    ${CMAKE_CURRENT_SOURCE_DIR}/build/templates
)
