// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ConfigurationLocator.h"
#ifndef PLATFORM_UNIX
#include "../Common.Lib/PathUtils.h"
#include "../Common.Lib/ModuleUtils.h"
#include "../Common.Headers/SafeFindFileHandle.h"
#endif

using namespace CommonLib;
using namespace std;

namespace MicrosoftInstrumentationEngine
{
    // static
    HRESULT CConfigurationLocator::GetFromEnvironment(
        _In_ SourceVector &sources
        )
    {
        HRESULT hr = S_OK;

#ifndef PLATFORM_UNIX
        // Pull config paths from environment variables by checking for known prefix.
        // Ignore casing of variable names on Windows.
        auto freeEnvironmentBlock = [](LPTCH p) { FreeEnvironmentStrings(p); };
        auto pEnvironmentBlock = unique_ptr<WCHAR, decltype(freeEnvironmentBlock)>(GetEnvironmentStrings(), freeEnvironmentBlock);

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
            const size_t cchVariable = wcslen(wszVariable);
            if (cchVariable > cchConfigurationPathEnvironmentVariablePrefix)
            {
                int compareResult = CompareStringOrdinal(
                    wszVariable,
                    (int)cchConfigurationPathEnvironmentVariablePrefix,
                    s_wszConfigurationPathEnvironmentVariablePrefix,
                    (int)cchConfigurationPathEnvironmentVariablePrefix,
                    TRUE /* bIgnoreCase*/);
                if (CSTR_EQUAL == compareResult)
                {
                    LPCWSTR wszVariableValue = wcsstr(wszVariable, s_wszEnvironmentVariableNameValueSeparator) + 1;

                    if (wszVariableValue != nullptr)
                    {
                        // The value found after the first '=' character is the variable value; get the configuration paths from the value
                        IfFailRet(AddExpandedPaths(sources, wszVariableValue));
                    }
                }
            }
        }
#else
        WCHAR wszProfilerPath[MAX_PATH];
        if (!GetEnvironmentVariable(s_wszProfilerPathVariableName, wszProfilerPath, MAX_PATH))
        {
            return E_UNEXPECTED;
        }

        WCHAR* pFileName = PathFindFileName(wszProfilerPath);
        if (pFileName == wszProfilerPath)
        {
            return E_UNEXPECTED;
        }
        *pFileName = _T('\0');

        StringCchCatW(wszProfilerPath, MAX_PATH, s_wszProductionBreakpointsConfigName);

        IfFailRet(AddSource(sources, wszProfilerPath));
#endif

        return S_OK;
    }

