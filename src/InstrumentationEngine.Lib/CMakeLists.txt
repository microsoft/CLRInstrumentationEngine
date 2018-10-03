# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)
project(InstrumentationEngine.Lib)
find_package(LibXml2 REQUIRED)

include_directories(. ../../inc/clr/inc ${LIBXML2_INCLUDE_DIR})

build_init(CPP InstrumentationEngine.Lib)

set(src_files
    ./stdafx.cpp
    ./ConfigurationLoader.cpp
    ./InstrumentationMethod.cpp
    ./Logging.cpp
)

add_lib(${PROJECT_NAME}
    STATIC
    true # use_redefines
    false # hide_symbols
    ${src_files}
    )
