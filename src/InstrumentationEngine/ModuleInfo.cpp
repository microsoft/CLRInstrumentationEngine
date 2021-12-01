// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "ModuleInfo.h"
// These have to go after ModuleInfo.h because of cyclical dependencies...
#include "ProfilerManager.h"
#include "TypeCreator.h"
#include "MethodJitInfo.h"

#ifdef _WINDOWS_
#include "AssemblyInjector.h"
#endif

MicrosoftInstrumentationEngine::CModuleInfo::CModuleInfo(_In_ CProfilerManager* pProfilerManager) :
    m_pProfilerManager(pProfilerManager),
    m_moduleID(0),
    m_bIs64bit(false),
    m_bIsIlOnly(true),
    m_bIsMscorlib(false),
    m_bIsDynamic(false),
    m_bIsLoadedFromDisk(false),
    m_bIsNgen(false),
    m_bIsWinRT(false),
    m_bIsFlatLayout(false),
    m_pModuleBaseLoadAddress(0),
    m_pbSectionStart(nullptr),
    m_numSections(0),
    m_tkEntrypoint(mdTokenNil)
{
    DEFINE_REFCOUNT_NAME(CModuleInfo);
    InitializeCriticalSection(&m_cs);
}

MicrosoftInstrumentationEngine::CModuleInfo::~CModuleInfo()
{
    DeleteCriticalSection(&m_cs);
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::Initialize(
    _In_ ModuleID moduleID,
    _In_ const WCHAR* wszModuleName,
    _In_ IAssemblyInfo* pAssemblyInfo,
    _In_ IAppDomainInfo* pAppDomainInfo,
    _In_ LPCBYTE pModuleBaseLoadAddress,
    _In_ IMetaDataImport2* pMetadataImport,
    _In_ IMetaDataAssemblyImport* pMetadataAssemblyImport,
    _In_ IMetaDataEmit2* pMetaDataEmit2,
    _In_ IMetaDataAssemblyEmit* pMetaDataAssemblyEmit
    )
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pAssemblyInfo);
    IfNullRetPointer(pAppDomainInfo);

    IfNullRetPointer(pMetadataImport);
    IfNullRetPointer(pMetadataAssemblyImport);

    // NOTE: Winmds will not have metadata emit or assembly emit.
    // Do not Fail if they are null

    m_moduleID = moduleID;
    m_bstrModulePath = wszModuleName;
    m_bstrModuleName = PathFindFileName(m_bstrModulePath);
    m_pAssemblyInfo = pAssemblyInfo;
    m_pAppDomainInfo = pAppDomainInfo;
    m_pModuleBaseLoadAddress = pModuleBaseLoadAddress;

    m_pMetadataImport = pMetadataImport;
    m_pMetadataAssemblyImport = pMetadataAssemblyImport;
    m_pMetaDataEmit2 = pMetaDataEmit2;
    m_pMetaDataAssemblyEmit = pMetaDataAssemblyEmit;

    IfFailRet(m_pMetadataImport->GetScopeProps(NULL, 0, NULL, &m_mvid));

    IfFailRet(GetModuleTypeFlags());

    if (!m_bIsDynamic)
    {
        // Note that this must be called _after_ GetModuleTypeFlags so that we know whether
        // module is loaded flat or mapped.
        IfFailRet(ReadModuleHeaders());

        // check if there is a chance of any native code in the assembly
        if (!m_pCorHeader || IsFlagSet(m_pCorHeader->Flags, COMIMAGE_FLAGS_ILONLY))
        {
            m_bIsIlOnly = false;
        }

        IfFailRet(DetermineIfIsMscorlib());

        IfFailRet(ReadFixedFileVersion());
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::Dispose()
{
    m_pTypeFactory.Release();
    m_pAssemblyInfo.Release();
    m_pAppDomainInfo.Release();

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMethodInfoById(_In_ FunctionID functionId, CMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppMethodInfo);
    *ppMethodInfo = nullptr;

    std::unordered_map<FunctionID, CComPtr<CMethodInfo>>::iterator iter = m_methodInfos.find(functionId);

    if (iter == m_methodInfos.end())
    {
        CLogging::LogMessage(_T("CModuleInfo::GetMethodInfoById - Failed to find specified method %08" PRIxPTR), functionId);
        return E_FAIL;
    }

    *ppMethodInfo = (iter->second);
    (*ppMethodInfo)->AddRef();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMethodInfoByToken(_In_ mdToken methodToken, CMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    IfNullRetPointer(ppMethodInfo);
    *ppMethodInfo = nullptr;

    std::unordered_map<mdMethodDef, CComPtr<CMethodInfo>>::iterator iter = m_methodInfosByToken.find(methodToken);

    if (iter == m_methodInfosByToken.end())
    {
        CLogging::LogMessage(_T("CModuleInfo::GetMethodInfoByToken - Failed to find specified method %04x"), methodToken);
        return E_FAIL;
    }

    *ppMethodInfo = (iter->second);
    (*ppMethodInfo)->AddRef();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::AddMethodInfo(_In_ FunctionID functionId, CMethodInfo* pMethodInfo)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    mdToken methodToken;
    IfFailRet(pMethodInfo->GetMethodToken(&methodToken));

    m_methodInfos[functionId] = pMethodInfo;
    m_methodInfosByToken[methodToken] = pMethodInfo;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::ReleaseMethodInfo(_In_ FunctionID functionId)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    CComPtr<CMethodInfo> pMethodInfo;
    pMethodInfo = m_methodInfos[functionId];
    if (pMethodInfo == NULL)
    {
        CLogging::LogError(_T("CModuleInfo::ReleaseMethodInfo - Methodinfo not found"));
        return E_FAIL;
    }

    mdToken methodToken;
    IfFailRet(pMethodInfo->GetMethodToken(&methodToken));

    m_methodInfos.erase(functionId);
    m_methodInfosByToken.erase(methodToken);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetModuleName(_Out_ BSTR* pbstrModuleName)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbstrModuleName);
    hr = m_bstrModuleName.CopyTo(pbstrModuleName);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetFullPath(_Out_ BSTR* pbstrFullPath)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbstrFullPath);

    hr = m_bstrModulePath.CopyTo(pbstrFullPath);

    return hr;
}

