// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"

#include "HostExtensionBase.h"
#include "CallbacksEmitter.h"
#include "ExtensionsCommon\DataItemContainer.h"
#include "ModuleMetaDataLoader.h"
#include "ExtensionsCommon/WellKnownInstrumentationDefs.h"
#include "ExtensionsCommon\NativeInstanceMethodInstrumentationInfo.h"

using Agent::Diagnostics::Param;

static const GUID GUID_AppDomainIMList = { /* 2049cb51-2558-450d-8edf-84887cf073b2 */
    0x2049cb51,
    0x2558,
    0x450d,
    { 0x8e, 0xdf, 0x84, 0x88, 0x7c, 0xf0, 0x73, 0xb2 }
};

HRESULT STDMETHODCALLTYPE AttachStub(
    _In_ CHostExtensionBase* pThis,
    _In_ const __int64 modulePathPtr,
    _In_ const __int64 modulePtr,
    _In_ const __int64 classGuidPtr,
    _In_ const __int32 priority)
{
    return pThis->Attach(modulePathPtr, modulePtr, classGuidPtr, priority);
}

HRESULT STDMETHODCALLTYPE IsAttachedStub(
	_In_ CHostExtensionBase* /*thisObj*/)
{
    return S_OK;
}

_Success_(return == 0)
HRESULT STDMETHODCALLTYPE GetAgentVersionStub(
    _In_ CHostExtensionBase* /*pThis*/,
    _In_ const unsigned __int32 cnBufferSize,
	_In_ const __int64 bufferPtr) // <-- this actually a pointer where we will be writing to. Maybe we should make it a pointer
{
    std::wstring strAgentVersion = L"2.0.0";

    wchar_t* pBuffer = reinterpret_cast<wchar_t*>(bufferPtr);
    wcscpy_s(pBuffer, cnBufferSize, strAgentVersion.c_str());

    return S_OK;
}

CHostExtensionBase::CHostExtensionBase()
{
}

CHostExtensionBase::~CHostExtensionBase()
{
}

_Success_(return == 0)
HRESULT CHostExtensionBase::GetModulePath(_Out_ std::wstring& modulePath) const
{
    HRESULT hr = S_OK;

    HMODULE hModule = NULL;
    IfFalseRet(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, const_cast<LPWSTR>(ExtensionsBaseOriginalModuleName), &hModule), HRESULT_FROM_WIN32(::GetLastError()));
    TCHAR szPath[MAX_PATH + 1] = { 0 };
    DWORD dwLen = ::GetModuleFileName(hModule, szPath, MAX_PATH);
    IfFalseRet(dwLen != 0, HRESULT_FROM_WIN32(::GetLastError()));

    std::wstring fullName(szPath);
    std::wstring fileName(PathFindFileName(szPath));

    modulePath = std::wstring(szPath, 0, fullName.size() - fileName.size());

    return hr;
}


HRESULT CHostExtensionBase::InternalInitialize(
    _In_ const IProfilerManagerSptr& spHost)
{
    auto hr = S_OK;

    Agent::Instrumentation::CMethodInstrumentationInfoCollectionSptr spInteropMethods;
    IfFailRet(Create(spInteropMethods));

    Agent::Instrumentation::Native::CNativeInstanceMethodInstrumentationInfoSptr spInteropMethod;

    // For all AppInsights extensions, turn ngen off.
    IUnknownSptr spIUnknown;
    IfFailRet(spHost->GetCorProfilerInfo(&spIUnknown));
    ICorProfilerInfoQiSptr spICorProfilerInfo = spIUnknown;
    IfFailRet(spICorProfilerInfo->SetEventMask(COR_PRF_DISABLE_ALL_NGEN_IMAGES));

    IfFailRet(
        Create(
        spInteropMethod,
        Agent::Interop::WildcardName,
        Agent::Interop::WildcardName,
        L"Microsoft.Diagnostics.Instrumentation.Extensions.Base.NativeMethods.Attach",
        4,
        0,
        reinterpret_cast<UINT_PTR>(this),
        reinterpret_cast<UINT_PTR>(&AttachStub)));
    IfFailRet(spInteropMethods->Add(spInteropMethod));

    IfFailRet(
        Create(
        spInteropMethod,
        Agent::Interop::WildcardName,
        Agent::Interop::WildcardName,
        L"Microsoft.Diagnostics.Instrumentation.Extensions.Base.NativeMethods.IsAttached",
        0,
        0,
        reinterpret_cast<UINT_PTR>(this),
        reinterpret_cast<UINT_PTR>(&IsAttachedStub)));
    IfFailRet(spInteropMethods->Add(spInteropMethod));

    IfFailRet(
        Create(
        spInteropMethod,
        Agent::Interop::WildcardName,
        Agent::Interop::WildcardName,
        L"Microsoft.Diagnostics.Instrumentation.Extensions.Base.NativeMethods.GetAgentVersion",
        2,
        0,
        reinterpret_cast<UINT_PTR>(this),
        reinterpret_cast<UINT_PTR>(&GetAgentVersionStub)));
    IfFailRet(spInteropMethods->Add(spInteropMethod));

    IfFailRet(CreateAndBuildUp(m_spInteropHandler, spInteropMethods));
    CDecorationCallbacksInfoStaticReaderSptr spDecorationCallbacksInfoReader;
    IfFailRet(CreateAndBuildUp(spDecorationCallbacksInfoReader));

    m_spHost = spHost;
    m_spDecorationCallbacksInfoReader = spDecorationCallbacksInfoReader;

    return hr;
}

