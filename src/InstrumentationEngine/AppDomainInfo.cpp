// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "AppDomainInfo.h"
#include "EnumeratorImpl.h"

MicrosoftInstrumentationEngine::CAppDomainInfo::CAppDomainInfo(_In_ AppDomainID appDomainId) :
    m_appDomainId(appDomainId),
    m_bIsInitialized(false),
    m_bIsSystemDomain(false),
    m_bIsSharedDomain(false),
    m_bIsDefaultDomain(false)
{
    DEFINE_REFCOUNT_NAME(CAppDomainInfo);
    InitializeCriticalSection(&m_cs);
}

MicrosoftInstrumentationEngine::CAppDomainInfo::~CAppDomainInfo()
{
    DeleteCriticalSection(&m_cs);
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::FinishInitialization(_In_ ICorProfilerInfo* pRawICorProfilerInfo)
{
    HRESULT hr = S_OK;

    // Get name regardless of whether previously initialized, since appdomains can be renamed.
    ULONG cchName = 0;
    ULONG cchNameLength = 0;
    ProcessID processID = 0;

    if(FAILED(pRawICorProfilerInfo->GetAppDomainInfo(m_appDomainId, cchName, &cchNameLength, NULL, &processID)))
    {
        CLogging::LogError(_T("Unable to get AppDomain info for id %08" PRIxPTR), m_appDomainId);
        return E_FAIL;
    }
    if (cchNameLength == 0)
    {
        CLogging::LogError(_T("Bogus appdomain name length %04x"), cchNameLength);
        return E_FAIL;
    }

    CAutoVectorPtr<WCHAR> nameBuffer(new WCHAR[cchNameLength]);
    if (!nameBuffer)
    {
        CLogging::LogError(_T("Out of memory during buffer allocation"));
        return E_OUTOFMEMORY;
    }

    cchName = cchNameLength;
    if(FAILED(pRawICorProfilerInfo->GetAppDomainInfo(m_appDomainId, cchName, &cchNameLength, nameBuffer, &processID)) || cchName != cchNameLength)
    {
        CLogging::LogError(_T("Could not get AppDomain name"));
        return S_OK;
    }

    m_bstrAppDomainName = nameBuffer;

    // If just a rename then skip rest of initialization.
    if (!m_bIsInitialized)
    {
        CLogging::LogMessage(_T("Initialized appdomain in CAppDomainInfo::FinishInitialization"));

#ifndef PLATFORM_UNIX
        if (wcscmp(_T("EE Shared Assembly Repository"), m_bstrAppDomainName) == 0)
        {
            m_bIsSharedDomain = true;
        }
        else if (_wcsicmp(_T("mscorlib.dll"), m_bstrAppDomainName) == 0 ||
            _wcsicmp(_T("System.Private.CoreLib.dll"), m_bstrAppDomainName) == 0)
        {
            m_bIsSystemDomain = true;
        }
        else if (wcscmp(L"DefaultDomain", m_bstrAppDomainName) == 0)
        {
            m_bIsDefaultDomain = true;
        }
#else
        if (PAL_wcscmp(_T("EE Shared Assembly Repository"), m_bstrAppDomainName) == 0)
        {
            m_bIsSharedDomain = true;
        }
        else if (PAL_wcscmp(_T("mscorlib.dll"), m_bstrAppDomainName) == 0 ||
            PAL_wcscmp(_T("System.Private.CoreLib.dll"), m_bstrAppDomainName) == 0)
        {
            m_bIsSystemDomain = true;
        }
        else if (PAL_wcscmp(_T("DefaultDomain"), m_bstrAppDomainName) == 0)
        {
            m_bIsDefaultDomain = true;
        }
#endif

        m_bIsInitialized = true;
    }
    else
    {
        CLogging::LogMessage(_T("Renamed appdomain in CAppDomainInfo::FinishInitialization"));
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::AddAssemblyInfo(_In_ CAssemblyInfo* pAssemblyInfo)
{
    HRESULT hr = S_OK;
    CCriticalSectionHolder lock(&m_cs);

    AssemblyID assemblyId;
    IfFailRet(pAssemblyInfo->GetID(&assemblyId));
    m_assemblyInfos[assemblyId] = pAssemblyInfo;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::AssemblyInfoUnloaded(_In_ CAssemblyInfo* pAssemblyInfo)
{
    HRESULT hr = S_OK;
    CCriticalSectionHolder lock(&m_cs);

    AssemblyID assemblyId;
    IfFailRet(pAssemblyInfo->GetID(&assemblyId));
    m_assemblyInfos.erase(assemblyId);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::AddModuleInfo(_In_ CModuleInfo* pModuleInfo)
{
    HRESULT hr = S_OK;
    CCriticalSectionHolder lock(&m_cs);

    ModuleID moduleId;
    IfFailRet(pModuleInfo->GetModuleID(&moduleId));
    m_moduleInfos[moduleId] = pModuleInfo;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::ModuleInfoUnloaded(_In_ CModuleInfo* pModuleInfo)
{
    HRESULT hr = S_OK;
    CCriticalSectionHolder lock(&m_cs);

    ModuleID moduleId;
    IfFailRet(pModuleInfo->GetModuleID(&moduleId));
    m_moduleInfos.erase(moduleId);

    return hr;
}

shared_ptr<list<AssemblyID>> MicrosoftInstrumentationEngine::CAppDomainInfo::GetAssemblyIds()
{
    CCriticalSectionHolder lock(&m_cs);

    shared_ptr<list<AssemblyID>> pAssemblyIds = make_shared<list<AssemblyID>>();
    for (std::pair<AssemblyID, CComPtr<CAssemblyInfo>> assemblyInfoPair : m_assemblyInfos)
    {
        pAssemblyIds->emplace_back(assemblyInfoPair.first);
    }

    return pAssemblyIds;
}

shared_ptr<list<ModuleID>> MicrosoftInstrumentationEngine::CAppDomainInfo::GetModuleIds()
{
    CCriticalSectionHolder lock(&m_cs);

    shared_ptr<list<ModuleID>> pModuleIds = make_shared<list<ModuleID>>();
    for (std::pair<ModuleID, CComPtr<CModuleInfo>> moduleInfoPair : m_moduleInfos)
    {
        pModuleIds->emplace_back(moduleInfoPair.first);
    }

    return pModuleIds;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetAppDomainId(_Out_ AppDomainID* pAppDomainId)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pAppDomainId);

    *pAppDomainId = m_appDomainId;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetIsSystemDomain(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);
    IfFalseRet(m_bIsInitialized, E_FAIL);

    *pbValue = m_bIsSystemDomain;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetIsSharedDomain(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);
    IfFalseRet(m_bIsInitialized, E_FAIL);

    *pbValue = m_bIsSharedDomain;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetIsDefaultDomain(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);
    IfFalseRet(m_bIsInitialized, E_FAIL);

    *pbValue = m_bIsDefaultDomain;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetName(_Out_ BSTR* pbstrName)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbstrName);
    IfFalseRet(m_bIsInitialized, E_FAIL);

    CComBSTR bstrName = m_bstrAppDomainName;
    *pbstrName = bstrName.Detach();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetAssemblies(_Out_ IEnumAssemblyInfo** ppAssemblyInfos)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppAssemblyInfos);
    *ppAssemblyInfos = NULL;

    CCriticalSectionHolder lock(&m_cs);

    vector<CComPtr<IAssemblyInfo>> vecAssemblies;
    for (std::pair<AssemblyID, CComPtr<CAssemblyInfo>> p : m_assemblyInfos)
    {
        vecAssemblies.push_back((IAssemblyInfo*)(p.second));
    }

    CComPtr<CEnumerator<IEnumAssemblyInfo, IAssemblyInfo>> pEnumerator;
    pEnumerator.Attach(new CEnumerator<IEnumAssemblyInfo, IAssemblyInfo>);
    if (pEnumerator == NULL)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRet(pEnumerator->Initialize(vecAssemblies));

    *ppAssemblyInfos = (IEnumAssemblyInfo*)pEnumerator;
    (*ppAssemblyInfos)->AddRef();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetModules(_Out_ IEnumModuleInfo** ppModuleInfos)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppModuleInfos);
    *ppModuleInfos = NULL;

    CCriticalSectionHolder lock(&m_cs);

    vector<CComPtr<IModuleInfo>> vecModules;
    for (std::pair<ModuleID, CComPtr<CModuleInfo>> p : m_moduleInfos)
    {
        vecModules.push_back((IModuleInfo*)(p.second));
    }

    CComPtr<CEnumerator<IEnumModuleInfo, IModuleInfo>> pEnumerator;
    pEnumerator.Attach(new CEnumerator<IEnumModuleInfo, IModuleInfo>);
    if (pEnumerator == NULL)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRet(pEnumerator->Initialize(vecModules));

    *ppModuleInfos = (IEnumModuleInfo*)pEnumerator;
    (*ppModuleInfos)->AddRef();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetAssemblyInfoById(_In_ AssemblyID assemblyID, _Out_ IAssemblyInfo** ppAssemblyInfo)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppAssemblyInfo);
    *ppAssemblyInfo = NULL;

    CCriticalSectionHolder lock(&m_cs);

    std::unordered_map<AppDomainID, CComPtr<CAssemblyInfo>>::iterator iter = m_assemblyInfos.find(assemblyID);
    if (iter == m_assemblyInfos.end())
    {
        CLogging::LogMessage(_T("CAppDomainCollection::GetAssemblyInfoById - Failed to find specified assembly %08" PRIxPTR), assemblyID);
        return E_FAIL;
    }

    *ppAssemblyInfo = iter->second;
    (*ppAssemblyInfo)->AddRef();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetAssemblyInfosByName(_In_ BSTR pszAssemblyName, _Out_ IEnumAssemblyInfo** ppAssemblyInfos)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppAssemblyInfos);
    *ppAssemblyInfos = NULL;

    CCriticalSectionHolder lock(&m_cs);

    vector<CComPtr<IAssemblyInfo>> vecAssemblies;
    for (std::pair<AssemblyID, CComPtr<CAssemblyInfo>> p : m_assemblyInfos)
    {
        CComPtr<IAssemblyInfo> pAssemblyInfo = (IAssemblyInfo*)(p.second);

        CComBSTR bstrCurrName;
        IfFailRet(pAssemblyInfo->GetName(&bstrCurrName));

#ifndef PLATFORM_UNIX
        if (wcscmp(pszAssemblyName, bstrCurrName) == 0)
#else
        if (PAL_wcscmp(pszAssemblyName, bstrCurrName) == 0)
#endif
        {
            vecAssemblies.push_back(pAssemblyInfo);
        }
    }

    CComPtr<CEnumerator<IEnumAssemblyInfo, IAssemblyInfo>> pEnumerator;
    pEnumerator.Attach(new CEnumerator<IEnumAssemblyInfo, IAssemblyInfo>);
    if (pEnumerator == NULL)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRet(pEnumerator->Initialize(vecAssemblies));
    *ppAssemblyInfos = pEnumerator.Detach();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetModuleCount(_Out_ ULONG* pcModuleInfos)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pcModuleInfos);

    CCriticalSectionHolder lock(&m_cs);

    *pcModuleInfos = ((ULONG)m_moduleInfos.size());

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetModuleInfoById(_In_ ModuleID moduleId, _Out_ IModuleInfo** ppModuleInfo)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppModuleInfo);
    *ppModuleInfo = NULL;

    CCriticalSectionHolder lock(&m_cs);

    std::unordered_map<ModuleID, CComPtr<CModuleInfo>>::iterator iter = m_moduleInfos.find(moduleId);
    if (iter == m_moduleInfos.end())
    {
        CLogging::LogMessage(_T("CAppDomainCollection::GetModuleInfoById - Failed to find specified assembly %08" PRIxPTR), moduleId);
        return E_FAIL;
    }

    *ppModuleInfo = iter->second;
    (*ppModuleInfo)->AddRef();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetModuleInfosByMvid(GUID mvid, _Out_opt_ IEnumModuleInfo** ppModuleInfo)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppModuleInfo);
    *ppModuleInfo = NULL;

    CCriticalSectionHolder lock(&m_cs);

    vector<CComPtr<IModuleInfo>> vecModules;
    for (std::pair<AssemblyID, CComPtr<CModuleInfo>> p : m_moduleInfos)
    {
        CComPtr<IModuleInfo> pModuleInfo = (IModuleInfo*)(p.second);

        GUID currMvid;
        IfFailRet(pModuleInfo->GetMVID(&currMvid));

        if (currMvid == mvid)
        {
            vecModules.push_back(pModuleInfo);
        }
    }

    CComPtr<CEnumerator<IEnumModuleInfo, IModuleInfo>> pEnumerator;
    pEnumerator.Attach(new CEnumerator<IEnumModuleInfo, IModuleInfo>);
    if (pEnumerator == NULL)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRet(pEnumerator->Initialize(vecModules));
    *ppModuleInfo = (IEnumModuleInfo*)pEnumerator;
    (*ppModuleInfo)->AddRef();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAppDomainInfo::GetModuleInfosByName(_In_ BSTR pszModuleName, _Out_ IEnumModuleInfo** ppEnumModuleInfo)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppEnumModuleInfo);
    *ppEnumModuleInfo = NULL;

    CCriticalSectionHolder lock(&m_cs);

    vector<CComPtr<IModuleInfo>> vecModules;
    for (std::pair<AssemblyID, CComPtr<CModuleInfo>> p : m_moduleInfos)
    {
        CComPtr<IModuleInfo> pModuleInfo = (IModuleInfo*)(p.second);

        CComBSTR bstrCurrName;
        IfFailRet(pModuleInfo->GetModuleName(&bstrCurrName));

#ifndef PLATFORM_UNIX
		if (wcscmp(pszModuleName, bstrCurrName) == 0)
#else
        if (PAL_wcscmp(pszModuleName, bstrCurrName) == 0)
#endif
        {
            vecModules.push_back(pModuleInfo);
        }
    }

    CComPtr<CEnumerator<IEnumModuleInfo, IModuleInfo>> pEnumerator;
    pEnumerator.Attach(new CEnumerator<IEnumModuleInfo, IModuleInfo>);
    if (pEnumerator == NULL)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRet(pEnumerator->Initialize(vecModules));

    *ppEnumModuleInfo = pEnumerator.Detach();

    return hr;
}