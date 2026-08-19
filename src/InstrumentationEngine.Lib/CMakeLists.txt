# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.14)
project(InstrumentationEngine.Lib)

include_directories(.)

build_init(CPP InstrumentationEngine.Lib)

set(src_files
    ./stdafx.cpp
    ./ConfigurationLoader.cpp
    ./ConfigurationLocator.cpp
    ./ConfigurationSource.cpp
    ./DataContainer.cpp
    ./FileLoggerSink.cpp
    ./HostLoggerSink.cpp
    ./InstrumentationMethod.cpp
    ./InstrumentationMethodAttachContext.cpp
    ./InstrumentationMethodSetting.cpp
    ./LoggerService.cpp
    ./Logging.cpp
)

add_lib(${PROJECT_NAME}
    STATIC
    true # use_pal
    true # use_redefines
    false # hide_symbols
    ${src_files}
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    LibXml2::LibXml2
)