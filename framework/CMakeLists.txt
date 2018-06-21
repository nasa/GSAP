set (HEADERS
	inc/BatteryModel.h
	inc/CommManager.h
	inc/Communicator.h
	inc/Prognoser.h
	inc/CommunicatorFactory.h
	inc/ModelBasedPrognoser.h
	inc/ModelFactory.h
	inc/ProgManager.h
	inc/PrognoserFactory.h
	inc/StringUtils.h
	inc/RandomCommunicator.h
	inc/PlaybackCommunicator.h
	inc/RecorderCommunicator.h
	inc/STDINCommunicator.h
	inc/StringUtils.h)

set(SRCS
	src/BatteryModel.cpp
	src/CommManager.cpp
	src/Communicator.cpp
	src/Prognoser.cpp
	src/ModelBasedPrognoser.cpp
	src/ProgManager.cpp
	src/RandomCommunicator.cpp
	src/PlaybackCommunicator.cpp
	src/RecorderCommunicator.cpp
	src/STDINCommunicator.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../support/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)

add_library(framework ${HEADERS} ${SRCS})