// Get info about the assembly that contains this module
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetAssemblyInfo(_Out_ IAssemblyInfo** ppAssemblyInfo)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppAssemblyInfo);
    hr = m_pAssemblyInfo.CopyTo(ppAssemblyInfo);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetAppDomainInfo(_Out_ IAppDomainInfo** ppAppDomainInfo)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppAppDomainInfo);
    hr = m_pAppDomainInfo.CopyTo(ppAppDomainInfo);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMetaDataImport(_Out_ IUnknown** ppMetaDataImport)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppMetaDataImport);

    hr = m_pMetadataImport.CopyTo((IMetaDataImport2**)ppMetaDataImport);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMetaDataAssemblyImport(_Out_ IUnknown** ppMetadataAssemblyImport)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppMetadataAssemblyImport);

    hr = m_pMetadataAssemblyImport.CopyTo((IMetaDataAssemblyImport**)ppMetadataAssemblyImport);

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMetaDataEmit(_Out_ IUnknown** ppMetaDataEmit)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppMetaDataEmit);
    *ppMetaDataEmit = NULL;

    if (m_pMetaDataEmit2 != NULL)
    {
        hr = m_pMetaDataEmit2.CopyTo((IMetaDataEmit2**)ppMetaDataEmit);
        return hr;
    }
    else
    {
        CLogging::LogMessage(_T("CModuleInfo::GetMetadataEmit - no IMetaDataEmit exists"));
        return E_FAIL;
    }
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMetaDataAssemblyEmit(_Out_ IUnknown** ppMetaDataAssemblyEmit)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppMetaDataAssemblyEmit);
    *ppMetaDataAssemblyEmit = NULL;

    if (m_pMetaDataAssemblyEmit != NULL)
    {
        hr = m_pMetaDataAssemblyEmit.CopyTo((IMetaDataAssemblyEmit**)ppMetaDataAssemblyEmit);
        return hr;
    }
    else
    {
        CLogging::LogMessage(_T("CModuleInfo::GetMetaDataAssemblyEmit - no IMetaDataAssemblyEmit exists"));
        return E_FAIL;
    }
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetModuleID(_Out_ ModuleID* pModuleId)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pModuleId);

    *pModuleId = m_moduleID;

    return hr;
}

