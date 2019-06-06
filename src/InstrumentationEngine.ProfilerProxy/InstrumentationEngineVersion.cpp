// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"

using namespace ProfilerProxy;

// This versionRegex produces the following match result :
//      [0]: Entire Version String
//      [1]: SemanticVersion (eg. 1.0.0)
//      [2]: Preview tag if exists (eg. -build12345)
//      [3]: Debug tag if exists (eg. _Debug)
const std::wregex InstrumentationEngineVersion::versionRegex(_T("^(\\d+\\.\\d+\\.\\d+)(-build\\d+)?(_Debug)?$"));

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

const std::wstring& InstrumentationEngineVersion::ToString() const
{
    return m_versionStr;
}

LPCWSTR InstrumentationEngineVersion::c_str() const
{
    return m_versionStr.c_str();
}

int InstrumentationEngineVersion::Compare(_In_ const InstrumentationEngineVersion& right) const noexcept
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