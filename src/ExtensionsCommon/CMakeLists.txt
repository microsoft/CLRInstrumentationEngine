# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.14)
project(ExtensionsCommon)
find_package(LibXml2 REQUIRED)

include_directories(. ${LIBXML2_INCLUDE_DIR})
build_init(CPP ExtensionsCommon)

#note: we purposfully exclude windows-only files,
#including ModuleHandle.cpp, EventLoggingBase.cpp, and PathUtils.cpp
set(src_files
    ./stdafx.cpp
	# ./CrtDiagnostics.cpp
	# ./DataContainerAdapter.cpp
	# ./DataItemContainer.cpp
	# ./DecorationCallbackInfo.cpp
	# ./DecorationCallbacksInfo.cpp
	# ./DecorationCallbacksInfoReader.cpp
	# ./DecorationCallbacksInfoStaticReader.cpp
	# ./Exception.cpp
	# ./InteropInstrumentationHandler.cpp
	# ./LoadArgumentsHelper.cpp
	# ./ManagedRedirectCodeInjector.cpp
	# ./MethodInstrumentationInfoCollection.cpp
	# ./MethodInstrumentationInfoKey.cpp
	# ./NativeInstanceMethodInstrumentationInfo.cpp
	# ./NativeInstanceMethodsCodeInjector.cpp
	# ./ProfilerManagerHostBase.cpp
	# ./ReflectionHelper.cpp
	# ./TextUtils.cpp
	# ./WellKnownInstrumentationDefs.cpp
	# ./WellKnownMethodInfo.cpp
	# ./WellKnownTypeInfo.cpp
    ./InstrumentationMethodBase.cpp
)

add_lib(${PROJECT_NAME}
    STATIC
    false # use_redefines
    true # hide_symbols
    ${src_files}
    )

#target_compile_options(Common.Lib PUBLIC "-H")