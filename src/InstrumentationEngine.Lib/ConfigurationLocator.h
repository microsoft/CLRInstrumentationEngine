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
#ifdef PLATFORM_UNIX
#ifdef X86
        static constexpr const char* s_szConfigurationPathEnvironmentVariablePrefix = "MicrosoftInstrumentationEngine_ConfigPath32_";
#else
        static constexpr const char* s_szConfigurationPathEnvironmentVariablePrefix = "MicrosoftInstrumentationEngine_ConfigPath64_";
#endif
        static constexpr const char s_cEnvironmentVariableNameValueSeparator = '=';
        static constexpr const char s_cEnvironmentVariablePathDelimiter = ':';
#else  // PLATFORM_UNIX
#ifdef _M_IX86
        static constexpr const WCHAR* s_wszConfigurationPathEnvironmentVariablePrefix = _T("MicrosoftInstrumentationEngine_ConfigPath32_");
#elif defined(_M_ARM64)
        static constexpr const WCHAR* s_wszConfigurationPathEnvironmentVariablePrefix = _T("MicrosoftInstrumentationEngine_ConfigPathARM64_");
#else
        static constexpr const WCHAR* s_wszConfigurationPathEnvironmentVariablePrefix = _T("MicrosoftInstrumentationEngine_ConfigPath64_");
#endif
        static constexpr const WCHAR* s_wszEnvironmentVariableNameValueSeparator = _T("=");
        static constexpr const WCHAR* s_wszEnvironmentVariablePathDelimiter = _T(";");
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
