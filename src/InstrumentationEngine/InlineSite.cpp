// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "InlineSite.h"

MicrosoftInstrumentationEngine::CInlineSite::CInlineSite(
            _In_ mdToken inlineeToken,
            _In_ mdToken inlineSiteToken,
            _In_ ModuleID moduleID
            ) : m_inlineeToken(inlineeToken),
                m_inlineSiteToken(inlineSiteToken),
                m_moduleID(moduleID)
{
}

mdToken MicrosoftInstrumentationEngine::CInlineSite::GetInlineeToken() const
{
    return m_inlineeToken;
}

mdToken MicrosoftInstrumentationEngine::CInlineSite::GetInlineSiteToken() const
{
    return m_inlineSiteToken;
}

ModuleID MicrosoftInstrumentationEngine::CInlineSite::GetModuleID() const
{
    return m_moduleID;
}