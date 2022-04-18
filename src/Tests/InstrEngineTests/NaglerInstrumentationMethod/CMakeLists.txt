# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.14)

project(NaglerInstrumentationEngine)
find_package(LibXml2 REQUIRED)

include_directories(. ${REPOSITORY_ROOT}/src ${LIBXML2_INCLUDE_DIR})

# don't use the PAL for tests
build_init(CPP InstrumentationEngine)

set(src_files
    ./dllmain.cpp
    ./InstrumentationEngineApi.cpp
    ../../../InstrumentationEngine.Api/InstrumentationEngine_i.cpp
    ./NaglerInstrumentationMethod.cpp
    ./wcrt.cpp
    ./SystemUtils.cpp
    ./stdafx.cpp
)

add_lib(${PROJECT_NAME}
    STATIC
    false # use_pal
    false # use_redefines
    true # hide_symbols
    ${src_files}
    )

target_include_directories(${PROJECT_NAME} PUBLIC ${REPOSITORY_ROOT}/src/unix/inc/mincor )

# For some reason linking against musl-libc we need libintl and on glibc we need libdl.
# They have nothing to do with one another, each is just a quirk of the environment.
if(CLR_CMAKE_PLATFORM_ALPINE_LINUX)
    target_link_libraries(${PROJECT_NAME} :libintl.so)
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
    # NOTE: Explicitly link to the .so version of libdl.so to make sure we don't somehow accidentally
    # start statically linking to these, as these are LGPL components.
    target_link_libraries(${PROJECT_NAME} :libdl.so)
else()
    target_link_libraries(${PROJECT_NAME} dl)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    pthread
    stdc++
    ${LIBXML2_LIBRARIES}
    Common.Lib
    LibXml2
)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    target_link_libraries(${PROJECT_NAME} stdc++fs)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    target_link_libraries(${PROJECT_NAME}
        iconv
        "-framework CoreFoundation"
        "-framework Security"
    )
endif()
