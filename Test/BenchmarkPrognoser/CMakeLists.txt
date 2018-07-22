set(HEADERS
    BenchmarkPrognoser.h
)

set(SRCS
    BenchmarkPrognoser.cpp
    main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/support/inc/)
include_directories(${CMAKE_SOURCE_DIR}/framework/inc/)
include_directories(${CMAKE_SOURCE_DIR}/test/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(testLib gsap)
add_executable(BenchmarkPrognoser ${HEADERS} ${SRCS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/BatteryPlayback.txt DESTINATION ${CMAKE_SOURCE_DIR}/bin/)
