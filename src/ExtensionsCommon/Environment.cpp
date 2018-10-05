// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"

#include "PathUtils.h"

#include "Environment.h"

namespace Agent
{
namespace Settings
{
    HRESULT CEnvironment::GetEnvironmentVariable(
        _In_ const std::wstring& strVariableName,
        _Out_ std::wstring& strVariableValue)
    {
        const int MaxVaiableSize = 1024;
        std::wstring strVariableValueTemp(1024, L'\n');

        auto dwCount = ::GetEnvironmentVariable(
            strVariableName.c_str(),
            &strVariableValueTemp[0],
            MaxVaiableSize);

        strVariableValueTemp.resize(dwCount);
        strVariableValue.swap(strVariableValueTemp);

        if (0 == dwCount)
        {
            auto dwLastError = ::GetLastError();
            if (ERROR_ENVVAR_NOT_FOUND != dwLastError)
            {
                return HRESULT_FROM_WIN32(dwLastError);
            }

            return S_FALSE;
        }

        return S_OK;
    }

    // static
    HRESULT CEnvironment::GetConfigurationPaths(
        _In_ std::vector<ATL::CComBSTR> &paths,
        _In_ const IProfilerManagerLoggingSptr &logger
        )
    {
        // Pull config paths from environment variables by checking for known prefix.
        auto freeEnvironmentBlock = [](LPTCH p) { FreeEnvironmentStrings(p); };
        auto pEnvironmentBlock = std::unique_ptr<WCHAR, decltype(freeEnvironmentBlock)>(GetEnvironmentStrings(), freeEnvironmentBlock);

        // GetEnvironmentStrings returns a pointer into a block of memory that contains the environment variables
        // of this process. They are in the format of:
        //
        // VarName1=VarValue1\0
        // VarName2=VarValue2\0
        // ...
        // VarNameN=VarValueN\0\0
        //
        // This can be interpreted as the strings for each environment variable are back-to-back and the empty string
        // indicates the end of the environment block.

        const size_t cchConfigurationPathEnvironmentVariablePrefix = wcslen(s_wszConfigurationPathEnvironmentVariablePrefix);

        // Start: The beginning of the environment block is the first environment variable.
        // Current: Only process the current variable if it is not empty.
        // Next: Move to the next variable, which starts immediately after the '\0' in the current string.
        for (LPWSTR wszVariable = pEnvironmentBlock.get(); *wszVariable; wszVariable += wcslen(wszVariable) + 1)
        {
            // If the variable name starts with the known prefix
            if (0 == wcsncmp(wszVariable, s_wszConfigurationPathEnvironmentVariablePrefix, cchConfigurationPathEnvironmentVariablePrefix))
            {
                LPCWSTR wszVariableValue = wcsstr(wszVariable, s_wszEnvironmentVariableNameValueSeparator) + 1;
                
                if (wszVariableValue != nullptr)
                {
                    // The value found after the first '=' character is the variable value; get the configuration paths from the value
                    IfFailToRet(logger, AddExpandedPaths(paths, wszVariableValue, logger));
                }
            }
        }

        return S_OK;
    }

    HRESULT CEnvironment::AddExpandedPath(
        _In_ std::vector<ATL::CComBSTR> &paths,
        _In_ LPCWSTR wszPath,
        _In_ const IProfilerManagerLoggingSptr &logger
        )
    {
        const size_t cchExpandedValueWithNull = ExpandEnvironmentStrings(wszPath, nullptr, 0);
        if (0 == cchExpandedValueWithNull)
        {
            TraceMsgTo(logger, _T("CEnvironment::GetConfigurationPaths - ExpandEnvironmentStrings(szVariableValue, nullptr, 0) failed: 0x%08X"), HRESULT_FROM_WIN32(GetLastError()));

            return S_OK;
        }

        // Expand the value using environment variables
        std::unique_ptr<WCHAR[]> pwszExpandedValue = std::make_unique<WCHAR[]>(cchExpandedValueWithNull);
        if (0 == ExpandEnvironmentStrings(wszPath, pwszExpandedValue.get(), cchExpandedValueWithNull))
        {
            TraceMsgTo(logger, _T("CEnvironment::GetConfigurationPaths - ExpandEnvironmentStrings(szVariableValue, pszExpandedValue.get(), cchExpandedValueWithNull) failed: 0x%08X"), HRESULT_FROM_WIN32(GetLastError()));

            return S_OK;
        }

        // Get the absolute path of the expanded value
        std::unique_ptr<WCHAR[]> pwszAbsolutePath(_wfullpath(nullptr, pwszExpandedValue.get(), 0));
        // NOTE: From MSDN: "If there is an error (for example, if the value passed in relPath includes a drive letter
        // that is not valid or cannot be found, or if the length of the created absolute path name (absPath) is greater
        // than maxLength), the function returns NULL."
        // Thus, null does not necessary indicate an error in getting the absolute path, but could mean that the path has
        // and invalid drive letter or that the path does not exist.
        if (nullptr == pwszAbsolutePath.get())
        {
#ifdef PLATFORM_UNIX
            TraceMsgTo(logger, _T("CEnvironment::GetConfigurationPaths - _wfullpath returned nullptr for '%S'"), pszExpandedValue.get());
#else
            TraceMsgTo(logger, _T("CEnvironment::GetConfigurationPaths - _wfullpath returned nullptr for '%s'"), pwszExpandedValue.get());
#endif
        }
        // Check that the configuraiton file exists
        else if (!Agent::Io::PathUtils::Exists(pwszAbsolutePath.get()))
        {
#ifdef PLATFORM_UNIX
            TraceMsgTo(logger, _T("CEnvironment::GetConfigurationPaths - unable to find config path: '%S'"), pszAbsolutePath.get());
#else
            TraceMsgTo(logger, _T("CEnvironment::GetConfigurationPaths - unable to find config path: '%s'"), pwszAbsolutePath.get());
#endif
        }
        else
        {
            paths.push_back(ATL::CComBSTR(pwszAbsolutePath.get()));
        }

        return S_OK;
    }

    // static
    HRESULT CEnvironment::AddExpandedPaths(
        _In_ std::vector<ATL::CComBSTR> &paths,
        _In_ LPCWSTR wszPaths,
        _In_ const IProfilerManagerLoggingSptr &logger
        )
    {
        HRESULT hr = S_OK;

        const size_t cchPathsWithNull = wcslen(wszPaths) + 1;
        std::unique_ptr<WCHAR[]> pwszPaths = std::make_unique<WCHAR[]>(cchPathsWithNull);
        wcscpy_s(pwszPaths.get(), cchPathsWithNull, wszPaths);

        WCHAR* wszContext = nullptr;
        WCHAR* wszPath = wcstok_s(pwszPaths.get(), s_wszEnvironmentVariablePathDelimiter, &wszContext);
        while (nullptr != wszPath)
        {
            IfFailToRet(logger, AddExpandedPath(paths, wszPath, logger));

            wszPath = wcstok_s(nullptr, s_wszEnvironmentVariablePathDelimiter, &wszContext);
        }

        return S_OK;
    }
}
}