// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CInlineSite
    {
    private:
        mdToken m_inlineeToken;
        mdToken m_inlineSiteToken;
        ModuleID m_moduleID;

    public:
        CInlineSite(
            _In_ mdToken m_inlineeToken,
            _In_ mdToken m_inlineSiteToken,
            _In_ ModuleID m_moduleID
            );

        mdToken GetInlineeToken();
        mdToken GetInlineSiteToken();
        ModuleID GetModuleID();
    };
}