_Success_(return == 0)
HRESULT CHostExtensionBase::InternalShouldInstrumentMethod(
    _In_ const IMethodInfoSptr& spMethodInfo,
    _In_ BOOL isRejit,
    _Out_ BOOL* pbInstrument)
{
    *pbInstrument = FALSE;

    HRESULT hr = S_OK;
    IfFailRet(hr = m_spInteropHandler->ShouldInstrument(spMethodInfo));

    *pbInstrument = (hr == S_OK);
    return S_OK;
}

_Success_(return == 0)
HRESULT CHostExtensionBase::InternalInstrumentMethod(
    _In_ const IMethodInfoSptr& spMethodInfo,
    _In_ BOOL isRejit)
{
    IfFailRet(m_spInteropHandler->Instrument(spMethodInfo));

    return S_OK;
}


HRESULT CHostExtensionBase::InternalOnModuleLoaded(
    _In_ const IModuleInfoSptr& spModuleInfo)
{
    HRESULT hr = S_OK;

    ATL::CComBSTR bstrModuleName;
    IfFailRet(spModuleInfo->GetModuleName(&bstrModuleName));

    if (bstrModuleName == L"mscorlib.dll")
    {
        std::wstring strModulePath;
        IfFailRet(GetModulePath(strModulePath));

        std::shared_ptr<CModuleMetaDataLoader> spLoader;
        IfFailRet(CreateAndBuildUp(spLoader, strModulePath + ExtensionsBaseOriginalModuleName));

        IfFailRet(spLoader->Load());

        spModuleInfo->ImportModule(spLoader->GetMetaDataImport(), spLoader->GetImageBaseAddress());
    }

    if (nullptr == m_spEnterLeaveExceptionCodeInjector)
    {
        CDecorationCallbacksInfoSptr		spCallbacksInfo;
        IfFailRetHresult(
            this->m_spDecorationCallbacksInfoReader->Read(
            spModuleInfo,
            spCallbacksInfo), hr);

        // if we read callback information
        if (S_OK == hr)
        {
            CCallbacksEmitterSPtr spEnterLeaveExceptionCodeInjector;
            IfFailRet(
                CreateAndBuildUp(
                spEnterLeaveExceptionCodeInjector,
                spCallbacksInfo));

            m_spEnterLeaveExceptionCodeInjector = spEnterLeaveExceptionCodeInjector;
        }
    }

    if (nullptr != m_spEnterLeaveExceptionCodeInjector)
    {
        IfFailRet(this->m_spEnterLeaveExceptionCodeInjector->EmitModule(spModuleInfo));
    }
    else
    {
        TraceMsg(L"Module loaded without injecting type references");
    }

    return hr;
}

void ModuleAppendProcessBitness(_Out_ ATL::CComBSTR& bstrModule)
{
    #if defined(_WIN64)
        bstrModule += L"_x64.dll";
    #elif defined(_WIN32)
        bstrModule += L"_x86.dll";
    #endif
}

