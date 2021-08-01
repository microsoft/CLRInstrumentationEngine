# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.14)

# include google test. The version must match the same version as is used in the test nuget
fetch_google_test()

project(InstrumentationEngine.LibTests)
find_package(LibXml2 REQUIRED)

include_directories(. ${REPOSITORY_ROOT}/src ${LIBXML2_INCLUDE_DIR})

# don't use the PAL for tests
build_init(CPP InstrumentationEngine.LibTests)

#note: we purposfully exclude windows-only files,
#including ModuleHandle.cpp, EventLoggingBase.cpp, and PathUtils.cpp
set(src_files
    ./pch.cpp
    ./ConfigurationLoaderTests.cpp
    ./ConfigurationLocatorTests.cpp
    ../../InstrumentationEngine.Api/InstrumentationEngine_i.cpp
)

add_exe(
    ${PROJECT_NAME}
    true # use_redefines
    ${src_files}
    )

# Force every object to be included even if it's unused.
SET (ATL -Wl,--whole-archive atl -Wl,--no-whole-archive)
SET (CORECLRPAL -Wl,--whole-archive coreclrpal -Wl,--no-whole-archive)
SET (LINUXPAL -Wl,--whole-archive linux_pal -Wl,--no-whole-archive)
#SET (LINUXPALRT -Wl,--whole-archive palrt -Wl,--no-whole-archive)


# For some reason linking against musl-libc we need libintl and on glibc we need libdl.
# They have nothing to do with one another, each is just a quirk of the environment.
if(CLR_CMAKE_PLATFORM_ALPINE_LINUX)
    target_link_libraries(${PROJECT_NAME} :libintl.so)
else()
    # NOTE: Explicitly link to the .so version of libdl.so to make sure we don't somehow accidentally
    # start statically linking to these, as these are LGPL components.
    target_link_libraries(${PROJECT_NAME} :libdl.so)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    pthread
    stdc++
    ${LIBXML2_LIBRARIES}
    ${CORECLRPAL}
    ${LINUXPAL}
    ${ATL}
    InstrumentationEngine.Api
    InstrumentationEngine.Lib
    Common.Lib
    palrt
    corguids
    stdc++fs
    gtest_main
)

add_dependencies(${PROJECT_NAME} atl)
add_dependencies(${PROJECT_NAME} linux_pal)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})