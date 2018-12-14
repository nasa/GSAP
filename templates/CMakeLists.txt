set(HEADERS
    EmptyCommunicator.h
    EmptyModel.h
    EmptyObserver.h
    EmptyPredictor.h
    EmptyPrognoser.h
    EmptyPrognosticsModel.h
)

set(SRCS
    EmptyCommunicator.cpp
    EmptyModel.cpp
    EmptyObserver.cpp
    EmptyPredictor.cpp
    EmptyPrognoser.cpp
    EmptyPrognosticsModel.cpp
)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
link_libraries(gsap)

# Template Library
add_library(templateLib ${HEADERS} ${SRCS})
