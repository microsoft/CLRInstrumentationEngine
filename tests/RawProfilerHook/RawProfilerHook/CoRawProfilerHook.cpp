// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CoRawProfilerHook.h"

// ICorProfilerCallback

HRESULT STDMETHODCALLTYPE CCoRawProfilerHook::Initialize(
    /* [in] */ IUnknown *pICorProfilerInfoUnk)
{
    ::SetEnvironmentVariable(L"CCoRawProfilerHook::Initialize", L"S_OK");

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

    WCHAR modName[MAX_PATH];
    HRESULT hr = S_OK;
    IfFailRet(m_pRealProfilerInfo->GetModuleInfo(moduleId, nullptr, MAX_PATH, nullptr, modName, nullptr));
    if (std::experimental::filesystem::path(modName).filename() != L"RawProfilerHook.Tests_x64.dll")
    {
        return S_OK;
    }

    m_testModule = moduleId;

    IfFailRet(m_pRealProfilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataImport, (IUnknown **)&m_metadataImport));
    IfFailRet(m_pRealProfilerInfo->GetModuleMetaData(moduleId, ofRead | ofWrite, IID_IMetaDataEmit2, (IUnknown **)&m_emit));
    IfFailRet(m_pRealProfilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataAssemblyImport, (IUnknown**)&m_assemblyImport));
    IfFailRet(m_pRealProfilerInfo->GetModuleMetaData(moduleId, ofRead | ofWrite, IID_IMetaDataAssemblyEmit, (IUnknown**)&m_assemblyEmit));

    IfFailRet(m_metadataImport->FindTypeDefByName(L"RawProfilerHook.Tests.TestRawProfilerMethodCreateBase", mdTokenNil, &m_testType));

    //In order to create a new method on an existing type we must get a valid RVA of the method.

    const COR_SIGNATURE voidSig[] = { IMAGE_CEE_CS_CALLCONV_HASTHIS, 0, ELEMENT_TYPE_VOID };
    ULONG rva = 0;
    mdMethodDef methodModuleLoadToken = 0;
    IfFailRet(m_metadataImport->FindMethod(m_testType, L"MethodModuleLoad", voidSig, sizeof(voidSig), &methodModuleLoadToken));
    IfFailRet(m_metadataImport->GetMethodProps(methodModuleLoadToken, nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr, &rva, nullptr));

    mdMethodDef intermediateModuleMethod = 0;
    IfFailRet(CreateNewMethod(m_testType, L"ModDefineModCreateBody", rva, &intermediateModuleMethod));

    mdMethodDef newToken = 0;
    //We emit the method body during jit
    IfFailRet(CreateNewMethod(m_testType, L"ModDefineJitCreateBody", rva, &newToken));

    //These setup the calls from MethodModuleLoad->ModDefineModCreateBody->DestinationMethod
    IfFailRet(EmitCallTargetMethodBody(methodModuleLoadToken, intermediateModuleMethod));
    IfFailRet(EmitCallTargetMethodBody(intermediateModuleMethod, L"DestinationMethod"));

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

    ModuleID moduleId;
    mdMethodDef methodToken;
    HRESULT hr = S_OK;
    IfFailRet(m_pRealProfilerInfo->GetFunctionInfo(functionId, nullptr, &moduleId, &methodToken));

    if (moduleId == m_testModule)
    {
        if (m_firstJit)
        {
            //In order to emit a method during JitCompilationStarted, you have to emit a brand new type

            //TODO This only works for desktop
            const BYTE publicKeyToken[] = { 0xB7, 0x7A, 0x5C, 0x56, 0x19, 0x34, 0xE0, 0x89 };
            ASSEMBLYMETADATA asmMeta = { 0 };
            ZeroMemory(&asmMeta, sizeof(asmMeta));
            asmMeta.usMajorVersion = 4;
            mdAssemblyRef mscorlibRef = 0;
            IfFailRet(m_assemblyEmit->DefineAssemblyRef(publicKeyToken, sizeof(publicKeyToken), L"mscorlib", &asmMeta, nullptr, 0, 0, &mscorlibRef));

            mdTypeRef objectRef = 0;
            IfFailRet(m_emit->DefineTypeRefByName(mscorlibRef, L"System.Object", &objectRef));

            IfFailRet(m_emit->DefineTypeDef(L"CustomTestType", tdClass, objectRef, nullptr, &m_createdTestType));

            IfFailRet(CreateNewStaticMethod(m_createdTestMethod, m_testType, L"JitDefinedJitBody", &m_createdTestMethod));
            IfFailRet(EmitCallTargetMethodBody(m_createdTestMethod, L"DestinationMethod"));
            m_firstJit = false;
        }

        const ULONG methodNameSize = 256;
        WCHAR methodName[methodNameSize];
        IfFailRet(m_metadataImport->GetMethodProps(methodToken, nullptr, methodName, methodNameSize, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));

        if (std::wstring(L"MethodJitCompilationStarted") == methodName)
        {
            IfFailRet(EmitCallTargetMethodBody(methodToken, m_createdTestMethod));
        }
        if (std::wstring(L"MethodModuleLoadThenJit") == methodName)
        {
            IfFailRet(EmitCallTargetMethodBody(methodToken, L"ModDefineJitCreateBody"));
        }
        else if (std::wstring(L"ModDefineJitCreateBody") == methodName)
        {
            IfFailRet(EmitCallTargetMethodBody(methodToken, L"DestinationMethod"));
        }
    }

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