// Get the managed module version id
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMVID(_Out_ GUID* pguidMvid)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pguidMvid);
    *pguidMvid = m_mvid;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetIsILOnly(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    *pbValue = m_bIsIlOnly;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetIsMscorlib(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    *pbValue = m_bIsMscorlib;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetIsDynamic(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    *pbValue = m_bIsDynamic;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetIsLoadedFromDisk(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    *pbValue = m_bIsLoadedFromDisk;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetIsNgen(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    *pbValue = m_bIsNgen;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetIsWinRT(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    *pbValue = m_bIsWinRT;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetIs64bit(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    *pbValue = m_bIs64bit;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetImageBase(_Out_ LPCBYTE* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    *pbValue = m_pModuleBaseLoadAddress;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetCorHeader(_In_ DWORD cbValue, _Out_writes_(cbValue) BYTE* pbValue)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pbValue);

    if (m_pCorHeader == NULL)
    {
        CLogging::LogMessage(_T("Module has no cor header"));
        return E_FAIL;
    }

    memcpy_s(pbValue, cbValue, m_pCorHeader, cbValue);

    return hr;
}

// Returns the modules entrypoint token. If the module has no entrypoint token, returns E_FAIL;
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetEntrypointToken(_Out_ DWORD* pdwEntrypointToken)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pdwEntrypointToken);
    *pdwEntrypointToken = mdTokenNil;

    if (m_tkEntrypoint != mdTokenNil)
    {
        *pdwEntrypointToken = m_tkEntrypoint;
    }
    else
    {
        return E_FAIL;
    }

    return hr;
}

// Returns the VS_FIXEDFILEINFO for this module.
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetModuleVersion(_In_ DWORD cbValue, _Out_writes_(cbValue) BYTE* pbValue)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pbValue);

    if (cbValue != sizeof(VS_FIXEDFILEINFO))
    {
        CLogging::LogError(_T("CModuleInfo::GetModuleVersion - bad buffer size. Buffer should be sizeof(VS_FIXEDFILEINFO)"));
        return E_INVALIDARG;
    }
    if (m_pFixedFileVersion == NULL)
    {
        CLogging::LogMessage(_T("CModuleInfo::GetModuleVersion - No file version available for this module."));
        return E_FAIL;
    }

    memcpy_s(pbValue, cbValue, m_pFixedFileVersion, sizeof(VS_FIXEDFILEINFO));

    return hr;
}

// Request a rejit on a method
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::RequestRejit(_In_ mdToken methodToken)
{
    HRESULT hr = S_OK;

    CComPtr<ICorProfilerInfo> pRealProfilerInfo;
    IfFailRet(m_pProfilerManager->GetRealCorProfilerInfo(&pRealProfilerInfo));

    CComPtr<ICorProfilerInfo4> pRealProfilerInfo4;
    IfFailRet(pRealProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo4), (void**)&pRealProfilerInfo4));

    vector<ModuleID> moduleIds;
    vector<mdToken> methodTokens;

    moduleIds.emplace_back(m_moduleID);
    methodTokens.emplace_back(methodToken);

    // NOTE: with inlining enabled, must rejit all inline sites recursively. Otherwise, if function A is inlined
    // into function B which is itself inlined into Function C, the rejit or A will have no effect on that inline.
    // So, request rejit on all sites. The instrumentation methods will be responsible for actually disabling inlining
    // for the functions they care about in the "allow inline" callback. Note that for the rejit for the parent
    // frames, it is perfectly acceptable to inline again, just not with the methods the instrumentation methods are
    // actually inlining.
    hr = AddInlineSitesForRejit(methodToken, moduleIds, methodTokens);

    if (SUCCEEDED(hr) && moduleIds.size() != methodTokens.size())
    {
        CLogging::LogError(_T("CModuleInfo::RequestRejit - vector sizes for inline sites do not match"));
        return E_FAIL;
    }

    IfFailRet(pRealProfilerInfo4->RequestReJIT((ULONG)moduleIds.size(), moduleIds.data(), methodTokens.data()));

    return S_OK;
}

