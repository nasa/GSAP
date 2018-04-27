cmake_minimum_required(VERSION 2.8)

project(GSAP)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

#	Build option for enabling compiler and linker flags for gathering coverage data
#	In command line, set option this way:
#	cmake -DCoverage=ON <build folder>
option(Coverage "Coverage" OFF)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	# -g:                Produce debugging information
	# -O3:               Optimize as much as possible while remaning standards compliant
	# -O0:               Disable optimizations
	# -std:              Determine the language standard.
	# -Wall:             Enables all the warnings about constructions that some users consider questionable
	# -Wcast-qual:       Warn whenever a pointer is cast so as to remove a type qualifier from the target type
	# -Werror:           Make all warnings into errors
	# -Weffc++:          Warn about violations of some guidelines from Scott Meyers' Effective C++ (Note 2016-07-13: Currently removed because it warns about some things that are perfectly safe)
	# -Wextra:           Enables some extra warning flags that are not enabled by -Wall
	# -Wfloat-equal:     Warn if floating-point values are used in equality comparisons.
	# -Wformat:          Check calls to printf and scanf, etc.
	# -Winline:          Warn if a function that is declared as inline cannot be inlined
	# -Wold-style-cast:  Warn on C-style casts
	# -Wpedantic:        Issue all the warnings demanded by strict ISO C and ISO C++
	# -Wshadow:          Warn whenever a local variable or type declaration shadows another variable, parameter, type, or class member
	# -Wsign-conversion: Warn for implicit conversions that may change the sign of an integer value
	# -Wswitch-default:  Warn whenever a switch statement does not have a default case.
	# -Wno-unknown-pragmas: (2016-07-13) Temporarily added to disable warnings about #pragma unused
	set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wcast-qual -Wextra -Wfloat-equal")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Wold-style-cast -Wpedantic -Wshadow")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion -Wswitch-default -Wno-unknown-pragmas")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Winline")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Werror")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	# -g:                Produce debugging information
	# -O3:               Optimize as much as possible while remaning standards compliant
	# -Og:               Enable optimizations that don't interfere with debugging
	# -std:              Determine the language standard.
	# -Wall:             Enables all the warnings about constructions that some users consider questionable
	# -Wcast-qual:       Warn whenever a pointer is cast so as to remove a type qualifier from the target type
	# -Werror:           Make all warnings into errors
	# -Weffc++:          Warn about violations of some guidelines from Scott Meyers' Effective C++ (Note 2016-07-13: Currently removed because it warns about some things that are perfectly safe)
	# -Wextra:           Enables some extra warning flags that are not enabled by -Wall
	# -Wfloat-equal:     Warn if floating-point values are used in equality comparisons.
	# -Wformat:          Check calls to printf and scanf, etc.
	# -Winline:          Warn if a function that is declared as inline cannot be inlined
	# -Wold-style-cast:  Warn on C-style casts
	# -Wpedantic:        Issue all the warnings demanded by strict ISO C and ISO C++
	# -Wshadow:          Warn whenever a local variable or type declaration shadows another variable, parameter, type, or class member
	# -Wsign-conversion: Warn for implicit conversions that may change the sign of an integer value
	# -Wswitch-default:  Warn whenever a switch statement does not have a default case.
	# -Wno-unknown-pragmas: (2016-07-13) Temporarily added to disable warnings about #pragma unused
	set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wcast-qual -Wextra -Wfloat-equal")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Wold-style-cast -Wpedantic -Wshadow")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion -Wswitch-default -Wno-unknown-pragmas")

if(Coverage)
		#	If coverage option enabled, set compiler and linker flags to gather coverage data
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
		set(CMAKE_EXE_LINKER_FLAGS "-lgcov")
endif()

	set(CMAKE_CXX_FLAGS_DEBUG "-g -Og -Winline")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Werror")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	# /EHsc: Specifies the model of exception handling.
	# /GL:   Enables whole program optimization.
	# /Gm:   Enables minimal rebuild.
	# /GS:   Buffers security check.
	# /MD:   Creates a multithreaded DLL using MSVCRT.lib.
	# /MDd:  Creates a debug multithreaded DLL using MSVCRTD.lib.
	# /O2:   Creates fast code.
	# /Od:   Disables optimization.
	# /Oi:   Generates intrinsic functions.
	# /RTC1: Enables run-time error checking.
	# /sdl:  Enables additional (Windows-specific) security features and warnings.
	# /W4:   Sets which warning level to output.
	# /wd:   Disable warning
	# /Zi:   Generates complete debugging information.
	set(CMAKE_CXX_FLAGS "/EHsc /GS /sdl- /W4 /wd\"4996\" /Zi")
	set(CMAKE_CXX_FLAGS_DEBUG "/Gm /MDd /Od /RTC1")
	set(CMAKE_CXX_FLAGS_RELEASE "/GL /Gm- /MD /O2 /Oi")
else()
	message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} is not recognized.")
endif()

# Check command line to see if OpenMP is to be used.
option(UseOpenMP "UseOpenMP" FALSE)

# Build with OpenMP if desired, and package can be found.
if (UseOpenMP)
	message(STATUS "Attempting to find OpenMP package...")
	find_package(OpenMP)
	if (OPENMP_FOUND)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D USING_OPENMP")
		message(STATUS "Added OpenMP to buildsystem")
	else()
		message(WARNING "Command line asked for OpenMP, but package couldn't be found!")
	endif()
endif()

#Libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/support/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/framework/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Test/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example/)
