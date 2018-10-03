// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "ProfilerManager.h"
#include "CorProfilerInfoWrapper.h"
#include "MethodInfo.h"

MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::CCorProfilerInfoWrapper(
    _In_ CProfilerManager* pProfilerManager,
    _In_ ICorProfilerInfo* pRealCorProfilerInfo
    )
    :
    m_refcount(0), m_pProfilerManager(pProfilerManager)
{
    m_pRealCorProfilerInfo = pRealCorProfilerInfo;

    HRESULT hr = S_OK;

    // NOTE: It isn't an error for these to fail on older clrs and these bail because if the QI fails for an
    // older clr, it will fail for a newer one.
    hr = m_pRealCorProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo2), (LPVOID*)&m_pRealCorProfilerInfo2);
    if (FAILED(hr))
    {
        return;
    }

    hr = m_pRealCorProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo3), (LPVOID*)&m_pRealCorProfilerInfo3);
    if (FAILED(hr))
    {
        return;
    }

    hr = m_pRealCorProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo4), (LPVOID*)&m_pRealCorProfilerInfo4);
    if (FAILED(hr))
    {
        return;
    }

    hr = m_pRealCorProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo5), (LPVOID*)&m_pRealCorProfilerInfo5);
    if (FAILED(hr))
    {
        return;
    }

    hr = m_pRealCorProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo6), (LPVOID*)&m_pRealCorProfilerInfo6);
    if (FAILED(hr))
    {
        return;
    }

    hr = m_pRealCorProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo7), (LPVOID*)&m_pRealCorProfilerInfo7);
    if (FAILED(hr))
    {
        return;
    }
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::QueryInterface(
    _In_ REFIID riid,
    _Out_ void **ppvObject
    )
{
    IfNullRetPointer(ppvObject);
    *ppvObject = NULL;

    if (riid == __uuidof(IUnknown))
    {
        AddRef();
        *ppvObject = (IUnknown*)this;
        return S_OK;
    }

    if (riid == __uuidof(ICorProfilerInfo))
    {
        AddRef();
        *ppvObject = (ICorProfilerInfo*)this;
        return S_OK;
    }
    else if (riid == __uuidof(ICorProfilerInfo2) && m_pRealCorProfilerInfo2 != NULL)
    {
        AddRef();
        *ppvObject = (ICorProfilerInfo2*)this;
        return S_OK;
    }
    else if (riid == __uuidof(ICorProfilerInfo3) && m_pRealCorProfilerInfo3 != NULL)
    {
        AddRef();
        *ppvObject = (ICorProfilerInfo3*)this;
        return S_OK;
    }
    else if (riid == __uuidof(ICorProfilerInfo4) && m_pRealCorProfilerInfo4 != NULL)
    {
        AddRef();
        *ppvObject = (ICorProfilerInfo4*)this;
        return S_OK;
    }
    else if (riid == __uuidof(ICorProfilerInfo5) && m_pRealCorProfilerInfo5 != NULL)
    {
        AddRef();
        *ppvObject = (ICorProfilerInfo5*)this;
        return S_OK;
    }
    else if (riid == __uuidof(ICorProfilerInfo6) && m_pRealCorProfilerInfo6 != NULL)
    {
        AddRef();
        *ppvObject = (ICorProfilerInfo6*)this;
        return S_OK;
    }
    else if (riid == __uuidof(ICorProfilerInfo7) && m_pRealCorProfilerInfo7 != NULL)
    {
        AddRef();
        *ppvObject = (ICorProfilerInfo7*)this;
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::AddRef()
{
    return InterlockedIncrement(&m_refcount);
}

ULONG MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::Release()
{
    if (InterlockedDecrement(&m_refcount) == 0)
    {
        delete this;
        return 0;
    }

    return m_refcount;
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetClassFromObject(
	_In_ ObjectID objectId,
	_Out_ ClassID* pClassId
)
{
    return m_pRealCorProfilerInfo->GetClassFromObject(objectId, pClassId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetClassFromToken(
	_In_ ModuleID moduleId,
	_In_ mdTypeDef typeDef,
	_Out_ ClassID* pClassId
)
{
    return m_pRealCorProfilerInfo->GetClassFromToken(moduleId, typeDef, pClassId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetCodeInfo(
	_In_ FunctionID functionId,
	_Out_ LPCBYTE* pStart,
	_Out_ ULONG* pcSize
)
{
    return m_pRealCorProfilerInfo->GetCodeInfo(functionId, pStart, pcSize);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetEventMask(
	_Out_ DWORD* pdwEvents
)
{
    IfNullRetPointer(pdwEvents);
	return m_pProfilerManager->GetEventMask(pdwEvents);
}


HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionFromIP(
	_In_ LPCBYTE ip,
	_Out_ FunctionID* pFunctionId
)
{
    return m_pRealCorProfilerInfo->GetFunctionFromIP(ip, pFunctionId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionFromToken(
	_In_ ModuleID moduleId,
	_In_ mdToken token,
	_Out_ FunctionID* pFunctionId
)
{
    return m_pRealCorProfilerInfo->GetFunctionFromToken(moduleId, token, pFunctionId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetHandleFromThread(
	_In_ ThreadID threadId,
	_In_ HANDLE* phThread
)
{
    return m_pRealCorProfilerInfo->GetHandleFromThread(threadId, phThread);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetObjectSize(
	_In_ ObjectID objectId,
	_Out_ ULONG* pcSize
)
{
    return m_pRealCorProfilerInfo->GetObjectSize(objectId, pcSize);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::IsArrayClass(
	_In_ ClassID classId,
	_Out_ CorElementType* pBaseElemType,
	_Out_ ClassID* pBaseClassId,
	_Out_ ULONG* pcRank
)
{
    return m_pRealCorProfilerInfo->IsArrayClass(classId, pBaseElemType, pBaseClassId, pcRank);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetThreadInfo(
	_In_ ThreadID threadId,
	_Out_ DWORD* pdwWin32ThreadId
)
{
    return m_pRealCorProfilerInfo->GetThreadInfo(threadId, pdwWin32ThreadId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetCurrentThreadID(
	_Out_ ThreadID* pThreadId
)
{
    return m_pRealCorProfilerInfo->GetCurrentThreadID(pThreadId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetClassIDInfo(
	_In_ ClassID classId,
	_Out_ ModuleID* pModuleId,
	_Out_ mdTypeDef* pTypeDefToken
)
{
    return m_pRealCorProfilerInfo->GetClassIDInfo(classId, pModuleId, pTypeDefToken);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionInfo(
	_In_ FunctionID functionId,
	_Out_ ClassID* pClassId,
	_Out_ ModuleID* pModuleId,
	_Out_ mdToken* pToken
)
{
    return m_pRealCorProfilerInfo->GetFunctionInfo(functionId, pClassId, pModuleId, pToken);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetEventMask(
	_In_ DWORD dwEvents
)
{
    return m_pProfilerManager->SetEventMask(dwEvents);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetEnterLeaveFunctionHooks(
	_In_ FunctionEnter* pFuncEnter,
	_In_ FunctionLeave* pFuncLeave,
	_In_ FunctionTailcall* pFuncTailcall
)
{
    // TODO: Enforce a single consumer of this api?
    return m_pRealCorProfilerInfo->SetEnterLeaveFunctionHooks(pFuncEnter, pFuncLeave, pFuncTailcall);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetFunctionIDMapper(
	_In_ FunctionIDMapper* pFunc
)
{
    // TODO: Enforce a single consumer of this api?
    return m_pRealCorProfilerInfo->SetFunctionIDMapper(pFunc);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetTokenAndMetaDataFromFunction(
	_In_ FunctionID functionId,
	_In_ REFIID riid,
	_Out_ IUnknown **ppImport,
	_Out_ mdToken* pToken
)
{
    return m_pRealCorProfilerInfo->GetTokenAndMetaDataFromFunction(functionId, riid, ppImport, pToken);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetModuleInfo(
	_In_ ModuleID moduleId,
	_Out_ LPCBYTE* ppBaseLoadAddress,
	_In_ ULONG cchName,
	_Out_ ULONG* pcchName,
	_Out_writes_(cchName) WCHAR szName[],
	_Out_ AssemblyID* pAssemblyId
)
{
    return m_pRealCorProfilerInfo->GetModuleInfo(moduleId, ppBaseLoadAddress, cchName, pcchName, szName, pAssemblyId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetModuleMetaData(
	_In_ ModuleID moduleId,
	_In_ DWORD dwOpenFlags,
	_In_ REFIID riid,
	_Out_ IUnknown **ppOut
)
{
    return m_pRealCorProfilerInfo->GetModuleMetaData(moduleId, dwOpenFlags, riid, ppOut);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetILFunctionBody(
    _In_ ModuleID moduleId,
    _In_ mdMethodDef methodToken,
    _Out_ LPCBYTE* ppMethodHeader,
    _Out_ ULONG* pcbMethodSize
    )
{
    HRESULT hr = S_OK;
    *ppMethodHeader = nullptr;
    *pcbMethodSize = 0;

    CComPtr<CProfilerManager> pProfilerManager;
    IfFailRet(CProfilerManager::GetProfilerManagerInstance(&pProfilerManager));

    CComPtr<CAppDomainCollection> pAppDomainCollection;
    IfFailRet(pProfilerManager->GetAppDomainCollection((IAppDomainCollection**)&pAppDomainCollection));

    CComPtr<CModuleInfo> pModuleInfo;
	CComPtr<CMethodInfo> pMethodInfo;
    if (SUCCEEDED(pAppDomainCollection->GetModuleInfoById(moduleId, (IModuleInfo**)&pModuleInfo)))
    {
        hr = pModuleInfo->GetMethodInfoByToken(methodToken, &pMethodInfo);
        if (SUCCEEDED(hr) && pMethodInfo->IsInstrumented())
        {
            return pMethodInfo->GetIntermediateRenderedFunctionBody(ppMethodHeader, pcbMethodSize);
        }
    }

    IMAGE_COR_ILMETHOD* pMethodHeader = nullptr;
    ULONG cbMethodSize = 0;

    // Method info wasn't instrumented by an instrumentation method. Ask the module info for the il.
    // This will properly manage the cache of original il bytes.
    IfFailRet(pModuleInfo->GetMethodIl(m_pRealCorProfilerInfo, methodToken, (IMAGE_COR_ILMETHOD**)&pMethodHeader, &cbMethodSize));

	*ppMethodHeader = (LPCBYTE)pMethodHeader;
	*pcbMethodSize = cbMethodSize;

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetILFunctionBodyAllocator(
	_In_ ModuleID moduleId,
    _Out_ IMethodMalloc **ppMalloc
    )
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppMalloc);

    // NOTE: The actual malloc allocation in the CLR is done at the end when the profiler manager
    // is setting the IL. Not, during this callback. However, some memory must be returned for them to
    // write to so a private implementation is returned that just allocates on the heap.
    // Those bytes are deleted by CCorMethodMalloc after the call to SetILFunctionBody when its refcount
    // hits 0
    if (m_pMethodMalloc == nullptr)
    {
        m_pMethodMalloc.Attach(new CCorMethodMalloc);
        if (m_pMethodMalloc == nullptr)
        {
            return E_OUTOFMEMORY;
        }

    }

    *ppMalloc = (IMethodMalloc *)m_pMethodMalloc;
    (*ppMalloc)->AddRef();

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetILFunctionBody(
    _In_ ModuleID moduleId,
	_In_ mdMethodDef methodToken,
	_In_ LPCBYTE pbNewILMethodHeader
    )
{
    HRESULT hr = S_OK;

    CComPtr<CProfilerManager> pProfilerManager;
    IfFailRet(CProfilerManager::GetProfilerManagerInstance(&pProfilerManager));

    CComPtr<CAppDomainCollection> pAppDomainCollection;
    IfFailRet(pProfilerManager->GetAppDomainCollection((IAppDomainCollection**)&pAppDomainCollection));

    CComPtr<CModuleInfo> pModuleInfo;
    if (SUCCEEDED(pAppDomainCollection->GetModuleInfoById(moduleId, (IModuleInfo**)&pModuleInfo)))
    {
        CComPtr<CMethodInfo> pMethodInfo;
        hr = pModuleInfo->GetMethodInfoByToken(methodToken, &pMethodInfo);
        if (SUCCEEDED(hr))
        {
            if (m_pMethodMalloc == nullptr)
            {
                CLogging::LogError(_T("Incorrect buffer passed to SetFunctionBodyMalloc"));
                return E_FAIL;
            }
            BYTE* pBuffer;
            ULONG cbBuffer = 0;
            IfFailRet(m_pMethodMalloc->GetCurrentBufferAndLen(&pBuffer, &cbBuffer));

            if (pbNewILMethodHeader != pBuffer)
            {
                CLogging::LogError(_T("Incorrect buffer passed to SetFunctionBodyMalloc"));
                return E_FAIL;
            }

            // NOTE: Relying on the size of the buffer that was handed out by the IMethodMalloc.
            // this relies on the serialization of jit events by the profiler manager.
            // Calculating the actual method size would require a fourth parse of header.
            IfFailRet(pMethodInfo->SetFinalRenderedFunctionBody(pbNewILMethodHeader, cbBuffer));
        }
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetAppDomainInfo(
	_In_ AppDomainID appDomainId,
	_In_ ULONG cchName,
	_Out_ ULONG* pcchName,
	_Out_writes_(cchName) WCHAR szName[],
	_Out_ ProcessID* pProcessId
)
{
    return m_pRealCorProfilerInfo->GetAppDomainInfo(appDomainId, cchName, pcchName, szName, pProcessId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetAssemblyInfo(
    _In_ AssemblyID assemblyId,
	_In_ ULONG cchName,
	_Out_ ULONG* pcchName,
    _Out_writes_to_(cchName,* pcchName)  WCHAR szName[],
    _Out_ AppDomainID* pAppDomainId,
    _Out_ ModuleID* pModuleId
    )
{
    return m_pRealCorProfilerInfo->GetAssemblyInfo(assemblyId, cchName, pcchName, szName, pAppDomainId, pModuleId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetFunctionReJIT(
    _In_ FunctionID functionId
    )
{
    return m_pRealCorProfilerInfo->SetFunctionReJIT(functionId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::ForceGC()
{
    return m_pRealCorProfilerInfo->ForceGC();
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetILInstrumentedCodeMap(
    _In_ FunctionID functionId,
    _In_ BOOL fStartJit,
    _In_ ULONG cILMapEntries,
    _In_reads_(cILMapEntries) COR_IL_MAP rgILMapEntries[]
    )
{
    HRESULT hr = S_OK;

    CComPtr<CMethodInfo> pMethodInfo;
    IfFailRet(CMethodInfo::GetMethodInfoById(functionId, &pMethodInfo));

    IfFailRet(pMethodInfo->MergeILInstrumentedCodeMap(cILMapEntries, rgILMapEntries));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetInprocInspectionInterface(
    _Out_ IUnknown **ppicd
    )
{
    return m_pRealCorProfilerInfo->GetInprocInspectionInterface(ppicd);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetInprocInspectionIThisThread(
    _Out_ IUnknown **ppicd
    )
{
    return m_pRealCorProfilerInfo->GetInprocInspectionIThisThread(ppicd);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetThreadContext(
    _In_ ThreadID threadId,
    _Out_ ContextID* pContextId
    )
{
    return m_pRealCorProfilerInfo->GetThreadContext(threadId, pContextId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::BeginInprocDebugging(
    _In_ BOOL fThisThreadOnly,
    _Out_ DWORD* pdwProfilerContext
    )
{
    return m_pRealCorProfilerInfo->BeginInprocDebugging(fThisThreadOnly, pdwProfilerContext);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::EndInprocDebugging(
    _In_ DWORD dwProfilerContext
    )
{
    return m_pRealCorProfilerInfo->EndInprocDebugging(dwProfilerContext);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetILToNativeMapping(
    _In_ FunctionID functionId,
    _In_ ULONG32 cMap,
    _Out_ ULONG32* pcMap,
    _Out_writes_(cMap) COR_DEBUG_IL_TO_NATIVE_MAP map[]
    )
{
    return m_pRealCorProfilerInfo->GetILToNativeMapping(functionId, cMap, pcMap, map);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::DoStackSnapshot(
    _In_ ThreadID thread,
    _In_ StackSnapshotCallback *callback,
    _In_ ULONG32 infoFlags,
    _In_ void *clientData,
    _In_ BYTE context[],
    _In_ ULONG32 contextSize
    )
{
    // TODO: Override
    return m_pRealCorProfilerInfo2->DoStackSnapshot(thread, callback, infoFlags, clientData, context, contextSize);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetEnterLeaveFunctionHooks2(
    _In_ FunctionEnter2* pFuncEnter,
    _In_ FunctionLeave2* pFuncLeave,
    _In_ FunctionTailcall2* pFuncTailcall
    )
{
    // TODO: Enforce single caller?
    return m_pRealCorProfilerInfo2->SetEnterLeaveFunctionHooks2(pFuncEnter, pFuncLeave, pFuncTailcall);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionInfo2(
    _In_ FunctionID funcId,
    _In_ COR_PRF_FRAME_INFO frameInfo,
    _Out_ ClassID* pClassId,
    _Out_ ModuleID* pModuleId,
    _Out_ mdToken* pToken,
    _In_ ULONG32 cTypeArgs,
    _Out_ ULONG32* pcTypeArgs,
    _Out_writes_(cTypeArgs) ClassID typeArgs[]
    )
{
    return m_pRealCorProfilerInfo2->GetFunctionInfo2(funcId, frameInfo, pClassId, pModuleId, pToken, cTypeArgs, pcTypeArgs, typeArgs);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetStringLayout(
    _Out_ ULONG* pBufferLengthOffset,
    _Out_ ULONG* pStringLengthOffset,
    _Out_ ULONG* pBufferOffset
    )
{
    return m_pRealCorProfilerInfo2->GetStringLayout(pBufferLengthOffset, pStringLengthOffset, pBufferOffset);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetClassLayout(
    _In_ ClassID classID,
    _Inout_updates_(cFieldOffset) COR_FIELD_OFFSET rFieldOffset[],
    _In_ ULONG cFieldOffset,
    _Out_ ULONG* pcFieldOffset,
    _Out_ ULONG* pulClassSize
    )
{
    return m_pRealCorProfilerInfo2->GetClassLayout(classID, rFieldOffset, cFieldOffset, pcFieldOffset, pulClassSize);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetClassIDInfo2(
    _In_ ClassID classId,
    _Out_ ModuleID* pModuleId,
    _Out_ mdTypeDef* pTypeDefToken,
    ClassID* pParentClassId,
    _In_ ULONG32 cNumTypeArgs,
    _Out_ ULONG32* pcNumTypeArgs,
    _Out_writes_(cNumTypeArgs) ClassID typeArgs[]
)
{
    return m_pRealCorProfilerInfo2->GetClassIDInfo2(classId, pModuleId, pTypeDefToken, pParentClassId, cNumTypeArgs, pcNumTypeArgs, typeArgs);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetCodeInfo2(
	_In_ FunctionID functionID,
	_In_ ULONG32 cCodeInfos,
	_Out_ ULONG32* pcCodeInfos,
	_Out_writes_(cCodeInfos) COR_PRF_CODE_INFO codeInfos[]
)
{
    return m_pRealCorProfilerInfo2->GetCodeInfo2(functionID, cCodeInfos, pcCodeInfos, codeInfos);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetClassFromTokenAndTypeArgs(
	_In_ ModuleID moduleID,
	_In_ mdTypeDef typeDef,
	_In_ ULONG32 cTypeArgs,
	_Out_writes_(cTypeArgs) ClassID typeArgs[],
	_Out_ ClassID* pClassID
)
{
    return m_pRealCorProfilerInfo2->GetClassFromTokenAndTypeArgs(moduleID, typeDef, cTypeArgs, typeArgs, pClassID);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionFromTokenAndTypeArgs(
	_In_  ModuleID moduleID,
	_In_ mdMethodDef funcDef,
	_In_ ClassID classId,
	_In_ ULONG32 cTypeArgs,
	_Out_writes_(cTypeArgs) ClassID typeArgs[],
	_Out_ FunctionID* pFunctionID
)
{
    return m_pRealCorProfilerInfo2->GetFunctionFromTokenAndTypeArgs(moduleID, funcDef, classId, cTypeArgs, typeArgs, pFunctionID);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::EnumModuleFrozenObjects(
	_In_  ModuleID moduleID,
	_Out_ ICorProfilerObjectEnum **ppEnum
)
{
    return m_pRealCorProfilerInfo2->EnumModuleFrozenObjects(moduleID, ppEnum);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetArrayObjectInfo(
	_In_  ObjectID objectId,
	_In_  ULONG32 cDimensions,
	_Out_writes_(cDimensions) ULONG32 pDimensionSizes[],
	_Out_writes_(cDimensions)  int pDimensionLowerBounds[],
	_Out_ BYTE **ppData
)
{
    return m_pRealCorProfilerInfo2->GetArrayObjectInfo(objectId, cDimensions, pDimensionSizes, pDimensionLowerBounds, ppData);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetBoxClassLayout(
	_In_  ClassID classId,
	_Out_ ULONG32* pBufferOffset
)
{
    return m_pRealCorProfilerInfo2->GetBoxClassLayout(classId, pBufferOffset);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetThreadAppDomain(
	_In_ ThreadID threadId,
	_Out_ AppDomainID* pAppDomainId
)
{
    return m_pRealCorProfilerInfo2->GetThreadAppDomain(threadId, pAppDomainId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetRVAStaticAddress(
	_In_ ClassID classId,
	_In_ mdFieldDef fieldToken,
	_Out_ void **ppAddress
)
{
    return m_pRealCorProfilerInfo2->GetRVAStaticAddress(classId, fieldToken, ppAddress);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetAppDomainStaticAddress(
	_In_ ClassID classId,
	_In_ mdFieldDef fieldToken,
	_In_ AppDomainID appDomainId,
	_Out_ void **ppAddress
)
{
    return m_pRealCorProfilerInfo2->GetAppDomainStaticAddress(classId, fieldToken, appDomainId, ppAddress);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetThreadStaticAddress(
	_In_ ClassID classId,
	_In_ mdFieldDef fieldToken,
	_In_ ThreadID threadId,
	_Out_ void **ppAddress
)
{
    return m_pRealCorProfilerInfo2->GetThreadStaticAddress(classId, fieldToken, threadId, ppAddress);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetContextStaticAddress(
	_In_ ClassID classId,
	_In_ mdFieldDef fieldToken,
	_In_ ContextID contextId,
	_Out_ void **ppAddress
)
{
    return m_pRealCorProfilerInfo2->GetContextStaticAddress(classId, fieldToken, contextId, ppAddress);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetStaticFieldInfo(
	_In_ ClassID classId,
	_In_ mdFieldDef fieldToken,
	_Out_ COR_PRF_STATIC_TYPE* pFieldInfo
)
{
    return m_pRealCorProfilerInfo2->GetStaticFieldInfo(classId, fieldToken, pFieldInfo);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetGenerationBounds(
	_In_ ULONG cObjectRanges,
	_Out_ ULONG* pcObjectRanges,
	_Out_writes_(cObjectRanges) COR_PRF_GC_GENERATION_RANGE ranges[]
)
{
    return m_pRealCorProfilerInfo2->GetGenerationBounds(cObjectRanges, pcObjectRanges, ranges);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetObjectGeneration(
	_In_ ObjectID objectId,
	_Out_ COR_PRF_GC_GENERATION_RANGE *range
)
{
    return m_pRealCorProfilerInfo2->GetObjectGeneration(objectId, range);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetNotifiedExceptionClauseInfo(
	_Out_ COR_PRF_EX_CLAUSE_INFO* pinfo
)
{
    return m_pRealCorProfilerInfo2->GetNotifiedExceptionClauseInfo(pinfo);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::EnumJITedFunctions(
	_Out_ ICorProfilerFunctionEnum **ppEnum
    )
{
    return m_pRealCorProfilerInfo3->EnumJITedFunctions(ppEnum);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::RequestProfilerDetach(
    _In_ DWORD dwExpectedCompletionMilliseconds
    )
{
    return m_pRealCorProfilerInfo3->RequestProfilerDetach(dwExpectedCompletionMilliseconds);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetFunctionIDMapper2(
	_In_ FunctionIDMapper2* pFunc,
	_Out_ void *clientData
)
{
    return m_pRealCorProfilerInfo3->SetFunctionIDMapper2(pFunc, clientData);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetStringLayout2(
	_Out_ ULONG* pStringLengthOffset,
	_Out_ ULONG* pBufferOffset
)
{
    return m_pRealCorProfilerInfo3->GetStringLayout2(pStringLengthOffset, pBufferOffset);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetEnterLeaveFunctionHooks3(
	_In_ FunctionEnter3* pFuncEnter3,
	_In_ FunctionLeave3* pFuncLeave3,
	_In_ FunctionTailcall3* pFuncTailcall3
)
{
    // TODO: Enforce single caller?
    return m_pRealCorProfilerInfo3->SetEnterLeaveFunctionHooks3(pFuncEnter3, pFuncLeave3, pFuncTailcall3);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetEnterLeaveFunctionHooks3WithInfo(
	_In_ FunctionEnter3WithInfo* pFuncEnter3WithInfo,
	_In_ FunctionLeave3WithInfo* pFuncLeave3WithInfo,
	_In_ FunctionTailcall3WithInfo* pFuncTailcall3WithInfo
)
{
    // TODO: Enforce single caller?
    return m_pRealCorProfilerInfo3->SetEnterLeaveFunctionHooks3WithInfo(pFuncEnter3WithInfo, pFuncLeave3WithInfo, pFuncTailcall3WithInfo);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionEnter3Info(
	_In_ FunctionID functionId,
	_In_ COR_PRF_ELT_INFO eltInfo,
	_Out_ COR_PRF_FRAME_INFO* pFrameInfo,
	_Inout_ ULONG* pcbArgumentInfo,
	_Out_writes_(*pcbArgumentInfo) COR_PRF_FUNCTION_ARGUMENT_INFO* pArgumentInfo
)
{
    return m_pRealCorProfilerInfo3->GetFunctionEnter3Info(functionId, eltInfo, pFrameInfo, pcbArgumentInfo, pArgumentInfo);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionLeave3Info(
	_In_ FunctionID functionId,
	_In_ COR_PRF_ELT_INFO eltInfo,
	_Out_ COR_PRF_FRAME_INFO* pFrameInfo,
	_Out_ COR_PRF_FUNCTION_ARGUMENT_RANGE* pRetvalRange
)
{
    return m_pRealCorProfilerInfo3->GetFunctionLeave3Info(functionId, eltInfo, pFrameInfo, pRetvalRange);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionTailcall3Info(
	_In_ FunctionID functionId,
	_In_ COR_PRF_ELT_INFO eltInfo,
	_Out_ COR_PRF_FRAME_INFO* pFrameInfo
)
{
    return m_pRealCorProfilerInfo3->GetFunctionTailcall3Info(functionId, eltInfo, pFrameInfo);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::EnumModules(
	_In_ ICorProfilerModuleEnum **ppEnum
)
{
    return m_pRealCorProfilerInfo3->EnumModules(ppEnum);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetRuntimeInformation(
	_Out_ USHORT* pClrInstanceId,
	_Out_ COR_PRF_RUNTIME_TYPE* pRuntimeType,
	_Out_ USHORT* pMajorVersion,
	_Out_ USHORT* pMinorVersion,
	_Out_ USHORT* pBuildNumber,
	_Out_ USHORT* pQFEVersion,
	_In_ ULONG cchVersionString,
	_Out_ ULONG* pcchVersionString,
	_Out_writes_(cchVersionString) WCHAR szVersionString[]
)
{
    return m_pRealCorProfilerInfo3->GetRuntimeInformation(pClrInstanceId, pRuntimeType, pMajorVersion, pMinorVersion, pBuildNumber, pQFEVersion, cchVersionString, pcchVersionString, szVersionString);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetThreadStaticAddress2(
	_In_ ClassID classId,
	_In_ mdFieldDef fieldToken,
	_In_ AppDomainID appDomainId,
	_In_ ThreadID threadId,
	_Out_ void **ppAddress
)
{
    return m_pRealCorProfilerInfo3->GetThreadStaticAddress2(classId, fieldToken, appDomainId, threadId, ppAddress);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetAppDomainsContainingModule(
	_In_ ModuleID moduleId,
	_In_ ULONG32 cAppDomainIds,
	_Out_ ULONG32* pcAppDomainIds,
	_Out_writes_(cAppDomainIds) AppDomainID appDomainIds[]
)
{
    return m_pRealCorProfilerInfo3->GetAppDomainsContainingModule(moduleId, cAppDomainIds, pcAppDomainIds, appDomainIds);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetModuleInfo2(
	_In_ ModuleID moduleId,
	_Out_ LPCBYTE* ppBaseLoadAddress,
	_In_ ULONG cchName,
	_Out_ ULONG* pcchName,
	_Out_writes_(cchName) WCHAR szName[],
	_Out_ AssemblyID* pAssemblyId,
	_Out_ DWORD* pdwModuleFlags
)
{
    return m_pRealCorProfilerInfo3->GetModuleInfo2(moduleId, ppBaseLoadAddress, cchName, pcchName, szName, pAssemblyId, pdwModuleFlags);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::EnumThreads(
	_Out_ ICorProfilerThreadEnum **ppEnum
)
{
    return m_pRealCorProfilerInfo4->EnumThreads(ppEnum);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::InitializeCurrentThread()
{
    return m_pRealCorProfilerInfo4->InitializeCurrentThread();
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::RequestReJIT(
	_In_ ULONG cFunctions,
	_In_reads_(cFunctions) ModuleID moduleIds[],
	_In_reads_(cFunctions) mdMethodDef methodIds[]
)
{
    return m_pRealCorProfilerInfo4->RequestReJIT(cFunctions, moduleIds, methodIds);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::RequestRevert(
	_In_ ULONG cFunctions,
	_In_reads_(cFunctions) ModuleID moduleIds[],
	_In_reads_(cFunctions) mdMethodDef methodIds[],
	_In_reads_(cFunctions) HRESULT status[]
)
{
    return m_pRealCorProfilerInfo4->RequestRevert(cFunctions, moduleIds, methodIds, status);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetCodeInfo3(
	_In_ FunctionID functionID,
	_In_ ReJITID reJitId,
	_In_ ULONG32 cCodeInfos,
	_Out_ ULONG32* pcCodeInfos,
	_Out_writes_(cCodeInfos) COR_PRF_CODE_INFO codeInfos[]
)
{
    // Does this need to be overridden?
    return m_pRealCorProfilerInfo4->GetCodeInfo3(functionID, reJitId, cCodeInfos, pcCodeInfos, codeInfos);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetFunctionFromIP2(
	_In_ LPCBYTE ip,
	_Out_ FunctionID* pFunctionId,
	_Out_ ReJITID* pReJitId
)
{
    return m_pRealCorProfilerInfo4->GetFunctionFromIP2(ip, pFunctionId, pReJitId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetReJITIDs(
	_In_ FunctionID functionId,
	_In_ ULONG cReJitIds,
	_Out_ ULONG* pcReJitIds,
	_Out_writes_(cReJitIds) ReJITID reJitIds[]
)
{
    return m_pRealCorProfilerInfo4->GetReJITIDs(functionId, cReJitIds, pcReJitIds, reJitIds);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetILToNativeMapping2(
	_In_ FunctionID functionId,
	_In_ ReJITID reJitId,
	_In_ ULONG32 cMap,
	_Out_  ULONG32* pcMap,
	_Out_writes_(cMap) COR_DEBUG_IL_TO_NATIVE_MAP map[]
)
{
    // TODO: Override
    return m_pRealCorProfilerInfo4->GetILToNativeMapping2(functionId, reJitId, cMap, pcMap, map);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::EnumJITedFunctions2(
	_In_ ICorProfilerFunctionEnum **ppEnum
)
{
    return m_pRealCorProfilerInfo4->EnumJITedFunctions2(ppEnum);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetObjectSize2(
	_In_ ObjectID objectId,
	_Out_ SIZE_T* pcSize
)
{
    return m_pRealCorProfilerInfo4->GetObjectSize2(objectId, pcSize);
}

// ICorProfilerInfo5
HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetEventMask2(
    _Out_ DWORD *pdwEventsLow,
    _Out_ DWORD *pdwEventsHigh
)
{
    return m_pRealCorProfilerInfo5->GetEventMask2(pdwEventsLow, pdwEventsHigh);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::SetEventMask2(
    _In_ DWORD dwEventsLow,
    _In_ DWORD dwEventsHigh
)
{
    return m_pProfilerManager->SetEventMask2(dwEventsLow, dwEventsHigh);
}

// ICorProfilerInfo6
HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::EnumNgenModuleMethodsInliningThisMethod(
    _In_ ModuleID inlinersModuleId,
    _In_ ModuleID inlineeModuleId,
    _In_ mdMethodDef inlineeMethodId,
    _Out_ BOOL *incompleteData,
    _Out_ ICorProfilerMethodEnum **ppEnum
)
{
    return m_pRealCorProfilerInfo6->EnumNgenModuleMethodsInliningThisMethod(
        inlinersModuleId,
        inlineeModuleId,
        inlineeMethodId,
        incompleteData,
        ppEnum
    );
}

// ICorProfilerInfo7
HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::ApplyMetaData(
    _In_ ModuleID moduleId
)
{
    return m_pRealCorProfilerInfo7->ApplyMetaData(moduleId);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::GetInMemorySymbolsLength(
    _In_ ModuleID moduleId,
    _Out_ DWORD *pCountSymbolBytes
)
{
    return m_pRealCorProfilerInfo7->GetInMemorySymbolsLength(moduleId, pCountSymbolBytes);
}

HRESULT MicrosoftInstrumentationEngine::CCorProfilerInfoWrapper::ReadInMemorySymbols(
    _In_ ModuleID moduleId,
    _In_ DWORD symbolsReadOffset,
    _Out_ BYTE *pSymbolBytes,
    _In_ DWORD countSymbolBytes,
    _Out_ DWORD *pCountSymbolBytesRead
)
{
    return m_pRealCorProfilerInfo7->ReadInMemorySymbols(
        moduleId,
        symbolsReadOffset,
        pSymbolBytes,
        countSymbolBytes,
        pCountSymbolBytesRead
    );
}