// Append any inline site module/method tokens to the vector of existing methods to rejit. This will cause
// them to rejit as well.
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::AddInlineSitesForRejit(_In_ mdToken methodToken, _In_ vector<ModuleID>& moduleIds, _In_ vector<mdToken>& methodTokens)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder holder(&m_cs);

    if (m_pInlineSiteMap != nullptr)
    {
        m_pInlineSiteMap->FindInlineSites(methodToken, moduleIds, methodTokens);
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetModuleTypeFlags()
{
    HRESULT hr = S_OK;

    // A module is considered dynamic if and only if the base load address is 0
    m_bIsDynamic = m_pModuleBaseLoadAddress == 0;

    if(m_pProfilerManager->GetAttachedClrVersion() < ClrVersion_4 )
    {
#ifndef PLATFORM_UNIX
        if (m_bstrModuleName.Length() != 0)
        {
            //HACK: Figure out whether this is Ngen module by looking for <modulename>.ni.dll in the raw module name.
            tstring moduleName = m_bstrModuleName;
            const WCHAR* pExtension = PathFindExtension(moduleName.c_str());

            tstring modifiedModuleName = moduleName.substr(pExtension - moduleName.c_str());
            modifiedModuleName.append(L".ni.dll");

            HMODULE hNgen = GetModuleHandle(modifiedModuleName.c_str());
            m_bIsNgen = (hNgen != NULL);
        }
#endif
        m_bIsLoadedFromDisk = !m_bIsDynamic && (m_bstrModulePath.Length() != 0) && (m_bstrModuleName.Length() != 0);
    }
    else
    {
        CComPtr<ICorProfilerInfo> pCorProfilerInfo;
        IfFailRet(m_pProfilerManager->GetRealCorProfilerInfo(&pCorProfilerInfo));

        //In CLRv4, rely on GetModuleInfo2 to find out the type of the module.
        CComPtr<ICorProfilerInfo3> pCorInfo3;
        IfFailRet(pCorProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo3), (LPVOID*)&pCorInfo3));

        DWORD dwModuleFlags = 0;
        IfFailRet(pCorInfo3->GetModuleInfo2(m_moduleID, nullptr, 0, nullptr, nullptr, nullptr, &dwModuleFlags));

        m_bIsLoadedFromDisk = IsFlagSet(dwModuleFlags, COR_PRF_MODULE_DISK);

        m_bIsNgen = IsFlagSet(dwModuleFlags, COR_PRF_MODULE_NGEN);
        m_bIsWinRT = IsFlagSet(dwModuleFlags, COR_PRF_MODULE_WINDOWS_RUNTIME);
        m_bIsFlatLayout = IsFlagSet(dwModuleFlags, COR_PRF_MODULE_FLAT_LAYOUT);
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::ReadModuleHeaders()
{
    HRESULT hr = S_OK;

    if (m_pModuleBaseLoadAddress == NULL)
    {
        CLogging::LogError(_T("CModuleInfo::ReadModuleHeaders - No load address"));
        return E_FAIL;
    }

    IMAGE_DOS_HEADER const* pDOSHeader = reinterpret_cast<IMAGE_DOS_HEADER const*>(m_pModuleBaseLoadAddress);

    // Check for DOS signature
    if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        CLogging::LogError(_T("CModuleInfo::ReadModuleHeaders - Incorrect dos header magic number"));
        return E_FAIL;
    }

    // Check for NT signature (signature is at the same offset for both 32-bit and 64-bit PEs)
    IMAGE_NT_HEADERS32 const* pNTHeader = reinterpret_cast<IMAGE_NT_HEADERS32 const*>(m_pModuleBaseLoadAddress + pDOSHeader->e_lfanew);
    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        CLogging::LogError(_T("CModuleInfo::ReadModuleHeaders - Incorrect NT Signature"));
        return E_FAIL;
    }

    // The magic field is the same offset for both 32-bit and 64-bit PEs
    m_bIs64bit = pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;

    // The .NET header is stored in the optional header's data directory, so we need to use the right one
    IMAGE_DATA_DIRECTORY const* pDirectory = 0;
    if (m_bIs64bit)
    {
        pDirectory = &reinterpret_cast<IMAGE_NT_HEADERS64 const*>(pNTHeader)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER];
        m_pbSectionStart = (LPCBYTE)&(pNTHeader->OptionalHeader) + sizeof(IMAGE_OPTIONAL_HEADER64);
    }
    else
    {
        pDirectory = &pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER];
        m_pbSectionStart = (LPCBYTE)&(pNTHeader->OptionalHeader) + sizeof(IMAGE_OPTIONAL_HEADER32);
    }

    m_numSections = pNTHeader->FileHeader.NumberOfSections;

    // Get the COR header
    IMAGE_COR20_HEADER const* pCLRHeader = 0;
    if (pDirectory->Size > 0 && pDirectory->VirtualAddress)
    {
        LPCBYTE pbCLRHeader;
        IfFailRet(ResolveRva(pDirectory->VirtualAddress, &pbCLRHeader));
        pCLRHeader = reinterpret_cast<IMAGE_COR20_HEADER const*>(pbCLRHeader);
    }
    else
    {
        CLogging::LogError(_T("CModuleInfo::ReadModuleHeaders - No CLR header. Why did the CLR send this module?"));
        return E_FAIL;
    }

    m_pCorHeader = pCLRHeader;
    m_tkEntrypoint = m_pCorHeader->EntryPointToken;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::ResolveRva(_In_ DWORD rva, _Out_ LPCBYTE* ppbResolvedAddress)
{
    HRESULT hr = S_OK;

    IfNullRet(ppbResolvedAddress);
    *ppbResolvedAddress = nullptr;

    if (m_pModuleBaseLoadAddress == 0)
    {
        return E_NOTIMPL;
    }

    IfNullRet(m_pbSectionStart);

    if (m_bIsFlatLayout)
    {

        // Module was loaded using "Flat" layout (aka. on-disk format).

        if (m_pModuleBaseLoadAddress + rva < m_pbSectionStart)
        {
            // The RVA belongs to one of the image headers, not to one of the sections, so the
            // RVA and the flat offset are the same.
            *ppbResolvedAddress = m_pModuleBaseLoadAddress + rva;
            return S_OK;
        }

        // RVA is inside a section.
        const IMAGE_SECTION_HEADER* sections = reinterpret_cast<const IMAGE_SECTION_HEADER*>(m_pbSectionStart);
        for (int i = 0; i < m_numSections; i++)
        {
            IMAGE_SECTION_HEADER section = sections[i];
            if (section.VirtualAddress <= rva && rva < section.VirtualAddress + section.SizeOfRawData)
            {
                // Beginning of section is at offset "PointerToRawData" from pbImage, after
                // that point the section is laid out contiguously so work out how far RVA is
                // from beginning of section.
                *ppbResolvedAddress = m_pModuleBaseLoadAddress + section.PointerToRawData + (rva - section.VirtualAddress);
                return S_OK;
            }
        }

        return E_POINTER;
    }

    // Module was loaded using "Mapped" layout (aka. in-memory format) so we can treat all
    // virtual address read from headers as offsets from pbImage.
    *ppbResolvedAddress = m_pModuleBaseLoadAddress + rva;
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::DetermineIfIsMscorlib()
{
    HRESULT hr = S_OK;

    if (m_bstrModuleName.Length() > 0)
    {
#ifndef PLATFORM_UNIX
        if (wcscmp(m_bstrModuleName, _T("mscorlib.dll")) == 0)
#else
        if (PAL_wcscmp(m_bstrModuleName, _T("mscorlib.dll")) == 0)
#endif
        {
            m_bIsMscorlib = true;
        }
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::ReadFixedFileVersion()
{
    HRESULT hr = S_OK;

#ifndef PLATFORM_UNIX
    // TODO: (linux)

    if (m_bIsLoadedFromDisk)
    {
        DWORD unused;
        DWORD dwAllocSize = GetFileVersionInfoSize(m_bstrModulePath, &unused);
        if(dwAllocSize != 0)
        {
            CAutoVectorPtr<BYTE> pBuffer(new BYTE[dwAllocSize]);

            if (pBuffer)
            {
                if (GetFileVersionInfo(m_bstrModulePath, 0, dwAllocSize, (void*)(pBuffer.m_p)))
                {
                    VS_FIXEDFILEINFO* pFileInfo = NULL;
                    UINT len;

                    if (VerQueryValue((void*)(pBuffer.m_p), L"\\", reinterpret_cast<LPVOID *>(&pFileInfo), &len))
                    {
                        m_pFixedFileVersion.Attach(new VS_FIXEDFILEINFO());
                        memcpy_s(m_pFixedFileVersion, sizeof(VS_FIXEDFILEINFO), pFileInfo, sizeof(VS_FIXEDFILEINFO));
                    }
                }
            }
        }
    }
    else
    {
        CLogging::LogMessage(_T("CModuleInfo::ReadFixedFileVersion - Skipping fixed file version due to in-memory module"));
    }

#endif
    return S_OK;
}

// The profiler apis have issues around rejit request revert and generics
// If a function is modified by the profiler, GetILFunctionBody may return
// the modified IL (example: rejit before original jit).
// So, if any consumer requests the il function body, the original il is cached
// and resused in requests that come later.
//
// NOTE: this is unfortunate as this is yet another copy of the il.
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMethodIl(
    _In_ ICorProfilerInfo* pCorProfilerInfo,
    _In_ mdToken tkFunction,
    _Out_ IMAGE_COR_ILMETHOD** ppMethodHeader,
    _Out_ ULONG* pcbMethodSize
    )
{
    HRESULT hr = S_OK;
    CCriticalSectionHolder lock(&m_cs);

    std::unordered_map<mdToken, std::shared_ptr<CCachedILMethodBody>>::iterator iter = m_methodTokenToCachedILMap.find(tkFunction);

    if (iter == m_methodTokenToCachedILMap.end())
    {
        IfFailRet(pCorProfilerInfo->GetILFunctionBody(m_moduleID, tkFunction, (LPCBYTE*)ppMethodHeader, pcbMethodSize));

        std::shared_ptr<CCachedILMethodBody> pCachedMethodBody = std::shared_ptr<CCachedILMethodBody>(new CCachedILMethodBody(tkFunction, *ppMethodHeader, *pcbMethodSize));

        m_methodTokenToCachedILMap[tkFunction] = pCachedMethodBody;
    }
    else
    {
        std::shared_ptr<CCachedILMethodBody> pCachedMethodBody = iter->second;
        IfFailRet(pCachedMethodBody->GetOriginalMethodBody(ppMethodHeader, pcbMethodSize));
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::CreateTypeFactory(_Out_ ITypeCreator** ppTypeFactory)
{
    IfNullRetPointer(ppTypeFactory);

    if (!m_pTypeFactory)
    {
        m_pTypeFactory.Attach(new CTypeCreator(this));
        if (m_pTypeFactory == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    return m_pTypeFactory->QueryInterface<ITypeCreator>(ppTypeFactory);
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::IncrementMethodRejitCount(_In_ mdToken token)
{
    HRESULT hr = S_OK;

    CCriticalSectionHolder lock(&m_cs);

    if (m_methodTokenToRejitCountMap.find(token) == m_methodTokenToRejitCountMap.end())
    {
        m_methodTokenToRejitCountMap[token] = 1;
    }
    else
    {
        m_methodTokenToRejitCountMap[token]++;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMethodRejitCount(_In_ mdToken token, _Out_ DWORD* pdwRejitCount)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pdwRejitCount);
    *pdwRejitCount = 0;

    CCriticalSectionHolder lock(&m_cs);

    unordered_map<mdToken, UINT32>::iterator iter = m_methodTokenToRejitCountMap.find(token);
    if (iter != m_methodTokenToRejitCountMap.end())
    {
        *pdwRejitCount = iter->second;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::SetRejitMethodInfo(_In_ mdToken methodToken, _In_opt_ CMethodInfo* pMethodInfo)
{
    CCriticalSectionHolder lock(&m_cs);

    if (pMethodInfo != nullptr)
    {
        std::unordered_map<mdMethodDef, CComPtr<CMethodInfo>>::iterator iter = m_methodInfosByToken.find(methodToken);
        if (iter != m_methodInfosByToken.end())
        {
            CLogging::LogMessage(_T("CModuleInfo::SetRejitMethodInfo - method token already exists %04x. Overwriting"), methodToken);
        }
        else
        {
            CLogging::LogMessage(_T("CModuleInfo::SetRejitMethodInfo - adding new method token %04x."), methodToken);
        }

        m_methodInfosByToken[methodToken] = pMethodInfo;
    }
    else
    {
        CLogging::LogMessage(_T("CModuleInfo::SetRejitMethodInfo - erasing existing method token %04x."), methodToken);
        m_methodInfosByToken.erase(methodToken);
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetInlineSiteMap(_Out_ CInlineSiteMap** ppInilineSiteMap)
{
    HRESULT hr = S_OK;
    *ppInilineSiteMap = nullptr;

    CCriticalSectionHolder lock(&m_cs);

    if (m_pInlineSiteMap == nullptr)
    {
        m_pInlineSiteMap.Attach(new CInlineSiteMap);
        if (m_pInlineSiteMap == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    hr = m_pInlineSiteMap.CopyTo(ppInilineSiteMap);

    return S_OK;
}

// Allow instrumentation methods to receive an instance of IMethodInfo for a FunctionID.
// This method info is only for obtaining information about the method and cannot be used
// for instrumenation.
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMethodInfoById(_In_ FunctionID functionID, _Out_ IMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppMethodInfo);
    *ppMethodInfo = NULL;

    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    IfFailRet(m_pProfilerManager->GetRealCorProfilerInfo(&pCorProfilerInfo));

    ClassID classId;
    ModuleID moduleId;
    mdToken methodToken;
    IfFailRet(pCorProfilerInfo->GetFunctionInfo(functionID, &classId, &moduleId, &methodToken));

    CLogging::LogMessage(_T("CProfilerManager::GetMethodInfoById - creating new method info"));
    CComPtr<CMethodInfo> pMethodInfo;
    pMethodInfo.Attach(new CMethodInfo(m_pProfilerManager, functionID, methodToken, classId, this, nullptr));

    IfFailRet(pMethodInfo->Initialize(false, false));

    *ppMethodInfo = pMethodInfo.Detach();

    return S_OK;
}

// Allow instrumentation methods to receive an instance of IMethodInfo for a token.
// This method info is only for obtaining information about the method and cannot be used
// for instrumenation.
HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetMethodInfoByToken(_In_ mdToken methodToken, _Out_ IMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppMethodInfo);
    *ppMethodInfo = NULL;

    CComPtr<CMethodInfo> pMethodInfo;
    pMethodInfo.Attach(new CMethodInfo(m_pProfilerManager, 0, methodToken, 0, this, nullptr));

    IfFailRet(pMethodInfo->Initialize(false, false));

    *ppMethodInfo = pMethodInfo.Detach();

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::ImportModule(_In_ IUnknown* pSourceModule, _In_ LPCBYTE* pSourceImage)
{
#ifdef _WINDOWS_
    HRESULT hr = S_OK;

    CComPtr<ICorProfilerInfo> pRealProfilerInfo;
    IfFailRet(m_pProfilerManager->GetRealCorProfilerInfo(&pRealProfilerInfo));

    CComPtr<ICorProfilerInfo2> pRealProfilerInfo2;
    IfFailRet(pRealProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo2), reinterpret_cast<void**>(&pRealProfilerInfo2)));

    CComPtr<IMethodMalloc> pTargetMethodMalloc;
    IfFailRet(pRealProfilerInfo2->GetILFunctionBodyAllocator(m_moduleID, &pTargetMethodMalloc));

    AssemblyInjector ai(pRealProfilerInfo2, reinterpret_cast <IMetaDataImport2*>(pSourceModule), pSourceImage, m_pMetadataImport, m_pMetaDataEmit2, m_moduleID, pTargetMethodMalloc);
    IfFailRet(ai.ImportAll());

    return S_OK;
#else
    return E_NOTIMPL;
#endif
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetIsFlatLayout(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    IfNullRet(pbValue);

    *pbValue = m_bIsFlatLayout;

    return S_OK;
}

void MicrosoftInstrumentationEngine::CModuleInfo::SetMethodIsTransformed(_In_ mdMethodDef methodDef, bool isInstrumented)
{
    if (isInstrumented)
    {
        m_instrumentedMethods.emplace(methodDef);
    }
    else
    {
        m_instrumentedMethods.erase(methodDef);
    }
}

void MicrosoftInstrumentationEngine::CModuleInfo::SetILInstrumentationMap(_In_ CMethodInfo* pMethodInfo, _In_ CSharedArray<COR_IL_MAP> pMap)
{
    FunctionID functionId = 0;
    mdMethodDef methodDef = mdTokenNil;
    if (!pMethodInfo->IsRejit())
    {
        pMethodInfo->GetFunctionId(&functionId);
    }

    pMethodInfo->GetMethodToken(&methodDef);
    m_ilMaps[CMethodKey(methodDef, functionId)] = pMap;
}

HRESULT MicrosoftInstrumentationEngine::CModuleInfo::GetILInstrumentationMap(_In_ CMethodJitInfo* pMethodJitInfo, _In_ ULONG32 cMap, _Out_writes_opt_(cMap) COR_IL_MAP* pMap, _Out_ ULONG32* pcNeeded)
{
    IfNullRet(pcNeeded);
    HRESULT hr;

    *pcNeeded = 0;

    FunctionID functionId = 0;
    mdMethodDef methodDef = mdTokenNil;
    BOOL isRejit = false;

    IfFailRet(pMethodJitInfo->GetMethodToken(&methodDef));
    IfFailRet(pMethodJitInfo->GetIsRejit(&isRejit));

    if (!isRejit)
    {
        IfFailRet(pMethodJitInfo->GetFunctionID(&functionId));
    }

    auto it = m_ilMaps.find(CMethodKey(methodDef, functionId));
    if (it != m_ilMaps.end())
    {
        CSharedArray<COR_IL_MAP> map = (*it).second;

        // This is a safe cast since the underlying CLR APIs only use a ULONG32 anyway.
        *pcNeeded = (ULONG32)map.Count();
        if (cMap == 0 || pMap == NULL)
        {
            return S_OK;
        }
        size_t mapCount = map.Count();
        size_t cCount = ((size_t)cMap <= mapCount) ? (size_t)cMap : mapCount;
        IfFailRetErrno(memcpy_s(pMap, mapCount * sizeof(COR_IL_MAP), map.Get(), cCount * sizeof(COR_IL_MAP)));
    }

    return S_OK;
}

bool MicrosoftInstrumentationEngine::CModuleInfo::GetIsMethodInstrumented(_In_ mdMethodDef methodDef)
{
    return (m_instrumentedMethods.find(methodDef) != m_instrumentedMethods.end());
}
