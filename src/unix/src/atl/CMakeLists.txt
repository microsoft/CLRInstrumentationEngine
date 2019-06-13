# Copyright (c) Microsoft Corporation. All rights reserved.
#

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)

build_init(CPP Atl)

set(src_files
    atl.cpp
)

add_lib(atl
    STATIC
    true # use_redefines
    false # hide_symbols
    ${src_files}
    )
