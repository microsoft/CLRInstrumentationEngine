// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CorMethodMalloc.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CProfilerManager;

    class CCorProfilerInfoWrapper :
        public ICorProfilerInfo10
    {
    private:
        LONG m_refcount;

        CComPtr<ICorProfilerInfo> m_pRealCorProfilerInfo;
        CComPtr<ICorProfilerInfo2> m_pRealCorProfilerInfo2;
        CComPtr<ICorProfilerInfo3> m_pRealCorProfilerInfo3;
        CComPtr<ICorProfilerInfo4> m_pRealCorProfilerInfo4;
        CComPtr<ICorProfilerInfo5> m_pRealCorProfilerInfo5;
        CComPtr<ICorProfilerInfo6> m_pRealCorProfilerInfo6;
        CComPtr<ICorProfilerInfo7> m_pRealCorProfilerInfo7;
        CComPtr<ICorProfilerInfo8> m_pRealCorProfilerInfo8;
        CComPtr<ICorProfilerInfo9> m_pRealCorProfilerInfo9;
        CComPtr<ICorProfilerInfo10> m_pRealCorProfilerInfo10;

        // Non-addref'd back pointer the profiler manager.
        CProfilerManager* m_pProfilerManager;

        CComPtr<CCorMethodMalloc> m_pMethodMalloc;

    public:
        CCorProfilerInfoWrapper(_In_ CProfilerManager* pProfilerManager, _In_ ICorProfilerInfo* pRealCorProfilerInfo);

        // IUnknown
        // NOTE: custom IUnknown implementation to ensure null pointers on newer interfaces cause E_NOTIMPL.
    public:
        STDMETHOD(QueryInterface)(
            _In_ REFIID riid,
            _Out_ void **ppvObject
            );
        STDMETHOD_(ULONG, AddRef)();
        STDMETHOD_(ULONG, Release)();

        // ICorProfilerInfo info
    public:
        STDMETHOD(GetClassFromObject)(
            _In_ ObjectID objectId,
            _Out_ ClassID* pClassId
            );

        STDMETHOD(GetClassFromToken)(
            _In_ ModuleID moduleId,
            _In_ mdTypeDef typeDef,
            _Out_ ClassID* pClassId
            );

        STDMETHOD(GetCodeInfo)(
            _In_ FunctionID functionId,
            _Out_ LPCBYTE* pStart,
            _Out_ ULONG* pcSize
            );

        STDMETHOD(GetEventMask)(
            _Out_ DWORD* pdwEvents
            );

        STDMETHOD(GetFunctionFromIP)(
            _In_ LPCBYTE ip,
            _Out_ FunctionID* pFunctionId
            );

        STDMETHOD(GetFunctionFromToken)(
            _In_ ModuleID moduleId,
            _In_ mdToken token,
            _Out_ FunctionID* pFunctionId
            );

        STDMETHOD(GetHandleFromThread)(
            _In_ ThreadID threadId,
            _Out_ HANDLE* phThread
            );

        STDMETHOD(GetObjectSize)(
            _In_ ObjectID objectId,
            _Out_ ULONG* pcSize
            );

        STDMETHOD(IsArrayClass)(
            _In_ ClassID classId,
            _Out_ CorElementType* pBaseElemType,
            _Out_ ClassID* pBaseClassId,
            _Out_ ULONG* pcRank
            );

        STDMETHOD(GetThreadInfo)(
            _In_ ThreadID threadId,
            _Out_ DWORD* pdwWin32ThreadId
            );

        STDMETHOD(GetCurrentThreadID)(
            _Out_ ThreadID* pThreadId
            );

        STDMETHOD(GetClassIDInfo)(
            _In_ ClassID classId,
            _Out_ ModuleID* pModuleId,
            _Out_ mdTypeDef* pTypeDefToken
            );

        STDMETHOD(GetFunctionInfo)(
            _In_ FunctionID functionId,
            _Out_ ClassID* pClassId,
            _Out_ ModuleID* pModuleId,
            _Out_ mdToken* pToken
            );

        STDMETHOD(SetEventMask)(
            _In_ DWORD dwEvents
            );

        STDMETHOD(SetEnterLeaveFunctionHooks)(
            _In_ FunctionEnter* pFuncEnter,
            _In_ FunctionLeave* pFuncLeave,
            _In_ FunctionTailcall* pFuncTailcall
            );

        STDMETHOD(SetFunctionIDMapper)(
            _In_ FunctionIDMapper* pFunc
            );

        STDMETHOD(GetTokenAndMetaDataFromFunction)(
            _In_ FunctionID functionId,
            _In_ REFIID riid,
            _Out_ IUnknown **ppImport,
            _Out_ mdToken* pToken
            );

        STDMETHOD(GetModuleInfo)(
            _In_ ModuleID moduleId,
            _Out_opt_ LPCBYTE* ppBaseLoadAddress,
            _In_ ULONG cchName,
            _Out_ ULONG* pcchName,
            _Out_writes_to_(cchName, *pcchName) WCHAR szName[],
            _Out_opt_ AssemblyID* pAssemblyId
            );

        STDMETHOD(GetModuleMetaData)(
            _In_ ModuleID moduleId,
            _In_ DWORD dwOpenFlags,
            _In_ REFIID riid,
            _Out_ IUnknown **ppOut
            );

        STDMETHOD(GetILFunctionBody)(
            _In_ ModuleID moduleId,
            _In_ mdMethodDef methodId,
            _Out_ LPCBYTE* ppMethodHeader,
            _Out_ ULONG* pcbMethodSize
            );

        STDMETHOD(GetILFunctionBodyAllocator)(
            _In_ ModuleID moduleId,
            _Out_ IMethodMalloc **ppMalloc
            );

        STDMETHOD(SetILFunctionBody)(
            _In_ ModuleID moduleId,
            _In_ mdMethodDef methodid,
            _In_ LPCBYTE pbNewILMethodHeader
            );

        STDMETHOD(GetAppDomainInfo)(
            _In_ AppDomainID appDomainId,
            _In_ ULONG cchName,
            _Out_ ULONG* pcchName,
            _Out_writes_to_(cchName, *pcchName) WCHAR szName[],
            _Out_ ProcessID* pProcessId
            );

        STDMETHOD(GetAssemblyInfo)(
            _In_ AssemblyID assemblyId,
            _In_ ULONG cchName,
            _Out_opt_ ULONG* pcchName,
            _Out_writes_bytes_to_(cchName, *pcchName) WCHAR szName[],
            _Out_opt_ AppDomainID* pAppDomainId,
            _Out_opt_ ModuleID* pModuleId
            );

        STDMETHOD(SetFunctionReJIT)(
            _In_ FunctionID functionId
            );

        STDMETHOD(ForceGC)();

        STDMETHOD(SetILInstrumentedCodeMap)(
            _In_ FunctionID functionId,
            _In_ BOOL fStartJit,
            _In_ ULONG cILMapEntries,
            _In_reads_(cILMapEntries) COR_IL_MAP rgILMapEntries[]
            );

        STDMETHOD(GetInprocInspectionInterface)(
            _Out_ IUnknown **ppicd
            );

        STDMETHOD(GetInprocInspectionIThisThread)(
            _Out_ IUnknown **ppicd
            );

        STDMETHOD(GetThreadContext)(
            _In_ ThreadID threadId,
            _Out_ ContextID* pContextId
            );

        STDMETHOD(BeginInprocDebugging)(
            _In_ BOOL fThisThreadOnly,
            _Out_ DWORD* pdwProfilerContext
            );

        STDMETHOD(EndInprocDebugging)(
            _In_ DWORD dwProfilerContext
            );

        STDMETHOD(GetILToNativeMapping)(
            _In_ FunctionID functionId,
            _In_ ULONG32 cMap,
            _Out_ ULONG32* pcMap,
            _Out_writes_(cMap) COR_DEBUG_IL_TO_NATIVE_MAP map[]
            );

        // ICorProfilerInfo2
    public:
        STDMETHOD(DoStackSnapshot)(
            _In_ ThreadID thread,
            _In_ StackSnapshotCallback *callback,
            _In_ ULONG32 infoFlags,
            _In_ void *clientData,
            _In_reads_bytes_(contextSize) BYTE context[],
            _In_ ULONG32 contextSize
            );

        STDMETHOD(SetEnterLeaveFunctionHooks2)(
            _In_ FunctionEnter2* pFuncEnter,
            _In_ FunctionLeave2* pFuncLeave,
            _In_ FunctionTailcall2* pFuncTailcall
            );

        STDMETHOD(GetFunctionInfo2)(
            _In_ FunctionID funcId,
            _In_ COR_PRF_FRAME_INFO frameInfo,
            _Out_opt_ ClassID* pClassId,
            _Out_opt_ ModuleID* pModuleId,
            _Out_opt_ mdToken* pToken,
            _In_ ULONG32 cTypeArgs,
            _Out_opt_ ULONG32* pcTypeArgs,
            _Out_writes_bytes_to_opt_(cTypeArgs, *pcTypeArgs) ClassID typeArgs[]
            );

        STDMETHOD(GetStringLayout)(
            _Out_ ULONG* pBufferLengthOffset,
            _Out_ ULONG* pStringLengthOffset,
            _Out_ ULONG* pBufferOffset
            );

        STDMETHOD(GetClassLayout)(
            _In_ ClassID classID,
            _Inout_updates_(cFieldOffset) COR_FIELD_OFFSET rFieldOffset[],
            _In_ ULONG cFieldOffset,
            _Out_ ULONG* pcFieldOffset,
            _Out_ ULONG* pulClassSize
            );

        STDMETHOD(GetClassIDInfo2)(
            _In_ ClassID classId,
            _Out_ ModuleID* pModuleId,
            _Out_ mdTypeDef* pTypeDefToken,
            _Out_ ClassID* pParentClassId,
            _In_ ULONG32 cNumTypeArgs,
            _Out_ ULONG32* pcNumTypeArgs,
            _Out_writes_(cNumTypeArgs) ClassID typeArgs[]
            );

        STDMETHOD(GetCodeInfo2)(
            _In_ FunctionID functionID,
            _In_ ULONG32 cCodeInfos,
            _Out_ ULONG32* pcCodeInfos,
            _Out_writes_(cCodeInfos) COR_PRF_CODE_INFO codeInfos[]
            );

        STDMETHOD(GetClassFromTokenAndTypeArgs)(
            _In_ ModuleID moduleID,
            _In_ mdTypeDef typeDef,
            _In_ ULONG32 cTypeArgs,
            _Out_writes_(cTypeArgs) ClassID typeArgs[],
            _Out_ ClassID* pClassID
            );

        STDMETHOD(GetFunctionFromTokenAndTypeArgs)(
            _In_  ModuleID moduleID,
            _In_ mdMethodDef funcDef,
            _In_ ClassID classId,
            _In_ ULONG32 cTypeArgs,
            _Out_writes_(cTypeArgs) ClassID typeArgs[],
            _Out_ FunctionID* pFunctionID
            );

        STDMETHOD(EnumModuleFrozenObjects)(
            _In_  ModuleID moduleID,
            _Out_ ICorProfilerObjectEnum **ppEnum
            );

        STDMETHOD(GetArrayObjectInfo)(
            _In_  ObjectID objectId,
            _In_  ULONG32 cDimensions,
            _Out_writes_(cDimensions) ULONG32 pDimensionSizes[],
            _Out_writes_(cDimensions)  int pDimensionLowerBounds[],
            _Out_ BYTE **ppData
            );

        STDMETHOD(GetBoxClassLayout)(
            _In_  ClassID classId,
            _Out_ ULONG32* pBufferOffset
            );

        STDMETHOD(GetThreadAppDomain)(
            _In_ ThreadID threadId,
            _Out_ AppDomainID* pAppDomainId
            );

        STDMETHOD(GetRVAStaticAddress)(
            _In_ ClassID classId,
            _In_ mdFieldDef fieldToken,
            _Out_ void **ppAddress
            );

        STDMETHOD(GetAppDomainStaticAddress)(
            _In_ ClassID classId,
            _In_ mdFieldDef fieldToken,
            _In_ AppDomainID appDomainId,
            _Out_ void **ppAddress
            );

        STDMETHOD(GetThreadStaticAddress)(
            _In_ ClassID classId,
            _In_ mdFieldDef fieldToken,
            _In_ ThreadID threadId,
            _Out_ void **ppAddress
            );

        STDMETHOD(GetContextStaticAddress)(
            _In_ ClassID classId,
            _In_ mdFieldDef fieldToken,
            _In_ ContextID contextId,
            _Out_ void **ppAddress
            );

        STDMETHOD(GetStaticFieldInfo)(
            _In_ ClassID classId,
            _In_ mdFieldDef fieldToken,
            _Out_ COR_PRF_STATIC_TYPE* pFieldInfo
            );

        STDMETHOD(GetGenerationBounds)(
            _In_ ULONG cObjectRanges,
            _Out_ ULONG* pcObjectRanges,
            _Out_writes_(cObjectRanges) COR_PRF_GC_GENERATION_RANGE ranges[]
            );

        STDMETHOD(GetObjectGeneration)(
            _In_ ObjectID objectId,
            _Out_ COR_PRF_GC_GENERATION_RANGE *range
            );

        STDMETHOD(GetNotifiedExceptionClauseInfo)(
            _Out_ COR_PRF_EX_CLAUSE_INFO* pinfo
            );

        // ICorProfilerInfo3
    public:
        STDMETHOD(EnumJITedFunctions)(
            _Out_ ICorProfilerFunctionEnum **ppEnum);

        STDMETHOD(RequestProfilerDetach)(
            _In_ DWORD dwExpectedCompletionMilliseconds
            );

        STDMETHOD(SetFunctionIDMapper2)(
            _In_ FunctionIDMapper2* pFunc,
            _In_ void *clientData
            );

        STDMETHOD(GetStringLayout2)(
            _Out_ ULONG* pStringLengthOffset,
            _Out_ ULONG* pBufferOffset
            );

        STDMETHOD(SetEnterLeaveFunctionHooks3)(
            _In_ FunctionEnter3* pFuncEnter3,
            _In_ FunctionLeave3* pFuncLeave3,
            _In_ FunctionTailcall3* pFuncTailcall3
            );

        STDMETHOD(SetEnterLeaveFunctionHooks3WithInfo)(
            _In_ FunctionEnter3WithInfo* pFuncEnter3WithInfo,
            _In_ FunctionLeave3WithInfo* pFuncLeave3WithInfo,
            _In_ FunctionTailcall3WithInfo* pFuncTailcall3WithInfo
            );

        STDMETHOD(GetFunctionEnter3Info)(
            _In_ FunctionID functionId,
            _In_ COR_PRF_ELT_INFO eltInfo,
            _Out_ COR_PRF_FRAME_INFO* pFrameInfo,
            _Inout_ ULONG* pcbArgumentInfo,
            _Out_writes_(*pcbArgumentInfo) COR_PRF_FUNCTION_ARGUMENT_INFO* pArgumentInfo
            );

        STDMETHOD(GetFunctionLeave3Info)(
            _In_ FunctionID functionId,
            _In_ COR_PRF_ELT_INFO eltInfo,
            _Out_ COR_PRF_FRAME_INFO* pFrameInfo,
            _Out_ COR_PRF_FUNCTION_ARGUMENT_RANGE* pRetvalRange
            );

        STDMETHOD(GetFunctionTailcall3Info)(
            _In_ FunctionID functionId,
            _In_ COR_PRF_ELT_INFO eltInfo,
            _Out_ COR_PRF_FRAME_INFO* pFrameInfo
            );

        STDMETHOD(EnumModules)(
            _In_ ICorProfilerModuleEnum **ppEnum
            );

        STDMETHOD(GetRuntimeInformation)(
            _Out_opt_ USHORT* pClrInstanceId,
            _Out_opt_ COR_PRF_RUNTIME_TYPE* pRuntimeType,
            _Out_opt_ USHORT* pMajorVersion,
            _Out_opt_ USHORT* pMinorVersion,
            _Out_opt_ USHORT* pBuildNumber,
            _Out_opt_ USHORT* pQFEVersion,
            _In_ ULONG cchVersionString,
            _Out_opt_ ULONG* pcchVersionString,
            _Out_writes_opt_(cchVersionString) WCHAR szVersionString[]
            );

        STDMETHOD(GetThreadStaticAddress2)(
            _In_ ClassID classId,
            _In_ mdFieldDef fieldToken,
            _In_ AppDomainID appDomainId,
            _In_ ThreadID threadId,
            _Out_ void **ppAddress
            );

        STDMETHOD(GetAppDomainsContainingModule)(
            _In_ ModuleID moduleId,
            _In_ ULONG32 cAppDomainIds,
            _Out_ ULONG32* pcAppDomainIds,
            _Out_writes_(cAppDomainIds) AppDomainID appDomainIds[]
            );

        STDMETHOD(GetModuleInfo2)(
            _In_ ModuleID moduleId,
            _Out_opt_ LPCBYTE* ppBaseLoadAddress,
            _In_ ULONG cchName,
            _Out_opt_ ULONG* pcchName,
            _Out_writes_bytes_to_(cchName, *pcchName) WCHAR szName[],
            _Out_opt_ AssemblyID* pAssemblyId,
            _Out_opt_ DWORD* pdwModuleFlags
            );

        // ICorProfilerInfo4
    public:

        STDMETHOD(EnumThreads)(
            _Out_ ICorProfilerThreadEnum **ppEnum
            );

        STDMETHOD(InitializeCurrentThread)();

        STDMETHOD(RequestReJIT)(
            _In_ ULONG cFunctions,
            _In_reads_(cFunctions) ModuleID moduleIds[],
            _In_reads_(cFunctions) mdMethodDef methodIds[]
            );

        STDMETHOD(RequestRevert)(
            _In_ ULONG cFunctions,
            _In_reads_(cFunctions) ModuleID moduleIds[],
            _In_reads_(cFunctions) mdMethodDef methodIds[],
            _Out_writes_(cFunctions) HRESULT status[]
            );

        STDMETHOD(GetCodeInfo3)(
            _In_ FunctionID functionID,
            _In_ ReJITID reJitId,
            _In_ ULONG32 cCodeInfos,
            _Out_ ULONG32* pcCodeInfos,
            _Out_writes_(cCodeInfos) COR_PRF_CODE_INFO codeInfos[]
            );

        STDMETHOD(GetFunctionFromIP2)(
            _In_ LPCBYTE ip,
            _Out_ FunctionID* pFunctionId,
            _Out_ ReJITID* pReJitId
            );

        STDMETHOD(GetReJITIDs)(
            _In_ FunctionID functionId,
            _In_ ULONG cReJitIds,
            _Out_ ULONG* pcReJitIds,
            _Out_writes_(cReJitIds) ReJITID reJitIds[]
            );

        STDMETHOD(GetILToNativeMapping2)(
            _In_ FunctionID functionId,
            _In_ ReJITID reJitId,
            _In_ ULONG32 cMap,
            _Out_  ULONG32* pcMap,
            _Out_writes_(cMap) COR_DEBUG_IL_TO_NATIVE_MAP map[]
            );

        STDMETHOD(EnumJITedFunctions2)(
            _Out_ ICorProfilerFunctionEnum **ppEnum
            );

        STDMETHOD(GetObjectSize2)(
            _In_ ObjectID objectId,
            _Out_ SIZE_T* pcSize
            );

        // ICorProfilerInfo5
    public:
        STDMETHOD(GetEventMask2)(
            _Out_ DWORD *pdwEventsLow,
            _Out_ DWORD *pdwEventsHigh
            );

        STDMETHOD(SetEventMask2)(
            _In_ DWORD dwEventsLow,
            _In_ DWORD dwEventsHigh
            );

        // ICorProfilerInfo6
    public:
        STDMETHOD(EnumNgenModuleMethodsInliningThisMethod)(
            _In_ ModuleID inlinersModuleId,
            _In_ ModuleID inlineeModuleId,
            _In_ mdMethodDef inlineeMethodId,
            _Out_ BOOL *incompleteData,
            _Out_ ICorProfilerMethodEnum **ppEnum
            );

        // ICorProfilerInfo7
    public:
        STDMETHOD(ApplyMetaData)(
            _In_ ModuleID moduleId
            );

        STDMETHOD(GetInMemorySymbolsLength)(
            _In_ ModuleID moduleId,
            _Out_ DWORD *pCountSymbolBytes
            );

        STDMETHOD(ReadInMemorySymbols)(
            _In_ ModuleID moduleId,
            _In_ DWORD symbolsReadOffset,
            _Out_writes_(countSymbolBytes)  BYTE *pSymbolBytes,
            _In_ DWORD countSymbolBytes,
            _Out_ DWORD *pCountSymbolBytesRead
            );

    // ICorProfilerInfo8
    public:
        STDMETHOD(IsFunctionDynamic)(
            _In_ FunctionID functionId,
            _Out_ BOOL *isDynamic);

        STDMETHOD(GetFunctionFromIP3)(
            _In_ LPCBYTE ip,
            _Out_ FunctionID *functionId,
            _Out_ ReJITID *pReJitId);

        STDMETHOD(GetDynamicFunctionInfo)(
            _In_ FunctionID functionId,
            _Out_ ModuleID *moduleId,
            _Out_ PCCOR_SIGNATURE *ppvSig,
            _Out_ ULONG *pbSig,
            _In_ ULONG cchName,
            _Out_ ULONG *pcchName,
            _Out_writes_(cchName) WCHAR wszName[]);

    // ICorProfilerInfo9
    public:
        STDMETHOD(GetNativeCodeStartAddresses)(
            _In_ FunctionID functionID,
            _In_ ReJITID reJitId,
            _In_ ULONG32 cCodeStartAddresses,
            _Out_ ULONG32 *pcCodeStartAddresses,
            _Out_writes_(cCodeStartAddresses) UINT_PTR codeStartAddresses[]);

        STDMETHOD(GetILToNativeMapping3)(
            _In_ UINT_PTR pNativeCodeStartAddress,
            _In_ ULONG32 cMap,
            _Out_ ULONG32 *pcMap,
            _Out_writes_(cMap) COR_DEBUG_IL_TO_NATIVE_MAP map[]);

        STDMETHOD(GetCodeInfo4)(
            _In_ UINT_PTR pNativeCodeStartAddress,
            _In_ ULONG32 cCodeInfos,
            _Out_ ULONG32 *pcCodeInfos,
            _Out_writes_(cCodeInfos) COR_PRF_CODE_INFO codeInfos[]);

    // ICorProfilerInfo10
    public:
        STDMETHOD(EnumerateObjectReferences)(
            _In_ ObjectID objectId,
            _In_ ObjectReferenceCallback callback,
            _In_ void *clientData);

        STDMETHOD(IsFrozenObject)(
            _In_ ObjectID objectId,
            _Out_ BOOL *pbFrozen);

        STDMETHOD(GetLOHObjectSizeThreshold)(
            _Out_ DWORD *pThreshold);

        STDMETHOD(RequestReJITWithInliners)(
            _In_ DWORD dwRejitFlags,
            _In_ ULONG cFunctions,
            _In_reads_(cFunctions) ModuleID moduleIds[],
            _In_reads_(cFunctions) mdMethodDef methodIds[]);

        STDMETHOD(SuspendRuntime)(void);

        STDMETHOD(ResumeRuntime)(void);
    };
}