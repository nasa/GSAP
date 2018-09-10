set(SRCS
    progMain.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(gsap)
add_executable(example ${SRCS})

install(FILES ${SRCS} DESTINATION ${GSAP_INSTALL_EXAMPLE_DIR} )
install(DIRECTORY cfg data DESTINATION ${GSAP_INSTALL_EXAMPLE_DIR} )
install(FILES install-CMakeLists.txt DESTINATION ${GSAP_INSTALL_EXAMPLE_DIR}
  RENAME CMakeLists.txt)
