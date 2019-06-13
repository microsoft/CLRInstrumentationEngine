// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "CoRawProfilerHook.h"

// ICorProfilerCallback

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::Initialize(
    /* [in] */ IUnknown *pICorProfilerInfoUnk)
{
    ::SetEnvironmentVariable(L"CCoRawProfilerHook::Initialize", L"S_OK");

    ATL::CComPtr<ICorProfilerInfo5> m_pRealProfilerInfo;
    pICorProfilerInfoUnk->QueryInterface(__uuidof(ICorProfilerInfo5), (LPVOID*)&m_pRealProfilerInfo);
    m_pRealProfilerInfo->SetEventMask2(0, COR_PRF_HIGH_ADD_ASSEMBLY_REFERENCES);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::Shutdown(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::AppDomainCreationStarted(
    /* [in] */ AppDomainID appDomainId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::AppDomainCreationFinished(
    /* [in] */ AppDomainID appDomainId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::AppDomainShutdownStarted(
    /* [in] */ AppDomainID appDomainId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::AppDomainShutdownFinished(
    /* [in] */ AppDomainID appDomainId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::AssemblyLoadStarted(
    /* [in] */ AssemblyID assemblyId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::AssemblyLoadFinished(
    /* [in] */ AssemblyID assemblyId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::AssemblyUnloadStarted(
    /* [in] */ AssemblyID assemblyId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::AssemblyUnloadFinished(
    /* [in] */ AssemblyID assemblyId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ModuleLoadStarted(
    /* [in] */ ModuleID moduleId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ModuleLoadFinished(
    /* [in] */ ModuleID moduleId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ModuleUnloadStarted(
    /* [in] */ ModuleID moduleId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ModuleUnloadFinished(
    /* [in] */ ModuleID moduleId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ModuleAttachedToAssembly(
    /* [in] */ ModuleID moduleId,
    /* [in] */ AssemblyID AssemblyId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ClassLoadStarted(
    /* [in] */ ClassID classId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ClassLoadFinished(
    /* [in] */ ClassID classId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ClassUnloadStarted(
    /* [in] */ ClassID classId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ClassUnloadFinished(
    /* [in] */ ClassID classId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::FunctionUnloadStarted(
    /* [in] */ FunctionID functionId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::JITCompilationStarted(
    /* [in] */ FunctionID functionId,
    /* [in] */ BOOL fIsSafeToBlock){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::JITCompilationFinished(
    /* [in] */ FunctionID functionId,
    /* [in] */ HRESULT hrStatus,
    /* [in] */ BOOL fIsSafeToBlock){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::JITCachedFunctionSearchStarted(
    /* [in] */ FunctionID functionId,
    /* [out] */ BOOL *pbUseCachedFunction){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::JITCachedFunctionSearchFinished(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_JIT_CACHE result){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::JITFunctionPitched(
    /* [in] */ FunctionID functionId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::JITInlining(
    /* [in] */ FunctionID callerId,
    /* [in] */ FunctionID calleeId,
    /* [out] */ BOOL *pfShouldInline){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ThreadCreated(
    /* [in] */ ThreadID threadId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ThreadDestroyed(
    /* [in] */ ThreadID threadId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ThreadAssignedToOSThread(
    /* [in] */ ThreadID managedThreadId,
    /* [in] */ DWORD osThreadId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RemotingClientInvocationStarted(){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RemotingClientSendingMessage(
    /* [in] */ GUID *pCookie,
    /* [in] */ BOOL fIsAsync){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RemotingClientReceivingReply(
    /* [in] */ GUID *pCookie,
    /* [in] */ BOOL fIsAsync){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RemotingClientInvocationFinished(){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RemotingServerReceivingMessage(
    /* [in] */ GUID *pCookie,
    /* [in] */ BOOL fIsAsync){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RemotingServerInvocationStarted(){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RemotingServerInvocationReturned(){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RemotingServerSendingReply(
    /* [in] */ GUID *pCookie,
    /* [in] */ BOOL fIsAsync){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::UnmanagedToManagedTransition(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_TRANSITION_REASON reason){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ManagedToUnmanagedTransition(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_TRANSITION_REASON reason){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RuntimeSuspendStarted(
    /* [in] */ COR_PRF_SUSPEND_REASON suspendReason){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RuntimeSuspendFinished(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RuntimeSuspendAborted(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RuntimeResumeStarted(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RuntimeResumeFinished(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RuntimeThreadSuspended(
    /* [in] */ ThreadID threadId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RuntimeThreadResumed(
    /* [in] */ ThreadID threadId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::MovedReferences(
    /* [in] */ ULONG cMovedObjectIDRanges,
    /* [size_is][in] */ ObjectID oldObjectIDRangeStart[],
    /* [size_is][in] */ ObjectID newObjectIDRangeStart[],
    /* [size_is][in] */ ULONG cObjectIDRangeLength[]){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ObjectAllocated(
    /* [in] */ ObjectID objectId,
    /* [in] */ ClassID classId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ObjectsAllocatedByClass(
    /* [in] */ ULONG cClassCount,
    /* [size_is][in] */ ClassID classIds[],
    /* [size_is][in] */ ULONG cObjects[]){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ObjectReferences(
    /* [in] */ ObjectID objectId,
    /* [in] */ ClassID classId,
    /* [in] */ ULONG cObjectRefs,
    /* [size_is][in] */ ObjectID objectRefIds[]){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RootReferences(
    /* [in] */ ULONG cRootRefs,
    /* [size_is][in] */ ObjectID rootRefIds[]){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionThrown(
    /* [in] */ ObjectID thrownObjectId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionSearchFunctionEnter(
    /* [in] */ FunctionID functionId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionSearchFunctionLeave(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionSearchFilterEnter(
    /* [in] */ FunctionID functionId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionSearchFilterLeave(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionSearchCatcherFound(
    /* [in] */ FunctionID functionId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionOSHandlerEnter(
    /* [in] */ UINT_PTR __unused){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionOSHandlerLeave(
    /* [in] */ UINT_PTR __unused){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionUnwindFunctionEnter(
    /* [in] */ FunctionID functionId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionUnwindFunctionLeave(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionUnwindFinallyEnter(
    /* [in] */ FunctionID functionId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionUnwindFinallyLeave(void){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionCatcherEnter(
    /* [in] */ FunctionID functionId,
    /* [in] */ ObjectID objectId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionCatcherLeave(){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::COMClassicVTableCreated(
    /* [in] */ ClassID wrappedClassId,
    /* [in] */ REFGUID implementedIID,
    /* [in] */ void *pVTable,
    /* [in] */ ULONG cSlots){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::COMClassicVTableDestroyed(
    /* [in] */ ClassID wrappedClassId,
    /* [in] */ REFGUID implementedIID,
    /* [in] */ void *pVTable){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionCLRCatcherFound(){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ExceptionCLRCatcherExecute(){ return S_OK; }

// ICorProfilerCallback2

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ThreadNameChanged(
    /* [in] */ ThreadID threadId,
    /* [in] */ ULONG cchName,
    /* [annotation][in] */
    _In_reads_opt_(cchName)  WCHAR name[]){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::GarbageCollectionStarted(
    /* [in] */ int cGenerations,
    /* [size_is][in] */ BOOL generationCollected[],
    /* [in] */ COR_PRF_GC_REASON reason){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::SurvivingReferences(
    /* [in] */ ULONG cSurvivingObjectIDRanges,
    /* [size_is][in] */ ObjectID objectIDRangeStart[],
    /* [size_is][in] */ ULONG cObjectIDRangeLength[]){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::GarbageCollectionFinished(){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::FinalizeableObjectQueued(
    /* [in] */ DWORD finalizerFlags,
    /* [in] */ ObjectID objectID){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::RootReferences2(
    /* [in] */ ULONG cRootRefs,
    /* [size_is][in] */ ObjectID rootRefIds[],
    /* [size_is][in] */ COR_PRF_GC_ROOT_KIND rootKinds[],
    /* [size_is][in] */ COR_PRF_GC_ROOT_FLAGS rootFlags[],
    /* [size_is][in] */ UINT_PTR rootIds[]){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::HandleCreated(
    /* [in] */ GCHandleID handleId,
    /* [in] */ ObjectID initialObjectId){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::HandleDestroyed(
    /* [in] */ GCHandleID handleId){
    return S_OK;
}

// ICorProfilerCallback3

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::InitializeForAttach(
    /* [in] */ IUnknown *pCorProfilerInfoUnk,
    /* [in] */ void *pvClientData,
    /* [in] */ UINT cbClientData){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ProfilerAttachComplete(){ return S_OK; }

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ProfilerDetachSucceeded(){ return S_OK; }

// ICorProfilerCallback4

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ReJITCompilationStarted(
    /* [in] */ FunctionID functionId,
    /* [in] */ ReJITID rejitId,
    /* [in] */ BOOL fIsSafeToBlock){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::GetReJITParameters(
    /* [in] */ ModuleID moduleId,
    /* [in] */ mdMethodDef methodId,
    /* [in] */ ICorProfilerFunctionControl *pFunctionControl){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ReJITCompilationFinished(
    /* [in] */ FunctionID functionId,
    /* [in] */ ReJITID rejitId,
    /* [in] */ HRESULT hrStatus,
    /* [in] */ BOOL fIsSafeToBlock){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ReJITError(
    /* [in] */ ModuleID moduleId,
    /* [in] */ mdMethodDef methodId,
    /* [in] */ FunctionID functionId,
    /* [in] */ HRESULT hrStatus){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::MovedReferences2(
    /* [in] */ ULONG cMovedObjectIDRanges,
    /* [size_is][in] */ ObjectID oldObjectIDRangeStart[],
    /* [size_is][in] */ ObjectID newObjectIDRangeStart[],
    /* [size_is][in] */ SIZE_T cObjectIDRangeLength[]){
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::SurvivingReferences2(
    /* [in] */ ULONG cSurvivingObjectIDRanges,
    /* [size_is][in] */ ObjectID objectIDRangeStart[],
    /* [size_is][in] */ SIZE_T cObjectIDRangeLength[]){
    return S_OK;
}

// ICorProfilerCallback5

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ConditionalWeakTableElementReferences(
    /* [in] */ ULONG cRootRefs,
    /* [size_is][in] */ ObjectID keyRefIds[],
    /* [size_is][in] */ ObjectID valueRefIds[],
    /* [size_is][in] */ GCHandleID rootIds[]){
    return S_OK;
}

// ICorProfilerCallback6

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::GetAssemblyReferences(
    _In_ const WCHAR *wszAssemblyPath,
    _In_ ICorProfilerAssemblyReferenceProvider *pAsmRefProvider) {
    ::SetEnvironmentVariable(L"CCoRawProfilerHook::GetAssemblyReferences", L"S_OK");
    return S_OK;
}

//ICorProfilerCallback7

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::ModuleInMemorySymbolsUpdated(
    _In_ ModuleID moduleId) {
    return S_OK;
}
