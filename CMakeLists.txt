cmake_minimum_required(VERSION 3.8)

if ((NOT(DEFINED CMAKE_TOOLCHAIN_FILE)) OR (TTN_TOOLCHAIN STREQUAL "default")) # Make sure that selected toolchain is not overwritten
	find_program(CCACHE_PROGRAM ccache HINTS /ifxhome/titan_sw/ccache)
	if(CCACHE_PROGRAM)
		message("Found ${CCACHE_PROGRAM}")
		set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
	endif()
endif()


project(tictac LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(TTN_TOOLCHAIN "default" CACHE STRING "Remembers the toolchain initially selected. If empty, the system defaults is used.")

# Project wide setup
set(CMAKE_CXX_STANDARD 17)
include(FindPackageHandleStandardArgs)

find_package(Boost 1.54)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()
#message(STATUS "Found Boost ${Boost_VERSION}")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install/${TTN_TOOLCHAIN}" CACHE PATH "Install prefix" FORCE)
endif ()

# Externally provided content

# Main targets built by this project
add_subdirectory(src)

