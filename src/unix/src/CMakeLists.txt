# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)

build_init(CPP LinuxPal)

include_directories(.)
include_directories(./include)

set(src_files
    misc/file.cpp
    misc/pal.cpp
    misc/resource.cpp
    misc/strsafe.cpp
    misc/utils.cpp
    #misc/threading.cpp
    misc/sync.cpp
    memory/gmem.cpp
    memory/memorystream.cpp
    com/combase.cpp
    com/objbase.cpp
    com/oleaut32.cpp
)

add_lib(linux_pal
    STATIC
    true # use_redefines
    false # hide_symbols
    ${src_files}
    )
