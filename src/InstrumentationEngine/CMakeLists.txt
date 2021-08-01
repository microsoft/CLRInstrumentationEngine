# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)
project(InstrumentationEngine)

find_package(LibXml2 REQUIRED)

include_directories(. ../InstrumentationEngine.Lib ${LIBXML2_INCLUDE_DIR})

build_init(CPP InstrumentationEngine)

set(src_files
    ./stdafx.cpp
    ../InstrumentationEngine.Api/InstrumentationEngine_i.cpp
    ./AppDomainCollection.cpp
    ./AppDomainInfo.cpp
    ./ArrayType.cpp
    ./AssemblyInfo.cpp
    #./AssemblyInjector.cpp
    #./AtlModule.cpp
    ./BranchTargetInfo.cpp
    ./CompositeType.cpp
    ./CorHeaders.cpp
    ./CorMethodMalloc.cpp
    ./CorProfilerFunctionControlWrapper.cpp
    ./CorProfilerInfoWrapper.cpp
    ./dllmain.cpp
    ./ExceptionClause.cpp
    ./ExceptionSection.cpp
    ./FunctionType.cpp
    ./GenericInstanceType.cpp
    ./GenericParameterType.cpp
    ./InlineSite.cpp
    ./InlineSiteMap.cpp
    ./Instruction.cpp
    ./InstructionFactory.cpp
    ./InstructionGraph.cpp
    ./LocalVariableCollection.cpp
    ./MethodInfo.cpp
    ./MethodJitInfo.cpp
    ./MethodLocal.cpp
    ./MethodParameter.cpp
    ./ModifierType.cpp
    ./ModuleInfo.cpp
    ./ProfilerManager.cpp
    ./ProfilerManagerForInstrumentationMethod.cpp
    ./SignatureBuilder.cpp
    ./SimpleType.cpp
    ./SingleRetDefaultInstrumentation.cpp
    #./StrongName.cpp
    ./TokenType.cpp
    ./Type.cpp
    ./TypeCreator.cpp
)

add_lib(${PROJECT_NAME}
    SHARED
    true # use_redefines
    false # hide_symbols
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

target_link_libraries(${PROJECT_NAME}
    pthread
    stdc++
    ${LIBXML2_LIBRARIES}
    ${CORECLRPAL}
    ${LINUXPAL}
    ${ATL}
    palrt
    Common.Lib
    InstrumentationEngine.Api
    InstrumentationEngine.Lib
    corguids
    )

#ensure that the linux pal gets built before this
add_dependencies(${PROJECT_NAME} atl)
add_dependencies(${PROJECT_NAME} linux_pal)