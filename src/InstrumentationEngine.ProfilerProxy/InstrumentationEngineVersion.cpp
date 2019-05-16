// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"

using namespace ProfilerProxy;

InstrumentationEngineVersion::InstrumentationEngineVersion()
{
    m_versionStr = _T("");
    m_semanticVersionStr = _T("");
    m_isPreview = FALSE;
    m_isDebug = FALSE;
}

InstrumentationEngineVersion::InstrumentationEngineVersion(_In_ std::wstring versionStr)
{
    // This versionRegex produces the following match result :
    //      [0]: Entire Version String
    //      [1]: SemanticVersion (eg. 1.0.0)
    //      [2]: Preview tag if exists (eg. -build12345)
    //      [3]: Debug tag if exists (eg. _Debug)
    std::wregex versionRegex(_T("^(\\d+\\.\\d+\\.\\d+)(-build\\d+)?(_Debug)?$"));
    std::wsmatch versionMatch;
    if (std::regex_match(versionStr, versionMatch, versionRegex))
    {
        m_versionStr = versionStr;
        m_semanticVersionStr = versionMatch[1];
        m_isPreview = versionMatch[2].matched;
        m_isDebug = versionMatch[3].matched;
    }
    else
    {
        m_isPreview = FALSE;
        m_isDebug = FALSE;
    }
}

BOOL InstrumentationEngineVersion::IsValid() const
{
    return !m_versionStr.empty() && !m_semanticVersionStr.empty();
}

std::wstring InstrumentationEngineVersion::GetSemanticVersionString() const
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

std::wstring InstrumentationEngineVersion::ToString() const
{
    return m_versionStr;
}

LPCWSTR InstrumentationEngineVersion::c_str() const
{
    return m_versionStr.c_str();
}

int InstrumentationEngineVersion::Compare(const InstrumentationEngineVersion& right) const noexcept
{
    // We want to reverse the default string compare behavior so that
    // for the same samantic version, the preview tag is lower.
    // 1.0.0-build12345 < 1.0.0
    if (m_semanticVersionStr.compare(right.GetSemanticVersionString()) == 0 &&
        m_isPreview != right.IsPreview())
    {
        return right.ToString().compare(ToString());
    }

    // The default string compare behavior works fine for all other cases.
    return ToString().compare(right.ToString());
}