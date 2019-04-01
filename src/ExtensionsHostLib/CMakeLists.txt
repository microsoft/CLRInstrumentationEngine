# Copyright (c) Microsoft Corporation. All rights reserved.
#

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)
project(MicrosoftExtensionsHost)

include_directories(. ../InstrumentationEngine.Lib ../InstrumentationEngine)

build_init(CPP ExtensionsHost)

set(src_files
    ./stdafx.cpp
    ./CExtensionsHost.cpp
    ../InstrumentationEngine/refcount.cpp
    ../InstrumentationEngine/ImplQueryInterface.cpp
)

add_lib(${PROJECT_NAME}
    STATIC
    true # use_redefines
    false # hide_symbols
    ${src_files}
    )