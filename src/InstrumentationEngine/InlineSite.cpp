// Copyright (c) Microsoft Corporation. All rights reserved.
//

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

mdToken MicrosoftInstrumentationEngine::CInlineSite::GetInlineeToken()
{
    return m_inlineeToken;
}

mdToken MicrosoftInstrumentationEngine::CInlineSite::GetInlineSiteToken()
{
    return m_inlineSiteToken;
}

ModuleID MicrosoftInstrumentationEngine::CInlineSite::GetModuleID()
{
    return m_moduleID;
}