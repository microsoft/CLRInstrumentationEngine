# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.14)

# include google test. The version must match the same version as is used in the test nuget
fetch_google_test()

project(Common.LibTests)
find_package(LibXml2 REQUIRED)

# the midl directory is needed to replace the rpc headers referenced by midl generated headers.
include_directories(. ${REPOSITORY_ROOT}/src/unix/inc/midl ${REPOSITORY_ROOT}/src ${LIBXML2_INCLUDE_DIR})

build_init(CPP Common.LibTests)

#note: we purposefully exclude windows-only files,
#including ModuleHandle.cpp, EventLoggingBase.cpp, and PathUtils.cpp
set(src_files
    ./pch.cpp
    ./StringConversionTests.cpp
    ./XmlDocumentTests.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${src_files}
)

target_link_libraries(
    ${PROJECT_NAME}
    ${LIBXML2_LIBRARIES}
    Common.Lib
    gtest_main
)

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    target_link_libraries(${PROJECT_NAME}
        iconv
    )
endif(CMAKE_SYSTEM_NAME STREQUAL Darwin)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
