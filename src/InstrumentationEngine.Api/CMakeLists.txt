# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)
PROJECT(InstrumentationEngine.Api C CXX)

include_directories(.)
setup_midl_lib()

build_init(CPP InstrumentationEngine.Api)

set(src_files ./InstrumentationEngine_api.cpp)

add_lib(InstrumentationEngine.Api
    STATIC
    true # use_pal
    false # use_redefines
    false # hide_symbols
    ${src_files}
    )
