# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)

PROJECT(InstrumentationEngine.Method)

# Use C++11
SET(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")


find_package(LibXml2 REQUIRED)

include_directories(. ${LIBXML2_INCLUDE_DIR})

build_init(CPP InstrumentationEngine.Method)

set(src_files 
	./stdafx.cpp
	./InstrumentationMethodBase.cpp)

add_lib(InstrumentationEngine.Method
    SHARED
    true # use_redefines
    false # hide_symbols
    ${src_files}
    )

# if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    # # Force every object to be included even if it's unused.
    # SET (ATL -Wl,--whole-archive atl -Wl,--no-whole-archive)
    # SET (CORECLRPAL -Wl,--whole-archive coreclrpal -Wl,--no-whole-archive)
    # SET (LINUXPAL -Wl,--whole-archive linux_pal -Wl,--no-whole-archive)
# else()
    # SET (ATL atl)
    # SET (CORECLRPAL coreclrpal)
    # SET (LINUXPAL linux_pal)
# endif(CMAKE_SYSTEM_NAME STREQUAL Linux)


# # For some reason linking against musl-libc we need libintl and on glibc we need libdl.
# # They have nothing to do with one another, each is just a quirk of the environment.
# if(CLR_CMAKE_PLATFORM_ALPINE_LINUX)
    # target_link_libraries(${PROJECT_NAME} :libintl.so)
# elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
    # # NOTE: Explicitly link to the .so version of libdl.so to make sure we don't somehow accidentally
    # # start statically linking to these, as these are LGPL components.
    # target_link_libraries(${PROJECT_NAME} :libdl.so)
# else()
    # target_link_libraries(${PROJECT_NAME} dl)
# endif()

# target_link_libraries(${PROJECT_NAME}
    # pthread
    # stdc++
    # ${LIBXML2_LIBRARIES}
    # ${CORECLRPAL}
    # ${LINUXPAL}
    # ${ATL}
    # palrt
    # )

#ensure that the linux pal gets built before this
# add_dependencies(${PROJECT_NAME} atl)
# add_dependencies(${PROJECT_NAME} linux_pal)