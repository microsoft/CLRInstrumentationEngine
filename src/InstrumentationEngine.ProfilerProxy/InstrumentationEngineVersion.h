// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "stdafx.h"

namespace ProfilerProxy
{
    class InstrumentationEngineVersion
    {
    public:
        static HRESULT Create(_In_ const std::wstring& versionStr, _Out_ InstrumentationEngineVersion& validVersion)
        {
            HRESULT hr = S_OK;

            std::wsmatch versionMatch;
            validVersion = InstrumentationEngineVersion();
            if (std::regex_match(versionStr, versionMatch, versionRegex))
            {
                validVersion.m_versionStr = versionStr;
                validVersion.m_semanticVersionStr = versionMatch[1];
                validVersion.m_isPreview = versionMatch[2].matched;
                validVersion.m_isDebug = versionMatch[3].matched;

                return S_OK;
            }

            return E_FAIL;
        }

        const std::wstring& GetSemanticVersionString() const;

        BOOL IsPreview() const;

        BOOL IsDebug() const;

        const std::wstring& ToString() const;

        LPCWSTR c_str() const;

        int Compare(_In_ const InstrumentationEngineVersion& right) const noexcept;

        static const std::wregex versionRegex;

    private:
        std::wstring m_versionStr;
        std::wstring m_semanticVersionStr;
        BOOL m_isPreview;
        BOOL m_isDebug;
    };
}