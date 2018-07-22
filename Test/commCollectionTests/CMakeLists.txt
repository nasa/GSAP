set(HEADERS
    CommTests.h
)

set(SRCS
    CommTests.cpp
    main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/support/inc/)
include_directories(${CMAKE_SOURCE_DIR}/framework/inc/)
include_directories(${CMAKE_SOURCE_DIR}/test/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(testLib gsap)
add_executable(commCollectionTests ${HEADERS} ${SRCS})
