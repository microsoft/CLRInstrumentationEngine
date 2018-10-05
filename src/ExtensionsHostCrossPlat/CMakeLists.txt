# Copyright (c) Microsoft Corporation. All rights reserved.
# 

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)
project(MicrosoftExtensionsHost)

include_directories(. ../InstrumentationEngine.Lib ../InstrumentationEngine)

build_init(CPP ExtensionsHostCrossPlat)

set(src_files
    ./stdafx.cpp
    ./dllmain.cpp
    ./CExtensionsHost.cpp
    ./refcount.cpp
    ./ImplQueryInterface.cpp
)

add_lib(${PROJECT_NAME}
    SHARED
    true # use_redefines
    false # hide_symbols
    ${src_files}
    )

# Force every object to be included even if it's unused.
SET (CORECLRPAL -Wl,--whole-archive coreclrpal -Wl,--no-whole-archive)
SET (LINUXPAL -Wl,--whole-archive linux_pal -Wl,--no-whole-archive)

# For some reason linking against musl-libc we need libintl and on glibc we need libdl.
# They have nothing to do with one another, each is just a quirk of the environment.
if(CLR_CMAKE_PLATFORM_ALPINE_LINUX)
    target_link_libraries(${PROJECT_NAME} :libintl.so)
else()
    # NOTE: Explicitly link to the .so version of libdl.so to make sure we don't somehow accidentally
    # start statically linking to these, as these are LGPL components.
    target_link_libraries(${PROJECT_NAME} :libdl.so)
endif()

target_link_libraries(${PROJECT_NAME}
    pthread
    stdc++
    ${CORECLRPAL}
    ${LINUXPAL}
    palrt
    InstrumentationEngine.Lib
    corguids
    :libuuid.so
    :libunwind-x86_64.so
    )
