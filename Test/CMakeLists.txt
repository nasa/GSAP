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

# Test Library
add_library(testLib ${HEADERS} ${SRCS})

#Test Executables
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/gsapTests/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/commCollectionTests/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/batteryPrognoserTests/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkPrognoser/)
