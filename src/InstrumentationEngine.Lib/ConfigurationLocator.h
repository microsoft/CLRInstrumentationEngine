// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ConfigurationSource.h"

namespace MicrosoftInstrumentationEngine
{
    class CConfigurationLocator final
    {
        typedef std::vector<CComPtr<CConfigurationSource>> SourceVector;

    private:
        static constexpr const WCHAR* s_wszConfigurationFilePattern = _T("*.config");
#if defined(_WIN64) || defined(PLATFORM_UNIX)
        static constexpr const WCHAR* s_wszConfigurationPathEnvironmentVariablePrefix = _T("MicrosoftInstrumentationEngine_ConfigPath64_");
#else
        static constexpr const WCHAR* s_wszConfigurationPathEnvironmentVariablePrefix = _T("MicrosoftInstrumentationEngine_ConfigPath32_");
#endif
        static constexpr const WCHAR* s_wszEnvironmentVariableNameValueSeparator = _T("=");
        static constexpr const WCHAR* s_wszEnvironmentVariablePathDelimiter = _T(";");

#ifdef PLATFORM_UNIX
#ifdef X86
        static constexpr const WCHAR* s_wszProductionBreakpointsConfigName = _T("ProductionBreakpoints_x86.config");
        static constexpr const WCHAR* s_wszProfilerPathVariableName = _T("CORECLR_PROFILER_PATH_32");
#else
        static constexpr const WCHAR* s_wszProductionBreakpointsConfigName = _T("ProductionBreakpoints_x64.config");
        static constexpr const WCHAR* s_wszProfilerPathVariableName = _T("CORECLR_PROFILER_PATH_64");
#endif
#endif

    public:
        static HRESULT GetFromEnvironment(
            _In_ SourceVector &sources
            );

#ifndef PLATFORM_UNIX
        static HRESULT GetFromFilesystem(
            _In_ SourceVector& sources
            );
#endif

    private:
#ifndef PLATFORM_UNIX
        static HRESULT AddExpandedPath(
            _In_ SourceVector &sources,
            _In_ LPCWSTR wszConfigurationPath
            );

        static HRESULT AddExpandedPaths(
            _In_ SourceVector &sources,
            _In_ LPCWSTR wszConfigurationPaths
            );
#endif

        static HRESULT AddSource(
            _In_ SourceVector& sources,
            _In_ LPCWSTR wszConfigurationPath
            );
    };
}