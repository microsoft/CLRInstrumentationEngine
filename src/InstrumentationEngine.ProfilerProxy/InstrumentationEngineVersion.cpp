// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#include <regex>
#pragma warning(pop)

using namespace MicrosoftInstrumentationEngine;

// This versionRegex produces the following match result :
//      [0]: Entire Version String
//      [1]: SemanticVersion (eg. 1.0.0)
//      [2]: Preview tag if exists (eg. -build12345)
//      [3]: Debug tag if exists (eg. _Debug)
const std::wregex InstrumentationEngineVersion::versionRegex(_T("^(\\d+\\.\\d+\\.\\d+)(-build\\d+)?(_Debug)?$"));

// static
HRESULT InstrumentationEngineVersion::Create(_In_ const std::wstring& versionStr, _Out_ InstrumentationEngineVersion** ppVersion)
{
    HRESULT hr = S_OK;

    std::wsmatch versionMatch;
    if (std::regex_match(versionStr, versionMatch, versionRegex))
    {
        *ppVersion = new InstrumentationEngineVersion();
        (*ppVersion)->m_versionStr = versionStr;
        (*ppVersion)->m_semanticVersionStr = versionMatch[1];
        (*ppVersion)->m_isPreview = versionMatch[2].matched;
        (*ppVersion)->m_isDebug = versionMatch[3].matched;

        return S_OK;
    }

    return E_INVALIDARG;
}

const std::wstring& InstrumentationEngineVersion::GetSemanticVersionString() const
{
    return m_semanticVersionStr;
}

BOOL InstrumentationEngineVersion::IsPreview() const
{
    return m_isPreview;
}

BOOL InstrumentationEngineVersion::IsDebug() const
{
    return m_isDebug;
}



int InstrumentationEngineVersion::Compare(_In_ const InstrumentationEngineVersion& right) const noexcept
{
    // We want to reverse the default string compare behavior so that
    // for the same samantic version, the preview tag is lower.
    // 1.0.0-build12345 < 1.0.0
    if (m_semanticVersionStr.compare(right.GetSemanticVersionString()) == 0 &&
        IsPreview() != right.IsPreview())
    {
        return (static_cast<std::wstring>(right)).compare(m_versionStr);
    }

    // The default string compare behavior works fine for all other cases.
    return m_versionStr.compare(static_cast<std::wstring>(right));
}