HRESULT CCoRawProfilerHook::CreateNewMethod(mdTypeDef typeDef, LPWSTR methodName, ULONG rva, mdMethodDef* newMethod)
{
    HRESULT hr = S_OK;
    const COR_SIGNATURE voidSig[] = { IMAGE_CEE_CS_CALLCONV_HASTHIS, 0, ELEMENT_TYPE_VOID };

    IfFailRet(m_emit->DefineMethod(typeDef, methodName, mdPublic | mdHideBySig, voidSig, sizeof(voidSig), rva, miIL | miManaged, newMethod));

    return S_OK;
}

HRESULT CCoRawProfilerHook::CreateNewStaticMethod(mdTypeDef definingType, mdTypeDef parameterType, LPWSTR name, mdMethodDef* newMethod)
{
    HRESULT hr = S_OK;

    //The signature is public static void <MethodName>(<ParameterType> p1);
    COR_SIGNATURE sig[] = { IMAGE_CEE_CS_CALLCONV_DEFAULT, 1, ELEMENT_TYPE_VOID, ELEMENT_TYPE_CLASS, 0x0, 0x0, 0x0, 0x0};
    ULONG tokenSize = CorSigCompressToken(parameterType, &sig[4]);
    if (tokenSize < 1)
    {
        return E_FAIL;
    }
    IfFailRet(m_emit->DefineMethod(definingType, name, mdStatic | mdPublic | mdHideBySig, sig, sizeof(sig) - 4 + tokenSize, 0, miIL | miManaged, newMethod));

    return S_OK;
}

HRESULT CCoRawProfilerHook::EmitCallTargetMethodBody(mdMethodDef methodToken, LPWSTR targetMethod)
{
    HRESULT hr = S_OK;
    const COR_SIGNATURE voidSig[] = { IMAGE_CEE_CS_CALLCONV_HASTHIS, 0, ELEMENT_TYPE_VOID };

    mdMethodDef targetMethodToken;
    IfFailRet(m_metadataImport->FindMethod(m_testType, targetMethod, voidSig, sizeof(voidSig), &targetMethodToken));

    return EmitCallTargetMethodBody(methodToken, targetMethodToken);
}


HRESULT CCoRawProfilerHook::EmitCallTargetMethodBody(mdMethodDef methodToken, mdMethodDef targetMethod)
{
    HRESULT hr = S_OK;

    //These instructions are:
    // nop
    // ldarg.0
    // call <token>
    // nop
    // ret

    //This effectively allows us to chain from one method to another.
    //Note this works for both static methods that take 1 parameter or instance methods that take 0 parameters.

    BYTE instructions[] = { 0x0, 0x2, 0x28, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2a };
    DWORD methodSize = sizeof(instructions);

    ATL::CComPtr<IMethodMalloc> alloc;
    IfFailRet(m_pRealProfilerInfo->GetILFunctionBodyAllocator(m_testModule, &alloc));
    PVOID methodBody = alloc->Alloc(methodSize + sizeof(COR_ILMETHOD_FAT));
    if (methodBody == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    COR_ILMETHOD_FAT* methodPtr = reinterpret_cast<COR_ILMETHOD_FAT*>(methodBody);
    methodPtr->Size = sizeof(IMAGE_COR_ILMETHOD_FAT) / sizeof(DWORD);
    methodPtr->SetCodeSize(methodSize);
    methodPtr->SetFlags(CorILMethod_FatFormat);
    methodPtr->SetMaxStack(8);
    methodPtr->SetLocalVarSigTok(0);

    DWORD instructionIndex = sizeof(COR_ILMETHOD_FAT);

    //We substitute the token value for the target we want to call.
    *reinterpret_cast<DWORD*>(&instructions[3]) = targetMethod;

    memcpy(methodPtr->GetCode(), instructions, sizeof(instructions));
    IfFailRet(m_pRealProfilerInfo->SetILFunctionBody(m_testModule, methodToken, (LPCBYTE)methodBody));

    return S_OK;
}