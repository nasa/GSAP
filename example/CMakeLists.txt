set(SRCS
	progMain.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/support/inc/)
include_directories(${CMAKE_SOURCE_DIR}/framework/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(framework support)
add_executable(example ${SRCS})
