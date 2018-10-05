// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "InlineSiteMap.h"

MicrosoftInstrumentationEngine::CInlineSiteMap::CInlineSiteMap()
{
    InitializeCriticalSection(&m_cs);
}

MicrosoftInstrumentationEngine::CInlineSiteMap::~CInlineSiteMap()
{
    DeleteCriticalSection(&m_cs);
}

HRESULT MicrosoftInstrumentationEngine::CInlineSiteMap::AddInlineSite(_In_ mdToken inlinee, _In_ mdToken inlineSite, _In_ IModuleInfo* pSiteModuleInfo)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    shared_ptr<TModulesToInlineSitesMap> pModuleToInlineSitesMap;
    TInlineeToModulesMap::iterator iter = m_inlineSiteMap.find(inlinee);
    if (iter == m_inlineSiteMap.end())
    {
        pModuleToInlineSitesMap = make_shared<TModulesToInlineSitesMap>();
        m_inlineSiteMap[inlinee] = pModuleToInlineSitesMap;
    }
    else
    {
        pModuleToInlineSitesMap = iter->second;
    }

    ModuleID moduleID;
    IfFailRet(pSiteModuleInfo->GetModuleID(&moduleID));

    shared_ptr<TInlineSiteMap> pSitesMap = nullptr;
    TModulesToInlineSitesMap::iterator moduleInlineSitesIter = pModuleToInlineSitesMap->find(moduleID);
    if (moduleInlineSitesIter == pModuleToInlineSitesMap->end())
    {
        pSitesMap = make_shared<TInlineSiteMap>();
        (*pModuleToInlineSitesMap)[moduleID] = pSitesMap;
    }
    else
    {
        pSitesMap = moduleInlineSitesIter->second;
    }

    TInlineSiteMap::iterator inlineSitesIter = pSitesMap->find(inlineSite);
    if (inlineSitesIter == pSitesMap->end())
    {
        shared_ptr<CInlineSite> pInlineSite = make_shared<CInlineSite>(inlinee, inlineSite, moduleID);
        (*pSitesMap)[inlineSite] = pInlineSite;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInlineSiteMap::FindInlineSites(
    _In_ mdToken inlinee,
    _In_ vector<ModuleID>& moduleIds,
    _In_ vector<mdToken>& methodTokens
    )
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    TInlineeToModulesMap::iterator modulesMapIter = m_inlineSiteMap.find(inlinee);
    if (modulesMapIter != m_inlineSiteMap.end())
    {
        shared_ptr<TModulesToInlineSitesMap> pModulesToSites = modulesMapIter->second;

        for (pair<ModuleID, shared_ptr<TInlineSiteMap>> moduleInlineSitesPair : *pModulesToSites)
        {
            shared_ptr<TInlineSiteMap> inlineSiteMap = moduleInlineSitesPair.second;

            for (pair<mdToken, shared_ptr<CInlineSite>> pSitePair : *inlineSiteMap)
            {
                methodTokens.push_back(pSitePair.second->GetInlineSiteToken());
                moduleIds.push_back(pSitePair.second->GetModuleID());
            }
        }
    }

    return S_OK;
}