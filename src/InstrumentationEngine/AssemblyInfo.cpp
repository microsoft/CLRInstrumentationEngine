// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "AssemblyInfo.h"
// These have to go after AssemblyInfo.h because of cyclical dependencies...
#include "ProfilerManager.h"
#include "EnumeratorImpl.h"
#ifndef PLATFORM_UNIX
#include "StrongName.h"
#endif

MicrosoftInstrumentationEngine::CAssemblyInfo::CAssemblyInfo(_In_ CProfilerManager* pProfilerManager) :
    m_pProfilerManager(pProfilerManager),
    m_assemblyId(0),
    m_tkAssembly(mdTokenNil),
    m_manifestModuleID(0),
    m_pPublicKey(NULL),
    m_cbPublicKey(0),
    m_publicKeyTokenInitialized(false)
{
    DEFINE_REFCOUNT_NAME(CAssemblyInfo);
    InitializeCriticalSection(&m_cs);
}

MicrosoftInstrumentationEngine::CAssemblyInfo::~CAssemblyInfo()
{
    DeleteCriticalSection(&m_cs);
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::Initialize(
    _In_ AssemblyID assemblyId,
    _In_ const WCHAR* wszAssemblyName,
    _In_ IAppDomainInfo* pAppDomainInfo,
    _In_ ModuleID manifestModuleID
    )
{
    m_assemblyId = assemblyId;
    m_bstrAssemblyName = wszAssemblyName;
    m_pAppDomainInfo = pAppDomainInfo;
    m_manifestModuleID = manifestModuleID;

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetAppDomainInfo(_Out_ IAppDomainInfo** ppAppDomainInfo)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppAppDomainInfo);

    hr = m_pAppDomainInfo.CopyTo(ppAppDomainInfo);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::AddModuleInfo(_In_ CModuleInfo* pModuleInfo)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    ModuleID moduleId;
    IfFailRet(pModuleInfo->GetModuleID(&moduleId));
    m_moduleInfos[moduleId] = pModuleInfo;

    // If this is the manifest module, there is more work to be done
    if (moduleId == m_manifestModuleID)
    {
        m_pManifestModule = (IModuleInfo*)pModuleInfo;
        IfFailRet(HandleManifestModuleLoad());
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::HandleManifestModuleLoad()
{
    HRESULT hr = S_OK;

    if (m_pManifestModule == NULL)
    {
        CLogging::LogError(_T("Starting CAssemblyInfo::HandleManifestModuleLoad - No manifest module"));
        return E_FAIL;
    }

    CComPtr<IMetaDataAssemblyImport> pMetaDataAssemblyImport;
    IfFailRet(m_pManifestModule->GetMetaDataAssemblyImport((IUnknown**)&pMetaDataAssemblyImport));

    IfFailRet(pMetaDataAssemblyImport->GetAssemblyFromScope(&m_tkAssembly));
    IfFailRet(pMetaDataAssemblyImport->GetAssemblyProps(m_tkAssembly, &m_pPublicKey, &m_cbPublicKey, nullptr, nullptr, 0, nullptr, nullptr, nullptr));

    COR_PRF_RUNTIME_TYPE runtimeType;
    if (FAILED(m_pProfilerManager->GetRuntimeType(&runtimeType)) ||
        runtimeType != COR_PRF_CORE_CLR) // Don't attempt to calculate public key token in coreclr case.
    {
        IfFailRet(InitializePublicKeyToken());
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::ModuleInfoUnloaded(_In_ CModuleInfo* pModuleInfo)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    ModuleID moduleId;
    IfFailRet(pModuleInfo->GetModuleID(&moduleId));

    m_moduleInfos.erase(moduleId);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetModuleCount(_Out_ ULONG* pcModuleInfos)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pcModuleInfos);

    CCriticalSectionHolder lock(&m_cs);

    *pcModuleInfos = ((ULONG)m_moduleInfos.size());

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetModules(_In_ ULONG cModuleInfos, _Out_ IEnumModuleInfo** ppModuleInfos)
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

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetModuleById(_In_ ModuleID moduleId, _Out_ IModuleInfo** ppModuleInfo)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppModuleInfo);
    *ppModuleInfo = NULL;

    CCriticalSectionHolder lock(&m_cs);

    std::unordered_map<ModuleID, CComPtr<CModuleInfo>>::iterator iter = m_moduleInfos.find(moduleId);

    if (iter == m_moduleInfos.end())
    {
        CLogging::LogError(_T("CAssemblyInfo::GetModuleById - Failed to find specified assembly %08" PRIxPTR), moduleId);
        return E_FAIL;
    }

    *ppModuleInfo = iter->second;
    (*ppModuleInfo)->AddRef();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetModuleByMvid(_In_ GUID* pMvid, _Out_ IModuleInfo** ppModuleInfo)
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

        if (currMvid == *pMvid)
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

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetModulesByName(_In_ BSTR pszModuleName, _Out_ IEnumModuleInfo** ppEnumModuleInfo)
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

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetModuleForMethod(_In_ mdToken token, _Out_ IModuleInfo** ppModuleInfo)
{
    HRESULT hr = S_OK;
    *ppModuleInfo = nullptr;
    //VSFAIL(L"NYI");
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetModuleForType(_In_ BSTR pszTypeName, _In_ mdToken tkResolutionScope, _Out_ mdToken* pTkTypeDef)
{
    HRESULT hr = S_OK;
    *pTkTypeDef = mdTokenNil;
    //VSFAIL(L"NYI");
    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetManifestModule(_Out_ IModuleInfo** ppModuleInfo)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppModuleInfo);
    *ppModuleInfo = NULL;

    if (m_pManifestModule == NULL)
    {
        CLogging::LogMessage(_T("CAssemblyInfo::GetManifestModule - Why no manifest module?"));
        return E_FAIL;
    }

	IfFailRet(m_pManifestModule.CopyTo(ppModuleInfo));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetPublicKey(_In_ ULONG cbBytes, _Out_writes_(cbBytes) BYTE* pbBytes)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbBytes);
    if (cbBytes != m_cbPublicKey)
    {
        CLogging::LogError(_T("CAssemblyInfo::GetPublicKey - Incorrect public key buffer size"));
    }

    memcpy_s(pbBytes, cbBytes, m_pPublicKey, m_cbPublicKey);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetPublicKeySize(_Out_ ULONG* pcbBytes)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pcbBytes);

    *pcbBytes = m_cbPublicKey;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetPublicKeyToken(_Out_ BSTR* pbstrPublicKeyToken)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbstrPublicKeyToken);
    if (!m_publicKeyTokenInitialized)
    {
        CLogging::LogError(_T("Starting CAssemblyInfo::GetPublicKeyToken - public key token not initiliazed"));
        return E_FAIL;
    }

    IfFailRet(m_bstrPublicKeyToken.CopyTo(pbstrPublicKeyToken));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetID(_Out_ AssemblyID* pAssemblyId)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pAssemblyId);
    *pAssemblyId = m_assemblyId;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetName(_Out_ BSTR* pbstrName)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pbstrName);
    IfFailRet(m_bstrAssemblyName.CopyTo(pbstrName));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::GetMetaDataToken(_Out_ DWORD* pdwToken)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pdwToken);
    if (m_tkAssembly == mdTokenNil)
    {
        CLogging::LogMessage(_T("CAssemblyInfo::GetMetaDataToken - token is mdTokenNil."));
        return E_FAIL;
    }

    *pdwToken = m_tkAssembly;

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CAssemblyInfo::InitializePublicKeyToken()
{
    HRESULT hr = S_OK;

    // Strong name not supported on linux
#ifndef PLATFORM_UNIX
    if (m_publicKeyTokenInitialized)
    {
        return S_OK;
    }

    m_publicKeyTokenInitialized = true;

    // Check if there is a public key for this assembly
    if (!m_pPublicKey || !m_cbPublicKey)
    {
        CLogging::LogMessage(_T("CAssemblyInfo::InitializePublicKeyToken - No public key for assembly"));
        return S_FALSE;
    }

    // Regulators..... mount up
    BOOLEAN (__stdcall *StrongNameTokenFromPublicKeyPtr)(BYTE*, ULONG, BYTE**, ULONG*) = GetStrongNameTokenFromPublicKeyPtr();
    VOID (__stdcall *StrongNameFreeBufferPtr)(BYTE*) = GetStrongNameFreeBufferPtr();

    if (!StrongNameTokenFromPublicKeyPtr || !StrongNameFreeBufferPtr)
    {
        CLogging::LogError(_T("CAssemblyInfo::InitializePublicKeyToken - CLR exports not found"));
        return E_FAIL;
    }

    // Get the public key token from the public key
    BYTE* pPublicKeyToken = 0;
    ULONG publicKeyTokenSize = 0;
    if (!StrongNameTokenFromPublicKeyPtr(reinterpret_cast<BYTE*>(const_cast<void*>(m_pPublicKey)), m_cbPublicKey, &pPublicKeyToken, &publicKeyTokenSize))
    {
        CLogging::LogError(_T("CAssemblyInfo::InitializePublicKeyToken - Failed to get public key token"));
        return E_FAIL;
    }

    // Build up a string representation in uppercase hex for the public key token
    const DWORD dwPublicKeyTokenBufferLen = 16 + 1;
    WCHAR wszPublicKeyToken[dwPublicKeyTokenBufferLen] = { 0 };
    WCHAR *currTokenPos = wszPublicKeyToken;
    const WCHAR *endTokenBuffer = wszPublicKeyToken + 16; // Ignore last element since it is for null not content.
    for (ULONG i = 0; i < publicKeyTokenSize && (currTokenPos != endTokenBuffer); ++i)
    {
        // 2 for each printed character in a byte and 1 for the implicit null
        swprintf_s(currTokenPos, (2 + 1), L"%02X", pPublicKeyToken[i]);
        currTokenPos += 2;
    }

    // Free the buffer that was allocated by StrongNameTokenFromPublicKey
    StrongNameFreeBufferPtr(pPublicKeyToken);

    m_bstrPublicKeyToken = wszPublicKeyToken;

#endif

    return S_OK;
}