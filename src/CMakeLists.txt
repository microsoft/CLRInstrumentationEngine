# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Require at least version 2.8.12 of CMake
cmake_minimum_required(VERSION 2.8.12)

IF(NOT EXISTS "${CMAKE_INSTALL_PREFIX}" OR NOT IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
    message(FATAL_ERROR "Required property CMAKE_INSTALL_PREFIX not correctly defined.")
    return()
endif()

IF(NOT EXISTS "${CORECLR_PAL_ROOT}" OR NOT IS_DIRECTORY "${CORECLR_PAL_ROOT}")
    message(FATAL_ERROR "Required property CORECLR_PAL_ROOT not correctly defined.")
    return()
endif()

IF(NOT EXISTS "${REPOSITORY_ROOT}" OR NOT IS_DIRECTORY "${REPOSITORY_ROOT}")
    message(FATAL_ERROR "Required property REPOSITORY_ROOT not correctly defined.")
    return()
endif()

IF(NOT EXISTS "${REPOSITORY_ROOT}/src" OR NOT IS_DIRECTORY "${REPOSITORY_ROOT}/src")
    message(FATAL_ERROR "REPOSITORY_ROOT/src does not exist.")
    return()
endif()

if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(FATAL_ERROR "Unknown build type! Set CMAKE_BUILD_TYPE to Debug or Release")
    return()
endif ()

IF(NOT EXISTS "${INTERMEDIATES_DIR}" OR NOT IS_DIRECTORY "${INTERMEDIATES_DIR}")
    message(FATAL_ERROR "Required property INTERMEDIATES_DIR not correctly defined.")
    return()
endif()


find_program(AWK awk)
if (AWK STREQUAL "AWK-NOTFOUND")
    message(FATAL_ERROR "AWK not found")
endif()

# Macro to initialize various build properties based on build language/type.
# Supported arguments:
#   build_language - C, CPP
#   build_type - ClrInstrumentationEngine
#
# Make sure to call this macro as the first thing in the CMakeLists.txt file.
# For ex.,
#    build_init (CPP, ClrInstrumentationEngine)
#      will initialize the environment to build C++ ClrInstrumentationEngine sources.
macro (build_init build_language build_type)
    set (PROJECT_BUILD_LANGUAGE, "${build_language}")

    if ("${build_language}" STREQUAL CPP)
        message (" Build type : CPP")
        # The -fms-extensions enable the stuff like __if_exists, __declspec(uuid()), etc.
        add_compile_options(-fms-extensions)
        # Enabling this option helps with certain ares (like allowing forward declaration of Enums),
        # but this results in disabling certain new types like (int32_t) which will cause the
        # standard c++ template library to throw errors as these types will be based on other types.
        # For this reason, this option can't be enabled.
        # add_compile_options(-fms-compatibility)
        add_compile_options(-fdelayed-template-parsing)
        add_compile_options(-fdiagnostics-show-category=name)
        add_compile_options(-std=c++14)
        add_compile_options(-g)
        add_compile_options(-pthread)
        add_compile_options(-ferror-limit=1000)
    endif ("${build_language}" STREQUAL CPP)

    # Sets the project build type.
    set (PROJECT_BUILD_TYPE, "${build_type}")


    message (" Build type : ${build_type}")
    add_definitions(-DVSDEBUGENG_USE_CPP11_SCOPED_ENUMS)
    add_definitions(-DFEATURE_PAL)
    add_definitions(-DPORTABLE)

    #TODO: HACK this is pulling in InstrumentationEngine.h from the output of the windows build.
    # We are forced to do this because we don't have MIDL to generate source on Linux. Need to
    # find a better way to do this. Also note we only support 64bit Linux build so hard coding
    # that part.
    include_directories(${REPOSITORY_ROOT}/bin/${CMAKE_BUILD_TYPE}/x64)

    include_directories(${REPOSITORY_ROOT}/inc/clr/prof) # Must be ahead of Core CLR PAL
    include_directories(${REPOSITORY_ROOT}/src/unix/inc)
    include_directories(${REPOSITORY_ROOT}/src/unix/inc/atl)
    include_directories(${REPOSITORY_ROOT}/src/unix/inc/empty)
    include_directories(${REPOSITORY_ROOT}/src/unix/inc/winsdk)
    include_directories(${REPOSITORY_ROOT}/src/InstrumentationEngine.Api)
    include_directories(${CORECLR_PAL_ROOT}/inc)
    include_directories(${CORECLR_PAL_ROOT}/inc/rt)
    include_directories(${GENERATED_INCLUDE_DIR})
    include_directories(${REPOSITORY_ROOT}/inc/clr/extra)

    add_definitions(-DCOM_NO_WINDOWS_H)
    add_definitions(-DUNICODE)
    add_definitions(-D_ATL_DISABLE_NOTHROW_NEW)
    add_definitions(-D_ATL_NO_DEBUG_CRT)
    add_definitions(-DFEATURE_CORECLR_HOST=1)
    add_definitions(-D__PLACEMENT_NEW_INLINE)
    add_definitions(-DPAL_STDCPP_COMPAT)
    add_definitions(-DENABLE_DOWNLEVEL_FOR_NLS)
    add_definitions(-DPLATFORM_UNIX)
    add_definitions(-DXPLAT=1)

    if (CMAKE_SYSTEM_NAME STREQUAL Linux)
        add_definitions(-D__LINUX__)
        add_definitions(-D_LINUX_)
    endif(CMAKE_SYSTEM_NAME STREQUAL Linux)

    add_definitions(-DBIT64)

    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        add_definitions(-DDEBUG)
        # add_definitions(-D_DEBUG) TODO: Enable this
    endif ()

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
        message("Detected Linux x86_64")
        add_definitions(-DLINUX64)
        add_definitions(-D_M_AMD64)
        add_definitions(-DTARGET_AMD64)
    else (CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
        message(FATAL_ERROR "error: Detected non x86_64 target processor. Not supported!")
    endif(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)

    # Ignored warnings
    # TODO: Reenable these
    add_compile_options(-Wno-macro-redefined)
    add_compile_options(-Wno-ignored-attributes)
    add_compile_options(-Wno-unused-value)
    add_compile_options(-Wno-null-arithmetic)
    add_compile_options(-Wno-delete-non-virtual-dtor)
    add_compile_options(-Wno-tautological-constant-out-of-range-compare)
    add_compile_options(-Wno-extra-tokens)
    add_compile_options(-Wno-logical-op-parentheses)
    add_compile_options(-Wno-comment)
    add_compile_options(-Wno-invalid-offsetof)
    add_compile_options(-Wno-microsoft)
    add_compile_options(-Wno-null-conversion)
    add_compile_options(-Wno-missing-declarations)
    add_compile_options(-Wno-duplicate-decl-specifier)
    add_compile_options(-Wno-switch)
    add_compile_options(-Wno-uninitialized)
    add_compile_options(-Wno-ignored-pragmas)
    add_compile_options(-Wno-new-returns-null)
    add_compile_options(-Wno-deprecated-declarations)
    add_compile_options(-Wno-delete-incomplete)
    add_compile_options(-Wno-pragma-pack)

    # Turn all enabled warnings into errors
    add_compile_options(-Werror)

    add_compile_options(-fPIC)
endmacro (build_init)


# Adds a library and optionally adds dependency on generated file pal_redefines.h for


# projects that depend on the PAL.
macro(add_lib lib_name type use_redefines hide_symbols)
    add_library(${lib_name}
        ${type}
        ${ARGN})

    if (${use_redefines})
        add_dependencies(${lib_name} pal_redefines_header)
    endif (${use_redefines})

    if (${hide_symbols})
        set_property(TARGET ${lib_name} APPEND_STRING PROPERTY COMPILE_FLAGS "-fvisibility=hidden")
    endif(${hide_symbols})
endmacro(add_lib lib_name type use_redefines hide_symbols sources)

# Adds an executable and optionally adds dependency on generated file pal_redefines.h for
# projects that depend on the PAL.
macro(add_exe exe_name use_redefines)
    add_executable(${exe_name}
        ${ARGN})

    if (${use_redefines})
        add_dependencies(${exe_name} pal_redefines_header)
    endif (${use_redefines})
endmacro(add_exe exe_name use_redefines sources)

# Adds install target
macro(add_install type name)
    install(${type} ${name} ${ARGN}
        DESTINATION .)
endmacro(add_install target_name type)

macro(setup_midl_lib)
    # For Linux build only. Linux builds are not able to
    # automatically generate IIDs for IUnknown Interfaces
    # using __uuidof(..). So, we generate a static library
    # that can be linked into shared libs that need the
    # IID symbols

    include_directories(${REPOSITORY_ROOT}/src/unix/inc/midl)

    # ensure __cplusplus code complilation is used
    add_definitions(-D__cplusplus)

    # ensure that our own definitions of IID, etc. are used
    add_definitions(-D_MIDL_USE_GUIDDEF_)
endmacro(setup_midl_lib)

function(generate_exports_file inputFilename versionName outputFilename)
    set(AWK_SCRIPT generateversionscript.awk)

    add_custom_command(
        OUTPUT ${outputFilename}
        COMMAND ${AWK} -f ${REPOSITORY_ROOT}/src/unix/scripts/${AWK_SCRIPT} -v versionName=${versionName} ${inputFilename} >${outputFilename}
        DEPENDS ${inputFilename} ${REPOSITORY_ROOT}/src/unix/scripts/${AWK_SCRIPT}
        COMMENT "Generating exports file ${outputFilename}"
    )
    set_source_files_properties(${outputFilename}
                              PROPERTIES GENERATED TRUE)
endfunction()

# Set the project name
project(ClrInstrumentationEngine)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(CLR_CMAKE_PLATFORM_UNIX 1)

    # Detect Linux ID
    if(DEFINED CLR_CMAKE_LINUX_ID)
        if(CLR_CMAKE_LINUX_ID STREQUAL alpine)
            set(CLR_CMAKE_PLATFORM_ALPINE_LINUX 1)
        endif()
    endif()

    set(GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/inc)
    file(MAKE_DIRECTORY ${GENERATED_INCLUDE_DIR})

    set(PAL_SYMBOL_PREFIX clrie_)
    set(PAL_REDEFINES_HEADER ${GENERATED_INCLUDE_DIR}/pal_redefines.h)
endif(CMAKE_SYSTEM_NAME STREQUAL Linux)

# Set flag to indicate if this will be a 64bit Linux build
if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
    set(IS_64BIT_BUILD 1)
endif (CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)

# Ensure that on MacOS, dylibs are referenced as '@rpath/example.dylib' instead of just 'example.dylib'. Otherwise they
# will not be found unless the working directory == vsdbg directory.
set(CMAKE_MACOSX_RPATH true)

# Don't allow shared libraries to be built with undefined symbols
# Mac builds complain about these, Linux needs the switch
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-allow-shlib-undefined" )
endif(CMAKE_SYSTEM_NAME STREQUAL Linux)


if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    message(FATAL_ERROR "Darwin build not supported.")
    return()
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(PLAT_DLL_EXT "so")
else()
    message(FATAL_ERROR "Unknown CMAKE_SYSTEM_NAME '${CMAKE_SYSTEM_NAME}'.")
    return()
endif()

# Add the PAL imported targets, redefining the necessary symbols on Linux
# See Concord:/src/vsdbg/libvsbaseservices/PALRedefines.md for more info on this
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL armv7l)
        find_program(OBJCOPY ${TOOLCHAIN}-objcopy)
    else()
        find_program(OBJCOPY objcopy)
    endif()
    if(OBJCOPY STREQUAL "OBJCOPY-NOTFOUND")
        message(FATAL_ERROR "objcopy not found")
    endif()

    set(UNIX_EXPORTS_FILE ${REPOSITORY_ROOT}/src/unix/scripts/unixexports.src)

    # Custom target to generate the pal_redefines header which is used on Linux to link
    # against the pal libs with redefined symbols
    set(HEADER_REDEFINES_AWK_SCRIPT ${REPOSITORY_ROOT}/src/unix/scripts/generateredefinesheader.awk)

    add_custom_command(
        OUTPUT ${PAL_REDEFINES_HEADER}
        COMMAND ${AWK} -f ${HEADER_REDEFINES_AWK_SCRIPT} -v prefix=${PAL_SYMBOL_PREFIX} ${UNIX_EXPORTS_FILE} > ${PAL_REDEFINES_HEADER}
        DEPENDS ${UNIX_EXPORTS_FILE} ${HEADER_REDEFINES_AWK_SCRIPT}
        COMMENT "Generating PAL redefines header -> ${PAL_REDEFINES_HEADER}"
    )

    add_custom_target(pal_redefines_header DEPENDS ${PAL_REDEFINES_HEADER})

    # Create the objcopy redefines file
    set(REDEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/vsbaseservices.redefines)
    set(REDEFINES_AWK_SCRIPT ${REPOSITORY_ROOT}/src/unix/scripts/generatesymbolredefines.awk)

    add_custom_command(
        OUTPUT ${REDEFINES_FILE}
        COMMAND ${AWK} -f ${REDEFINES_AWK_SCRIPT} -v prefix=${PAL_SYMBOL_PREFIX} ${UNIX_EXPORTS_FILE} > ${REDEFINES_FILE}
        DEPENDS ${UNIX_EXPORTS_FILE} ${REDEFINES_AWK_SCRIPT}
        COMMENT "Generating PAL symbol redefines file -> ${REDEFINES_FILE}"
    )

    add_custom_target(pal_redefines DEPENDS ${REDEFINES_FILE})

    function(add_library_pal target_name do_redefinition lib_location)
        get_filename_component(lib_file_name ${lib_location} NAME)
        set(redefine_lib_location ${CMAKE_CURRENT_BINARY_DIR}/${lib_file_name})

        if(${do_redefinition})
            # Copy the PAL lib from it's original location and do the redefine
            add_custom_command(
                OUTPUT ${redefine_lib_location}
                COMMAND ${OBJCOPY} --redefine-syms ${REDEFINES_FILE} ${lib_location} ${redefine_lib_location}
                DEPENDS pal_redefines ${lib_location} ${REDEFINES_FILE}
                COMMENT "Redefining exported PAL symbols in ${lib_location}"
            )
        else()
            configure_file(${lib_location} ${redefine_lib_location} COPYONLY)
        endif(${do_redefinition})

        add_custom_target(${target_name}_redefines DEPENDS ${redefine_lib_location})

        add_library(${target_name} STATIC IMPORTED)
        set_property(TARGET ${target_name} PROPERTY IMPORTED_LOCATION ${redefine_lib_location})

        add_dependencies(${target_name} ${target_name}_redefines)
    endfunction()
else()
    message(FATAL_ERROR "Linux is only supported platform.")
endif()
add_library_pal(coreclrpal true ${CORECLR_PAL_ROOT}/lib/${CMAKE_BUILD_TYPE}/libcoreclrpal.a)
add_library_pal(palrt true ${CORECLR_PAL_ROOT}/lib/${CMAKE_BUILD_TYPE}/libpalrt.a)
add_library_pal(corguids false ${CORECLR_PAL_ROOT}/lib/${CMAKE_BUILD_TYPE}/libcorguids.a)

add_subdirectory(unix/src)
add_subdirectory(unix/src/atl)
add_subdirectory(InstrumentationEngine.Api)
add_subdirectory(InstrumentationEngine.Lib)
add_subdirectory(InstrumentationEngine)

