// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace Agent
{
namespace Settings
{
    class CEnvironment final
    {
    private:
#if defined(_WIN64) || defined(PLATFORM_UNIX)
        static constexpr const WCHAR* s_wszConfigurationPathEnvironmentVariablePrefix = _T("MicrosoftInstrumentationEngine_ConfigPath64_");
#else
        static constexpr const WCHAR* s_wszConfigurationPathEnvironmentVariablePrefix = _T("MicrosoftInstrumentationEngine_ConfigPath32_");
#endif
        static constexpr const WCHAR* s_wszEnvironmentVariableNameValueSeparator = _T("=");
        static constexpr const WCHAR* s_wszEnvironmentVariablePathDelimiter = _T(";");

    public:
        HRESULT GetEnvironmentVariable(
            _In_ const std::wstring& strVariableName,
            _Out_ std::wstring& strVariableValue);

        static HRESULT GetConfigurationPaths(
            _In_ std::vector<ATL::CComBSTR> &paths,
            _In_ const IProfilerManagerLoggingSptr &logger
            );

    private:
        static HRESULT AddExpandedPath(
            _In_ std::vector<ATL::CComBSTR> &paths,
            _In_ LPCWSTR wszConfigurationPath,
            _In_ const IProfilerManagerLoggingSptr &logger
            );

        static HRESULT AddExpandedPaths(
            _In_ std::vector<ATL::CComBSTR> &paths,
            _In_ LPCWSTR wszConfigurationPaths,
            _In_ const IProfilerManagerLoggingSptr &logger
            );
    };
}
}