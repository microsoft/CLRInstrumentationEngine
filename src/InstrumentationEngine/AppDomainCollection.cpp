// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "AppDomainCollection.h"
#include "EnumeratorImpl.h"
#include "ProfilerManager.h"

MicrosoftInstrumentationEngine::CAppDomainCollection::CAppDomainCollection(_In_ CProfilerManager* pProfilerManager) :
    m_pProfilerManager(pProfilerManager)
{
    InitializeCriticalSection(&m_cs);
    DEFINE_REFCOUNT_NAME(CAppDomainCollection);
}

MicrosoftInstrumentationEngine::CAppDomainCollection::~CAppDomainCollection()
{
    DeleteCriticalSection(&m_cs);
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::OnShutdown()
{
    CCriticalSectionHolder lock(&m_cs);

    m_appDomains.erase(m_appDomains.begin(), m_appDomains.end());

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::AddAppDomainInfo(_In_ AppDomainID appdomainId, _In_ CAppDomainInfo* pAppDomainInfo)
{
    HRESULT hr = S_OK;
    CCriticalSectionHolder lock(&m_cs);

    m_appDomains[appdomainId] = pAppDomainInfo;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::GetAppDomainCount(_Out_ DWORD* pdwCount)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pdwCount);

    CCriticalSectionHolder lock(&m_cs);

    *pdwCount = (DWORD)(m_appDomains.size());

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::GetAppDomainById(_In_ AppDomainID appDomainId, _Out_ IAppDomainInfo** ppAppDomainInfo)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CAppDomainCollection::GetAppDomainById"));
    IfNullRetPointer(ppAppDomainInfo);
    *ppAppDomainInfo = nullptr;

    CCriticalSectionHolder lock(&m_cs);

    std::unordered_map<AppDomainID, CComPtr<CAppDomainInfo>>::iterator iter = m_appDomains.find(appDomainId);

    if (iter == m_appDomains.end())
    {
        CLogging::LogMessage(_T("CAppDomainCollection::GetAppDomainById - Failed to find specified appdomain %04x"), appDomainId);
        return E_FAIL;
    }

    *ppAppDomainInfo = (iter->second);
    (*ppAppDomainInfo)->AddRef();

    CLogging::LogMessage(_T("End CAppDomainCollection::GetAppDomainById"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::RemoveAppDomainInfo(_In_ AppDomainID appDomainId)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CAppDomainCollection::RemoveAppDomainInfo"));

    CCriticalSectionHolder lock(&m_cs);

    std::unordered_map<AppDomainID, CComPtr<CAppDomainInfo>>::iterator iter = m_appDomains.find(appDomainId);

    if (iter == m_appDomains.end())
    {
        CLogging::LogError(_T("CAppDomainCollection::RemoveAppDomainInfo - Failed to find specified appdomain %04x"), appDomainId);
        return E_FAIL;
    }

    m_appDomains.erase(iter);

    CLogging::LogMessage(_T("End CAppDomainCollection::RemoveAppDomainInfo"));

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::GetAppDomainIDs(_In_ DWORD cAppDomains, _Out_ DWORD* pcActual, _Out_writes_(cAppDomains) AppDomainID* pAppDomainIDs)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CAppDomainCollection::GetAppDomainIDs"));

    IfNullRetPointer(pcActual);
    IfNullRetPointer(pAppDomainIDs);

    memset(pAppDomainIDs, cAppDomains, 0);

    *pcActual = 0;

    CCriticalSectionHolder lock(&m_cs);

    if (cAppDomains >= m_appDomains.size())
    {
        CLogging::LogError(_T("CAppDomainCollection::GetAppDomainIDs - Buffer too small"));
        return E_FAIL;
    }

    DWORD i = 0;
    for (std::pair<AppDomainID, CComPtr<CAppDomainInfo>> p : m_appDomains)
    {
        pAppDomainIDs[i] = (AppDomainID)(p.first);
        i++;
    }

    *pcActual = i;

    CLogging::LogMessage(_T("End CAppDomainCollection::GetAppDomainIDs"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::GetAppDomains(_Out_ IEnumAppDomainInfo** ppEnumAppDomains)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CAppDomainCollection::GetAppDomains"));

    CCriticalSectionHolder lock(&m_cs);

    vector<CComPtr<IAppDomainInfo>> vecAppDomains;
    for (std::pair<AppDomainID, CComPtr<CAppDomainInfo>> p : m_appDomains)
    {
        vecAppDomains.push_back((IAppDomainInfo*)(p.second));
    }

    CComPtr<CEnumerator<IEnumAppDomainInfo, IAppDomainInfo>> pEnumerator;
    pEnumerator.Attach(new CEnumerator<IEnumAppDomainInfo, IAppDomainInfo>);
    if (pEnumerator == NULL)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRet(pEnumerator->Initialize(vecAppDomains));

    *ppEnumAppDomains = (IEnumAppDomainInfo*)pEnumerator;
    (*ppEnumAppDomains)->AddRef();

    CLogging::LogMessage(_T("End CAppDomainCollection::GetAppDomains"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::GetAssemblyInfoById(_In_ AssemblyID assemblyID, _Out_ IAssemblyInfo** ppAssemblyInfo)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CAppDomainCollection::GetAssemblyInfoById"));

    CCriticalSectionHolder lock(&m_cs);

    for (std::pair<AppDomainID, CComPtr<CAppDomainInfo>> p : m_appDomains)
    {
        hr = p.second->GetAssemblyInfoById(assemblyID, ppAssemblyInfo);
        if (SUCCEEDED(hr))
        {
            CLogging::LogMessage(_T("End CAppDomainCollection::GetAssemblyInfoById - found assembly"));
            return S_OK;
        }
    }

    CLogging::LogMessage(_T("End CAppDomainCollection::GetAssemblyInfoById - no assembly found"));

    return E_FAIL;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::GetModuleInfoById(_In_ ModuleID moduleID, _Out_ IModuleInfo** ppModuleInfo)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CAppDomainCollection::GetModuleInfoById"));

    CCriticalSectionHolder lock(&m_cs);

    for (std::pair<AppDomainID, CComPtr<CAppDomainInfo>> p : m_appDomains)
    {
        hr = p.second->GetModuleInfoById(moduleID, ppModuleInfo);
        if (SUCCEEDED(hr))
        {
            CLogging::LogMessage(_T("End CAppDomainCollection::GetModuleInfoById - found module"));
            return S_OK;
        }
    }

    CLogging::LogMessage(_T("End CAppDomainCollection::GetModuleInfoById - no module found"));

    return E_FAIL;
}


HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::GetModuleInfosByMvid(GUID mvid, _Out_opt_ IEnumModuleInfo** ppEnum)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppEnum);

    CLogging::LogMessage(_T("Starting CAppDomainCollection::GetModuleInfosByMvid"));
    CCriticalSectionHolder lock(&m_cs);
    *ppEnum = nullptr;

    vector<CComPtr<IModuleInfo>> vecModules;

    for (std::pair<AppDomainID, CComPtr<CAppDomainInfo>> p : m_appDomains)
    {
        CComPtr<CAppDomainInfo> pAppDomain = p.second;

        CComPtr<IEnumModuleInfo> pEnumModuleInfo;
        hr = pAppDomain->GetModuleInfosByMvid(mvid, &pEnumModuleInfo);
        if (SUCCEEDED(hr))
        {
            DWORD cActual = 0;

            CComPtr<IModuleInfo> pModuleInfo;
            hr = pEnumModuleInfo->Next(1, &pModuleInfo, &cActual);
            while (SUCCEEDED(hr) && pModuleInfo != NULL)
            {
                vecModules.push_back(pModuleInfo);

                pModuleInfo.Release();
                hr = pEnumModuleInfo->Next(1, &pModuleInfo, &cActual);
            }
        }
    }

    CComPtr<CEnumerator<IEnumModuleInfo, IModuleInfo>> pEnumerator;
    pEnumerator.Attach(new CEnumerator<IEnumModuleInfo, IModuleInfo>);
    if (pEnumerator == NULL)
    {
        return E_OUTOFMEMORY;
    }
    IfFailRet(pEnumerator->Initialize(vecModules));

    *ppEnum = (IEnumModuleInfo*)pEnumerator;
    (*ppEnum)->AddRef();

    CLogging::LogMessage(_T("End CAppDomainCollection::GetModuleInfosByMvid"));

    return S_OK;
}

// Obtain a method info instance from a function id. This method info is for querying information only,
// and cannot be used for instrumentation.
HRESULT MicrosoftInstrumentationEngine::CAppDomainCollection::GetMethodInfoById(_In_ FunctionID functionID, _Out_ IMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;

    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    IfFailRet(m_pProfilerManager->GetRealCorProfilerInfo(&pCorProfilerInfo));

    ClassID classId;
    ModuleID moduleId;
    mdToken methodToken;
    IfFailRet(pCorProfilerInfo->GetFunctionInfo(functionID, &classId, &moduleId, &methodToken));

    CComPtr<IModuleInfo> pModuleInfo;
    IfFailRet(GetModuleInfoById(moduleId, &pModuleInfo));

    IfFailRet(pModuleInfo->GetMethodInfoById(functionID, ppMethodInfo));

    return hr;
}