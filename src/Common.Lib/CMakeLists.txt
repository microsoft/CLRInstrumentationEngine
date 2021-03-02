# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)
project(Common.Lib)
find_package(LibXml2 REQUIRED)

include_directories(. ${LIBXML2_INCLUDE_DIR})
build_init(CPP Common.Lib)

#note: we purposfully exclude windows-only files,
#including ModuleHandle.cpp, EventLoggingBase.cpp, and PathUtils.cpp
set(src_files
    ./stdafx.cpp
    ./refcount.cpp
)

add_lib(${PROJECT_NAME}
    STATIC
    false # use_redefines
    true # hide_symbols
    ${src_files}
    )

#target_compile_options(Common.Lib PUBLIC "-H")