__int32 CHostExtensionBase::Attach(
    _In_ const __int64 modulePathPtr,
    _In_ const __int64 modulePtr,
    _In_ const __int64 classGuidPtr,
    _In_ const __int32 dwPriority)
{
    ATL::CComBSTR bstrModulePath(reinterpret_cast<const wchar_t *>(modulePathPtr));
    ATL::CComBSTR bstrName(L"0");
    ATL::CComBSTR bstrDescription(L"0");
    ATL::CComBSTR bstrModuleName(reinterpret_cast<const wchar_t *>(modulePtr));
    ATL::CComBSTR classGuid(reinterpret_cast<const wchar_t *>(classGuidPtr));

    ModuleAppendProcessBitness(bstrModuleName);

    IInstrumentationMethodSptr spInstrumentationMethod;

    IfFailRet(
        this->m_spHost->AddInstrumentationMethod(
            bstrModulePath,
            bstrName,
            bstrDescription,
            bstrModuleName,
            classGuid,
            dwPriority,
            &spInstrumentationMethod));

    AppDomainID currentDomainId = 0;
    IfFailRet(GetCurrentThreadDomainID(currentDomainId));

    IAppDomainCollectionSptr spAppDomainCollection;
    IfFailRet(m_spHost->GetAppDomainCollection(&spAppDomainCollection));

    IEnumAppDomainInfoSptr spEnumAppDomainInfo;
    IfFailRet(spAppDomainCollection->GetAppDomains(&spEnumAppDomainInfo));

    ULONG ulAdFetched = 0;
    IAppDomainInfoSptr spAppDomainInfo;
    for (;
        SUCCEEDED(spEnumAppDomainInfo->Next(1, &spAppDomainInfo, &ulAdFetched))
        && ulAdFetched != 0
        && spAppDomainInfo != nullptr;
        spAppDomainInfo.Detach())
    {
        AppDomainID domainId = 0;
        IfFailRet(spAppDomainInfo->GetAppDomainId(&domainId));

        if (domainId == currentDomainId)
        {
            break;
        }
    }

    IfFalseRet(spAppDomainInfo != nullptr, E_FAIL);

    std::shared_ptr<CAppDomainIMListDataContainerAdapter> spAdapter;
    CreateAndBuildUp(spAdapter);

    std::vector<IInstrumentationMethodSptr> vList;
    HRESULT hrFound = S_OK;

    IfFailRet(hrFound = spAdapter->GetDataItem(spAppDomainInfo, vList));

    if (hrFound == S_FALSE)
    {
        vList.push_back(spInstrumentationMethod);
        spAdapter->SetDataItem(spAppDomainInfo, vList);
    }
    else
    {
        vList.push_back(spInstrumentationMethod);
    }

    return S_OK;
}

HRESULT CHostExtensionBase::InternalOnAppDomainShutdown(
    _In_ const IAppDomainInfoSptr& spAppDomainInfo)
{
    HRESULT hr = S_OK;
    std::shared_ptr<CAppDomainIMListDataContainerAdapter> spAdapter;
    CreateAndBuildUp(spAdapter);

    std::vector<IInstrumentationMethodSptr> vList;
    HRESULT hrFound = S_OK;
    IfFailRet(hrFound = spAdapter->GetDataItem(spAppDomainInfo, vList));

    AppDomainID aid = NULL;
    spAppDomainInfo->GetAppDomainId(&aid);

    if (hrFound == S_FALSE)
    {
        TraceMsg(
            L"There's no extension to unload for application domain",
            Param(L"AppDomainID", aid));

        return S_OK;
    }
    else
    {
        TraceMsg(
            L"Unloading extestions for application domain",
            Param(L"AppDomainID", aid),
            Param(L"Count", vList.size()));

        for (auto spIM : vList)
        {
            IfFailRet(m_spHost->RemoveInstrumentationMethod(spIM));
        }
    }
    return hr;
}


_Success_(return == 0)
HRESULT CHostExtensionBase::InternalAllowInlineSite(
    _In_ const IMethodInfoSptr& spMethodInfoInlinee,
    _In_ const IMethodInfoSptr& spMethodInfoCaller,
    _Out_ BOOL* pbAllowInline)
{
    HRESULT hr = S_OK;

    IfFailRetHresult(
        m_spInteropHandler->AllowInline(
        spMethodInfoInlinee, spMethodInfoCaller), hr);

    *pbAllowInline = (hr != S_OK);

    return hr;
}

