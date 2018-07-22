set(SRCS
    progMain.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(gsap)
add_executable(example ${SRCS})
