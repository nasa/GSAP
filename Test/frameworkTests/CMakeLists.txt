set(HEADERS
	CommunicatorTests.h
	FrameworkTests.h
	ProgManagerTests.h
)

set(SRCS
	CommunicatorTests.cpp
	FrameworkTests.cpp
	main.cpp
	ProgManagerTests.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/support/inc/)
include_directories(${CMAKE_SOURCE_DIR}/framework/inc/)
include_directories(${CMAKE_SOURCE_DIR}/test/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(testLib framework support)
add_executable(frameworkTests ${HEADERS} ${SRCS})
