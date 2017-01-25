set (HEADERS
	inc/CommManager.h
	inc/CommonCommunicator.h
	inc/CommonPrognoser.h
	inc/CommunicatorFactory.h
	inc/ModelBasedPrognoser.h
	inc/ProgManager.h
	inc/PrognoserFactory.h
	inc/SharedLib.h
	inc/RandomCommunicator.h
	inc/PlaybackCommunicator.h
	inc/RecorderCommunicator.h
	inc/Battery.h
	inc/BenchmarkTest.h
)

set(SRCS
	src/CommManager.cpp
	src/CommonCommunicator.cpp
	src/CommonPrognoser.cpp
	src/ModelBasedPrognoser.cpp
	src/ProgManager.cpp
	src/RandomCommunicator.cpp
	src/PlaybackCommunicator.cpp
	src/RecorderCommunicator.cpp
	src/Battery.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/support/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)

add_library(framework ${HEADERS} ${SRCS})
