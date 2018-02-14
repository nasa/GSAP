set(HEADERS
	ConfigMapTests.h
    ConstLoadTests.h
	DataStoreTests.h
	DPointsTests.h
	DPointTests.h
	MatrixTests.h
	ModelTests.h
	ObserverTests.h
	PEventTests.h
	PredictorTests.h
	ProgDataTests.h
	ThreadTests.h
	UDataTests.h
)

set(SRCS
	ConfigMapTests.cpp
    ConstLoadTests.cpp
	DataStoreTests.cpp
	DPointsTests.cpp
	DPointTests.cpp
	main.cpp
	MatrixTests.cpp
	ModelTests.cpp
	ObserverTests.cpp
	PEventTests.cpp
	PredictorTests.cpp
	ProgDataTests.cpp
	ThreadTests.cpp
	UDataTests.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/support/inc/)
include_directories(${CMAKE_SOURCE_DIR}/framework/inc/)
include_directories(${CMAKE_SOURCE_DIR}/test/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(testLib support framework)
add_executable(supportTests ${HEADERS} ${SRCS})
