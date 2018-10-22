cmake_minimum_required(VERSION 2.8)

project(gsap-example)

set(GSAP_INSTALL_BIN_DIR bin)
set(GSAP_INSTALL_LIB_DIR lib)
set(GSAP_INSTALL_INCLUDE_DIR include)
set(GSAP_INSTALL_EXAMPLE_DIR example)

# This hootenanny is to give the user a better default value for the
# install path in the GUI window. It seems we can't do this directly
# with SET for built-in variables like CMAKE_INSTALL_PREFIX.
set(INSTALL_DIRECTORY "${PROJECT_BINARY_DIR}/install" CACHE PATH
    "Directory to install to.")
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIRECTORY} CACHE INTERNAL
    "Install path prefix, prepended onto install directories." FORCE)

include(${CMAKE_CURRENT_SOURCE_DIR}/../compiler_flags.cmake)

set(SRCS
    progMain.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# GSAP lib
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/build/gsap
)

link_libraries(gsap)
add_executable(example ${SRCS})

install(FILES ${SRCS} DESTINATION ${GSAP_INSTALL_EXAMPLE_DIR} )
install(DIRECTORY cfg data DESTINATION ${GSAP_INSTALL_EXAMPLE_DIR} )
install(FILES install-CMakeLists.txt DESTINATION ${GSAP_INSTALL_EXAMPLE_DIR}
  RENAME CMakeLists.txt)
