// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "InlineSite.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    typedef unordered_map<mdToken, shared_ptr<CInlineSite>> TInlineSiteMap;
    typedef unordered_map<ModuleID, shared_ptr<TInlineSiteMap>> TModulesToInlineSitesMap;
    typedef unordered_map<mdToken, shared_ptr<TModulesToInlineSitesMap>> TInlineeToModulesMap;

    class __declspec(uuid("4D34D55F-28F0-4FCB-94B4-5F1942E97511"))
    CInlineSiteMap : public IUnknown, public CModuleRefCount
    {
    private:

        CRITICAL_SECTION m_cs;

        // Multi-level Map of the inlinee tokens through modules into inline sites.
        TInlineeToModulesMap m_inlineSiteMap;

    public:

        CInlineSiteMap();
        ~CInlineSiteMap();

        DEFINE_DELEGATED_REFCOUNT_ADDREF(CInlineSiteMap);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CInlineSiteMap);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IUnknown*>(static_cast<CInlineSiteMap*>(this)),
                riid,
                ppvObject
                );
        }

    public:
        HRESULT AddInlineSite(_In_ mdToken inlinee, _In_ mdToken inlineSite, _In_ IModuleInfo* pSiteModuleInfo);
        HRESULT RemoveInlineSite(_In_ mdToken inlinee, _In_ mdToken inlineSite, _In_ IModuleInfo* pSiteModuleInfo);

        HRESULT FindInlineSites(_In_ mdToken inlinee, _In_ vector<ModuleID>& moduleIds, _In_ vector<mdToken>& methodTokens);
    };
}