#ifndef PLATFORM_UNIX
    // static
    HRESULT CConfigurationLocator::GetFromFilesystem(
        _In_ SourceVector& sources
    )
    {
        HRESULT hr = S_OK;

        HINSTANCE hModule = ModuleUtils::GetHostModule();
        IfFalseRet(nullptr != hModule, HRESULT_FROM_WIN32(::GetLastError()));

        const wstring strModuleFolder = ModuleUtils::GetModuleDirectory(hModule);
        IfFalseRet(strModuleFolder.size() > 0, E_FAIL);

        // Search for files
        const wstring strFileSearchPattern = PathUtils::Combine(
            strModuleFolder,
            s_wszConfigurationFilePattern);

        WIN32_FIND_DATA findFileData;
        SafeFindFileHandle hSearchHandle = FindFirstFile(strFileSearchPattern.c_str(), &findFileData);
        if (hSearchHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                // Skip any directories; we only want files
                if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    continue;
                }

                wstring wsConfigPath = PathUtils::Combine(strModuleFolder, findFileData.cFileName);

                IfFailRet(AddSource(sources, wsConfigPath.c_str()));
            } while (FindNextFile(hSearchHandle, &findFileData));
        }

        return S_OK;
    }

    // static
    HRESULT CConfigurationLocator::AddExpandedPath(
        _In_ SourceVector &sources,
        _In_ LPCWSTR wszPath
        )
    {
        HRESULT hr = S_OK;

        const DWORD cchExpandedValueWithNull = ExpandEnvironmentStrings(wszPath, nullptr, 0);
        if (0 == cchExpandedValueWithNull)
        {
            CLogging::LogMessage(_T("ConfigurationLocator::AddExpandedPath - ExpandEnvironmentStrings(szVariableValue, nullptr, 0) failed: 0x%08X"), HRESULT_FROM_WIN32(GetLastError()));

            return S_OK;
        }

        // Expand the value using environment variables
        unique_ptr<WCHAR[]> pwszExpandedValue = make_unique<WCHAR[]>(cchExpandedValueWithNull);
        if (0 == ExpandEnvironmentStrings(wszPath, pwszExpandedValue.get(), cchExpandedValueWithNull))
        {
            CLogging::LogMessage(_T("ConfigurationLocator::AddExpandedPath - ExpandEnvironmentStrings(szVariableValue, pszExpandedValue.get(), cchExpandedValueWithNull) failed: 0x%08X"), HRESULT_FROM_WIN32(GetLastError()));

            return S_OK;
        }

        // Get the absolute path of the expanded value
        unique_ptr<WCHAR[]> pwszAbsolutePath(_wfullpath(nullptr, pwszExpandedValue.get(), 0));
        // NOTE: From MSDN: "If there is an error (for example, if the value passed in relPath includes a drive letter
        // that is not valid or cannot be found, or if the length of the created absolute path name (absPath) is greater
        // than maxLength), the function returns NULL."
        // Thus, null does not necessary indicate an error in getting the absolute path, but could mean that the path has
        // an invalid drive letter or that the path does not exist.
        if (nullptr == pwszAbsolutePath)
        {
            CLogging::LogMessage(_T("CConfigurationLocator::AddExpandedPath - _wfullpath returned nullptr for '%s'"), pwszExpandedValue.get());
        }
        else
        {
            IfFailRet(AddSource(sources, pwszAbsolutePath.get()));
        }

        return S_OK;
    }

    // static
    HRESULT CConfigurationLocator::AddExpandedPaths(
        _In_ SourceVector &sources,
        _In_ LPCWSTR wszPaths
        )
    {
        HRESULT hr = S_OK;

        const size_t cchPathsWithNull = wcslen(wszPaths) + 1;
        unique_ptr<WCHAR[]> pwszPaths = make_unique<WCHAR[]>(cchPathsWithNull);
        wcscpy_s(pwszPaths.get(), cchPathsWithNull, wszPaths);

        WCHAR* wszContext = nullptr;
        WCHAR* wszPath = wcstok_s(pwszPaths.get(), s_wszEnvironmentVariablePathDelimiter, &wszContext);
        while (nullptr != wszPath)
        {
            IfFailRet(AddExpandedPath(sources, wszPath));

            wszPath = wcstok_s(nullptr, s_wszEnvironmentVariablePathDelimiter, &wszContext);
        }

        return S_OK;
    }
#endif

    // static
    HRESULT CConfigurationLocator::AddSource(
        _In_ SourceVector& sources,
        _In_ LPCWSTR wszConfigurationPath
        )
    {
        if (::GetFileAttributes(wszConfigurationPath) == INVALID_FILE_ATTRIBUTES)
        {
            CLogging::LogMessage(_T("CConfigurationLocator::AddSource - unable to find config path '%s': 0x%08X"), wszConfigurationPath, HRESULT_FROM_WIN32(GetLastError()));
            return S_FALSE;
        }

        CComPtr<CConfigurationSource> pSource;
        pSource.Attach(new (nothrow) CConfigurationSource(wszConfigurationPath));
        IfFalseRet(nullptr != pSource, E_OUTOFMEMORY);

        sources.push_back(pSource.p);

        return S_OK;
    }
}