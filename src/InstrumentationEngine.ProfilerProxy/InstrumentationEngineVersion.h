// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "stdafx.h"

namespace MicrosoftInstrumentationEngine
{
    class InstrumentationEngineVersion
    {
    public:
        static HRESULT Create(_In_ const std::wstring& versionStr, _Out_ InstrumentationEngineVersion** ppVersion);


        const std::wstring& GetSemanticVersionString() const;

        BOOL IsPreview() const;

        BOOL IsDebug() const;

        // Allows conversion to wstring()
        operator std::wstring() const { return m_versionStr; }


        int Compare(_In_ const InstrumentationEngineVersion& right) const noexcept;

    private:
        static const std::wregex versionRegex;

        InstrumentationEngineVersion()
        {
            m_isDebug = false;
            m_isPreview = false;
        }

        ~InstrumentationEngineVersion() { }

        std::wstring m_versionStr;
        std::wstring m_semanticVersionStr;
        bool m_isPreview;
        bool m_isDebug;
    };
}