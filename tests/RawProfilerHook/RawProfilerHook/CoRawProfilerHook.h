// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "RawProfilerHook_i.h"

#include <cor.h>
#include <corprof.h>

class ATL_NO_VTABLE CCoRawProfilerHook
    : public ATL::CComObjectRootEx<ATL::CComMultiThreadModelNoCS>
    , public ATL::CComCoClass<CCoRawProfilerHook, &CLSID_CoRawProfilerHook>
    , public ICorProfilerCallback7
{
public:
    CCoRawProfilerHook()
    {
    }

    CCoRawProfilerHook(const CCoRawProfilerHook&) = delete;

    DECLARE_NO_REGISTRY()

    DECLARE_NOT_AGGREGATABLE(CCoRawProfilerHook)

    BEGIN_COM_MAP(CCoRawProfilerHook)
        COM_INTERFACE_ENTRY(ICorProfilerCallback)
        COM_INTERFACE_ENTRY(ICorProfilerCallback2)
        COM_INTERFACE_ENTRY(ICorProfilerCallback3)
        COM_INTERFACE_ENTRY(ICorProfilerCallback4)
        COM_INTERFACE_ENTRY(ICorProfilerCallback5)
        COM_INTERFACE_ENTRY(ICorProfilerCallback6)
        COM_INTERFACE_ENTRY(ICorProfilerCallback7)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    // ICorProfilerCallback

    HRESULT STDMETHODCALLTYPE Initialize(
        /* [in] */ IUnknown *pICorProfilerInfoUnk) override final;

    HRESULT STDMETHODCALLTYPE Shutdown(void) override final;

    HRESULT STDMETHODCALLTYPE AppDomainCreationStarted(
        /* [in] */ AppDomainID appDomainId) override final;

    HRESULT STDMETHODCALLTYPE AppDomainCreationFinished(
        /* [in] */ AppDomainID appDomainId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE AppDomainShutdownStarted(
        /* [in] */ AppDomainID appDomainId) override final;

    HRESULT STDMETHODCALLTYPE AppDomainShutdownFinished(
        /* [in] */ AppDomainID appDomainId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE AssemblyLoadStarted(
        /* [in] */ AssemblyID assemblyId) override final;

    HRESULT STDMETHODCALLTYPE AssemblyLoadFinished(
        /* [in] */ AssemblyID assemblyId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE AssemblyUnloadStarted(
        /* [in] */ AssemblyID assemblyId) override final;

    HRESULT STDMETHODCALLTYPE AssemblyUnloadFinished(
        /* [in] */ AssemblyID assemblyId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE ModuleLoadStarted(
        /* [in] */ ModuleID moduleId) override final;

    HRESULT STDMETHODCALLTYPE ModuleLoadFinished(
        /* [in] */ ModuleID moduleId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE ModuleUnloadStarted(
        /* [in] */ ModuleID moduleId) override final;

    HRESULT STDMETHODCALLTYPE ModuleUnloadFinished(
        /* [in] */ ModuleID moduleId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE ModuleAttachedToAssembly(
        /* [in] */ ModuleID moduleId,
        /* [in] */ AssemblyID AssemblyId) override final;

    HRESULT STDMETHODCALLTYPE ClassLoadStarted(
        /* [in] */ ClassID classId) override final;

    HRESULT STDMETHODCALLTYPE ClassLoadFinished(
        /* [in] */ ClassID classId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE ClassUnloadStarted(
        /* [in] */ ClassID classId) override final;

    HRESULT STDMETHODCALLTYPE ClassUnloadFinished(
        /* [in] */ ClassID classId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE FunctionUnloadStarted(
        /* [in] */ FunctionID functionId) override final;

    HRESULT STDMETHODCALLTYPE JITCompilationStarted(
        /* [in] */ FunctionID functionId,
        /* [in] */ BOOL fIsSafeToBlock) override final;

    HRESULT STDMETHODCALLTYPE JITCompilationFinished(
        /* [in] */ FunctionID functionId,
        /* [in] */ HRESULT hrStatus,
        /* [in] */ BOOL fIsSafeToBlock) override final;

    HRESULT STDMETHODCALLTYPE JITCachedFunctionSearchStarted(
        /* [in] */ FunctionID functionId,
        /* [out] */ BOOL *pbUseCachedFunction) override final;

    HRESULT STDMETHODCALLTYPE JITCachedFunctionSearchFinished(
        /* [in] */ FunctionID functionId,
        /* [in] */ COR_PRF_JIT_CACHE result) override final;

    HRESULT STDMETHODCALLTYPE JITFunctionPitched(
        /* [in] */ FunctionID functionId) override final;

    HRESULT STDMETHODCALLTYPE JITInlining(
        /* [in] */ FunctionID callerId,
        /* [in] */ FunctionID calleeId,
        /* [out] */ BOOL *pfShouldInline) override final;

    HRESULT STDMETHODCALLTYPE ThreadCreated(
        /* [in] */ ThreadID threadId) override final;

    HRESULT STDMETHODCALLTYPE ThreadDestroyed(
        /* [in] */ ThreadID threadId) override final;

    HRESULT STDMETHODCALLTYPE ThreadAssignedToOSThread(
        /* [in] */ ThreadID managedThreadId,
        /* [in] */ DWORD osThreadId) override final;

    HRESULT STDMETHODCALLTYPE RemotingClientInvocationStarted() override final;

    HRESULT STDMETHODCALLTYPE RemotingClientSendingMessage(
        /* [in] */ GUID *pCookie,
        /* [in] */ BOOL fIsAsync) override final;

    HRESULT STDMETHODCALLTYPE RemotingClientReceivingReply(
        /* [in] */ GUID *pCookie,
        /* [in] */ BOOL fIsAsync) override final;

    HRESULT STDMETHODCALLTYPE RemotingClientInvocationFinished() override final;

    HRESULT STDMETHODCALLTYPE RemotingServerReceivingMessage(
        /* [in] */ GUID *pCookie,
        /* [in] */ BOOL fIsAsync) override final;

    HRESULT STDMETHODCALLTYPE RemotingServerInvocationStarted() override final;

    HRESULT STDMETHODCALLTYPE RemotingServerInvocationReturned() override final;

    HRESULT STDMETHODCALLTYPE RemotingServerSendingReply(
        /* [in] */ GUID *pCookie,
        /* [in] */ BOOL fIsAsync) override final;

    HRESULT STDMETHODCALLTYPE UnmanagedToManagedTransition(
        /* [in] */ FunctionID functionId,
        /* [in] */ COR_PRF_TRANSITION_REASON reason) override final;

    HRESULT STDMETHODCALLTYPE ManagedToUnmanagedTransition(
        /* [in] */ FunctionID functionId,
        /* [in] */ COR_PRF_TRANSITION_REASON reason) override final;

    HRESULT STDMETHODCALLTYPE RuntimeSuspendStarted(
        /* [in] */ COR_PRF_SUSPEND_REASON suspendReason) override final;

    HRESULT STDMETHODCALLTYPE RuntimeSuspendFinished(void) override final;

    HRESULT STDMETHODCALLTYPE RuntimeSuspendAborted(void) override final;

    HRESULT STDMETHODCALLTYPE RuntimeResumeStarted(void) override final;

    HRESULT STDMETHODCALLTYPE RuntimeResumeFinished(void) override final;

    HRESULT STDMETHODCALLTYPE RuntimeThreadSuspended(
        /* [in] */ ThreadID threadId) override final;

    HRESULT STDMETHODCALLTYPE RuntimeThreadResumed(
        /* [in] */ ThreadID threadId) override final;

    HRESULT STDMETHODCALLTYPE MovedReferences(
        /* [in] */ ULONG cMovedObjectIDRanges,
        /* [size_is][in] */ ObjectID oldObjectIDRangeStart[],
        /* [size_is][in] */ ObjectID newObjectIDRangeStart[],
        /* [size_is][in] */ ULONG cObjectIDRangeLength[]) override final;

    HRESULT STDMETHODCALLTYPE ObjectAllocated(
        /* [in] */ ObjectID objectId,
        /* [in] */ ClassID classId) override final;

    HRESULT STDMETHODCALLTYPE ObjectsAllocatedByClass(
        /* [in] */ ULONG cClassCount,
        /* [size_is][in] */ ClassID classIds[],
        /* [size_is][in] */ ULONG cObjects[]) override final;

    HRESULT STDMETHODCALLTYPE ObjectReferences(
        /* [in] */ ObjectID objectId,
        /* [in] */ ClassID classId,
        /* [in] */ ULONG cObjectRefs,
        /* [size_is][in] */ ObjectID objectRefIds[]) override final;

    HRESULT STDMETHODCALLTYPE RootReferences(
        /* [in] */ ULONG cRootRefs,
        /* [size_is][in] */ ObjectID rootRefIds[]) override final;

    HRESULT STDMETHODCALLTYPE ExceptionThrown(
        /* [in] */ ObjectID thrownObjectId) override final;

    HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionEnter(
        /* [in] */ FunctionID functionId) override final;

    HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionLeave(void) override final;

    HRESULT STDMETHODCALLTYPE ExceptionSearchFilterEnter(
        /* [in] */ FunctionID functionId) override final;

    HRESULT STDMETHODCALLTYPE ExceptionSearchFilterLeave(void) override final;

    HRESULT STDMETHODCALLTYPE ExceptionSearchCatcherFound(
        /* [in] */ FunctionID functionId) override final;

    HRESULT STDMETHODCALLTYPE ExceptionOSHandlerEnter(
        /* [in] */ UINT_PTR __unused) override final;

    HRESULT STDMETHODCALLTYPE ExceptionOSHandlerLeave(
        /* [in] */ UINT_PTR __unused) override final;

    HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionEnter(
        /* [in] */ FunctionID functionId) override final;

    HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionLeave(void) override final;

    HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyEnter(
        /* [in] */ FunctionID functionId) override final;

    HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyLeave(void) override final;

    HRESULT STDMETHODCALLTYPE ExceptionCatcherEnter(
        /* [in] */ FunctionID functionId,
        /* [in] */ ObjectID objectId) override final;

    HRESULT STDMETHODCALLTYPE ExceptionCatcherLeave() override final;

    HRESULT STDMETHODCALLTYPE COMClassicVTableCreated(
        /* [in] */ ClassID wrappedClassId,
        /* [in] */ REFGUID implementedIID,
        /* [in] */ void *pVTable,
        /* [in] */ ULONG cSlots) override final;

    HRESULT STDMETHODCALLTYPE COMClassicVTableDestroyed(
        /* [in] */ ClassID wrappedClassId,
        /* [in] */ REFGUID implementedIID,
        /* [in] */ void *pVTable) override final;

    HRESULT STDMETHODCALLTYPE ExceptionCLRCatcherFound() override final;

    HRESULT STDMETHODCALLTYPE ExceptionCLRCatcherExecute() override final;

    // ICorProfilerCallback2

    HRESULT STDMETHODCALLTYPE ThreadNameChanged(
        /* [in] */ ThreadID threadId,
        /* [in] */ ULONG cchName,
        /* [annotation][in] */
        _In_reads_opt_(cchName)  WCHAR name[]) override final;

    HRESULT STDMETHODCALLTYPE GarbageCollectionStarted(
        /* [in] */ int cGenerations,
        /* [size_is][in] */ BOOL generationCollected[],
        /* [in] */ COR_PRF_GC_REASON reason) override final;

    HRESULT STDMETHODCALLTYPE SurvivingReferences(
        /* [in] */ ULONG cSurvivingObjectIDRanges,
        /* [size_is][in] */ ObjectID objectIDRangeStart[],
        /* [size_is][in] */ ULONG cObjectIDRangeLength[]) override final;

    HRESULT STDMETHODCALLTYPE GarbageCollectionFinished() override final;

    HRESULT STDMETHODCALLTYPE FinalizeableObjectQueued(
        /* [in] */ DWORD finalizerFlags,
        /* [in] */ ObjectID objectID) override final;

    HRESULT STDMETHODCALLTYPE RootReferences2(
        /* [in] */ ULONG cRootRefs,
        /* [size_is][in] */ ObjectID rootRefIds[],
        /* [size_is][in] */ COR_PRF_GC_ROOT_KIND rootKinds[],
        /* [size_is][in] */ COR_PRF_GC_ROOT_FLAGS rootFlags[],
        /* [size_is][in] */ UINT_PTR rootIds[]) override final;

    HRESULT STDMETHODCALLTYPE HandleCreated(
        /* [in] */ GCHandleID handleId,
        /* [in] */ ObjectID initialObjectId) override final;

    HRESULT STDMETHODCALLTYPE HandleDestroyed(
        /* [in] */ GCHandleID handleId) override final;

    // ICorProfilerCallback3

    HRESULT STDMETHODCALLTYPE InitializeForAttach(
        /* [in] */ IUnknown *pCorProfilerInfoUnk,
        /* [in] */ void *pvClientData,
        /* [in] */ UINT cbClientData) override final;

    HRESULT STDMETHODCALLTYPE ProfilerAttachComplete() override final;

    HRESULT STDMETHODCALLTYPE ProfilerDetachSucceeded() override final;

    // ICorProfilerCallback4

    HRESULT STDMETHODCALLTYPE ReJITCompilationStarted(
        /* [in] */ FunctionID functionId,
        /* [in] */ ReJITID rejitId,
        /* [in] */ BOOL fIsSafeToBlock) override final;

    HRESULT STDMETHODCALLTYPE GetReJITParameters(
        /* [in] */ ModuleID moduleId,
        /* [in] */ mdMethodDef methodId,
        /* [in] */ ICorProfilerFunctionControl *pFunctionControl) override final;

    HRESULT STDMETHODCALLTYPE ReJITCompilationFinished(
        /* [in] */ FunctionID functionId,
        /* [in] */ ReJITID rejitId,
        /* [in] */ HRESULT hrStatus,
        /* [in] */ BOOL fIsSafeToBlock) override final;

    HRESULT STDMETHODCALLTYPE ReJITError(
        /* [in] */ ModuleID moduleId,
        /* [in] */ mdMethodDef methodId,
        /* [in] */ FunctionID functionId,
        /* [in] */ HRESULT hrStatus) override final;

    HRESULT STDMETHODCALLTYPE MovedReferences2(
        /* [in] */ ULONG cMovedObjectIDRanges,
        /* [size_is][in] */ ObjectID oldObjectIDRangeStart[],
        /* [size_is][in] */ ObjectID newObjectIDRangeStart[],
        /* [size_is][in] */ SIZE_T cObjectIDRangeLength[]) override final;

    HRESULT STDMETHODCALLTYPE SurvivingReferences2(
        /* [in] */ ULONG cSurvivingObjectIDRanges,
        /* [size_is][in] */ ObjectID objectIDRangeStart[],
        /* [size_is][in] */ SIZE_T cObjectIDRangeLength[]) override final;

    // ICorProfilerCallback5

    HRESULT STDMETHODCALLTYPE ConditionalWeakTableElementReferences(
        /* [in] */ ULONG cRootRefs,
        /* [size_is][in] */ ObjectID keyRefIds[],
        /* [size_is][in] */ ObjectID valueRefIds[],
        /* [size_is][in] */ GCHandleID rootIds[]) override final;

    // ICorProfilerCallback6
    HRESULT STDMETHODCALLTYPE GetAssemblyReferences(
            _In_ const WCHAR *wszAssemblyPath,
            _In_ ICorProfilerAssemblyReferenceProvider *pAsmRefProvider);

    //ICorProfilerCallback7 methods
    HRESULT STDMETHODCALLTYPE ModuleInMemorySymbolsUpdated(
            _In_ ModuleID moduleId);



    static HRESULT FinalConstruct()
    {
        return S_OK;
    }

    static void FinalRelease()
    {
    }

    virtual ~CCoRawProfilerHook() {};
};

OBJECT_ENTRY_AUTO(CLSID_CoRawProfilerHook, CCoRawProfilerHook);