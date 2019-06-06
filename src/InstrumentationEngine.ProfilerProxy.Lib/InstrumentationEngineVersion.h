// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#include <regex>
#pragma warning(pop)

namespace MicrosoftInstrumentationEngine
{
    class InstrumentationEngineVersion
    {
    public:
        static HRESULT Create(_In_ const std::wstring& versionStr, _Out_ InstrumentationEngineVersion** ppVersion);

        const std::wstring& GetVersionString() const;

        const std::wstring& GetSemanticVersionString() const;

        BOOL IsPreview() const;

        BOOL IsDebug() const;

        int Compare(_In_ const InstrumentationEngineVersion& right) const noexcept;

    private:
        static const std::wregex versionRegex;

        InstrumentationEngineVersion(
            _In_ std::wstring versionStr,
            _In_ std::wstring semanticVersionStr,
            _In_ bool isPreview,
            _In_ bool isDebug);

        ~InstrumentationEngineVersion() { }

        std::wstring m_versionStr;
        std::wstring m_semanticVersionStr;
        bool m_isPreview;
        bool m_isDebug;
    };
}