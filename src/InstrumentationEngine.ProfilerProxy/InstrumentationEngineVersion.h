// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "stdafx.h"

namespace ProfilerProxy
{
    class InstrumentationEngineVersion
    {
    public:
        InstrumentationEngineVersion();

        InstrumentationEngineVersion(_In_ std::wstring versionStr);

        BOOL IsValid() const;

        std::wstring GetSemanticVersionString() const;

        BOOL IsPreview() const;

        BOOL IsDebug() const;

        std::wstring ToString() const;

        LPCWSTR c_str() const;

        int Compare(const InstrumentationEngineVersion& right) const noexcept;

    private:
        std::wstring m_versionStr;
        std::wstring m_semanticVersionStr;
        BOOL m_isPreview;
        BOOL m_isDebug;
    };
}