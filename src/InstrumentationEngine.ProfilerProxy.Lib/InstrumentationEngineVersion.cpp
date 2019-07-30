// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"

#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy can be used
#include <regex>
#pragma warning(pop)

using namespace ProfilerProxy;

// This versionRegex produces the following match result :
//      [0]: Entire Version String
//      [1]: SemanticVersion (eg. 1.0.0)
//      [2]: Preview tag if exists (eg. -build12345)
//      [3]: Debug tag if exists (eg. _Debug)
// This version regex only matches against Major version 1
const std::wregex InstrumentationEngineVersion::versionRegex(_T("^(1\\.\\d+\\.\\d+)(-build\\d+)?(_Debug)?$"));

// static
HRESULT InstrumentationEngineVersion::Create(
    _In_ const std::wstring& versionStr,
    _Out_ InstrumentationEngineVersion** ppVersion)
{
    HRESULT hr = S_OK;

    *ppVersion = nullptr;
    std::wsmatch versionMatch;
    if (std::regex_match(versionStr, versionMatch, versionRegex))
    {
        *ppVersion = new InstrumentationEngineVersion(
            versionStr,
            versionMatch[1],
            versionMatch[2].matched,
            versionMatch[3].matched);

        return S_OK;
    }

    return E_INVALIDARG;
}

const std::wstring& InstrumentationEngineVersion::GetVersionString() const
{
    return m_versionStr;
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
        return right.GetVersionString().compare(m_versionStr);
    }

    // The default string compare behavior works fine for all other cases.
    return m_versionStr.compare(right.GetVersionString());
}

InstrumentationEngineVersion::InstrumentationEngineVersion(
    _In_ std::wstring versionStr,
    _In_ std::wstring semanticVersionStr,
    _In_ bool isPreview,
    _In_ bool isDebug)
{
    m_versionStr = versionStr;
    m_semanticVersionStr = semanticVersionStr;
    m_isDebug = isDebug;
    m_isPreview = isPreview;
}
