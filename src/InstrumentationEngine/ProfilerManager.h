// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "InstrumentationMethod.h"
#include "AppDomainCollection.h"
#include "ClrVersion.h"
#include "MethodInfo.h"
#include "../ExtensionsHostLib/CExtensionsHost.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    typedef vector<pair<shared_ptr<CInstrumentationMethod>, DWORD>> TInstrumentationMethodsCollection;

    const GUID CLSID_CProfilerManager = { 0x324F817A, 0x7420, 0x4E6D,{ 0xB3, 0xC1, 0x14, 0x3f, 0xBE, 0xD6, 0xD8, 0x55 } };

    class __declspec(uuid("324F817A-7420-4E6D-B3C1-143FBED6D855"))
    CProfilerManager :
                     public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>,
#ifndef PLATFORM_UNIX
                     public ATL::CComCoClass<CProfilerManager, &__uuidof(CProfilerManager)>,
#else
                     public ATL::CComCoClass<CProfilerManager, &CLSID_CProfilerManager>,
#endif
                     public IProfilerManager,
                     public IProfilerManager2,
                     public IProfilerManager3,
                     public IProfilerManager4,
                     public IProfilerManager5,
                     public IProfilerManagerLogging,
                     public ICorProfilerCallback7
    {
    private:
        // currently, the profiler manager does not support in-proc sxs.
        // If this is non-null during the initialize call, the profiler attach is rejected.
        static CProfilerManager* s_profilerManagerInstance;

        // Instrumentation methods can disabling profiling on this process before it starts by calling this during initialize.
        bool m_bProfilingDisabled;

        // Critical section that ensures sequencing of AppDomain unloading
        CRITICAL_SECTION m_csForAppDomains;

        // Critical section that protects data structures in this object that are manipulated on multiple threads
        CRITICAL_SECTION m_cs;

        // Critical section that synchronized JIT compilation callbacks to avoid
        // data corruption when two threads attempt to compile the same method simultaneously
        CRITICAL_SECTION m_csForJIT;

        // Consider: expose this in the api? I'd rather there be a clr api for this instead.
        ClrVersion m_attachedClrVersion;

        COR_PRF_RUNTIME_TYPE m_runtimeType;

#ifndef PLATFORM_UNIX
        // Free threaded marshaller instance.
        CComPtr<IMarshal> m_pFTM;
#endif

        // Pointer to the single profiler host instance
        CComPtr<IProfilerManagerHost> m_profilerManagerHost;

        // Pointer to the real ICorProfilerInfo from the clr
        CComPtr<ICorProfilerInfo> m_pRealProfilerInfo;

        // The wrapped ICorProfilerInfo implementation that allows for instrumentation method cooperation
        CComPtr<ICorProfilerInfo> m_pWrappedProfilerInfo;

        // list of configuration file paths
        vector<ATL::CComBSTR> m_configFilePaths;

        // list of loaded instrumentation method guids
        std::vector<GUID> m_instrumentationMethodGuids;

        // Event that is used to synchronize the callback thread with the loading of the config path xml objects
        CHandle m_hLoadInstrumentationMethodsCompleteEvent;

        // list of instrumentation methods
        TInstrumentationMethodsCollection m_instrumentationMethods;

        // Event mask passed by raw handlers and instrumentation methods during initialize. This is bitwise union of the event mask
        // from each of these sources. Note that this lives here instead of profiler manager because of the GetEventMask method
        // that this object must return.
        DWORD m_dwEventMask;

        // High event mask used if raw handlers call SetEventMask2
        DWORD m_dwEventMaskHigh;

        // True if the call to Initialize is still on the stack
        bool m_bIsInInitialize;

        // True if we are currently initializing instrumentation method
        bool m_bIsInitializingInstrumentationMethod;

        // Flags set by instrumentation method
        DWORD m_dwInstrumentationMethodFlags;

        // If flag set - only trusted instrumentation methods can be loaded
        bool m_bValidateCodeSignature;

        // private nested class for holding the raw pointers to the callbacks.
        // This just makes it easy to have the destructor do Release while allowing for
        // caching the qi results.
        class CProfilerCallbackHolder
        {
        public:
            CComPtr<ICorProfilerCallback> m_CorProfilerCallback;
            CComPtr<ICorProfilerCallback2> m_CorProfilerCallback2;
            CComPtr<ICorProfilerCallback3> m_CorProfilerCallback3;
            CComPtr<ICorProfilerCallback4> m_CorProfilerCallback4;
			CComPtr<ICorProfilerCallback5> m_CorProfilerCallback5;
			CComPtr<ICorProfilerCallback6> m_CorProfilerCallback6;
			CComPtr<ICorProfilerCallback7> m_CorProfilerCallback7;

            IUnknown* GetMemberForInterface(REFGUID guidInterface)
            {
                if (guidInterface == __uuidof(ICorProfilerCallback))
                {
                    return m_CorProfilerCallback;
                }
                else if (guidInterface == __uuidof(ICorProfilerCallback2))
                {
                    return m_CorProfilerCallback2;
                }
                else if (guidInterface == __uuidof(ICorProfilerCallback3))
                {
                    return m_CorProfilerCallback3;
                }
                else if (guidInterface == __uuidof(ICorProfilerCallback4))
                {
                    return m_CorProfilerCallback4;
                }
				else if (guidInterface == __uuidof(ICorProfilerCallback5))
				{
					return m_CorProfilerCallback5;
				}
				else if (guidInterface == __uuidof(ICorProfilerCallback6))
				{
					return m_CorProfilerCallback6;
				}
				else if (guidInterface == __uuidof(ICorProfilerCallback7))
				{
					return m_CorProfilerCallback7;
				}
				else
                {
                    CLogging::LogError(_T("CProfilerCallbackHolder::GetMemberForInterface Bogus interface member requested"));
                    return NULL;
                }
            }
        };

        // This is the client that asked to receive the raw cor profiler event
        // model rather than using the simplified instrumentation method model. .
        unique_ptr<CProfilerCallbackHolder> m_profilerCallbackHolder;

        CComPtr<CAppDomainCollection> m_pAppDomainCollection;

    public:
        CProfilerManager();
        ~CProfilerManager();

        // Public non interface methods
    public:
#ifndef PLATFORM_UNIX
        DECLARE_PROTECT_FINAL_CONSTRUCT()
        DECLARE_REGISTRY_RESOURCEID(IDR_INSTRUMENTATIONENGINE)
#endif

        HRESULT FinalConstruct();
        void FinalRelease();

        void SetIsInInitialize(_In_ bool val);
        bool GetIsInInitialize() const;

        void SetIsInitializingInstrumentationMethod(_In_ bool val);
        bool GetIsInitializingInstrumentationMethod() const;
        void SetInitializingInstrumentationMethodFlags(_In_ DWORD dwFlags);
        DWORD GetInitializingInstrumentationMethodFlags() const;

        static HRESULT GetProfilerManagerInstance(_Out_ CProfilerManager** ppProfilerManager);

        HRESULT GetEventMask(_Out_ DWORD* dwEventMask);
        HRESULT SetEventMask(DWORD dwEventMask);
        HRESULT GetEventMask2(_Out_ DWORD* dwEventMaskLow, _Out_ DWORD* dwEventMaskHigh);
        HRESULT SetEventMask2(_In_ DWORD dwEventMaskLow, _In_ DWORD dwEventMaskHigh);

        ClrVersion GetAttachedClrVersion();

        // Returns the raw unwrapped profiler info instance.
        HRESULT GetRealCorProfilerInfo(ICorProfilerInfo** ppRealProfilerInfo);

        HRESULT CreateMethodInfo(
            _In_ FunctionID functionId,
            _Out_ CMethodInfo** ppMethodInfo
        );

        HRESULT CreateNewMethodInfo(_In_ FunctionID functionId, _Out_ CMethodInfo** ppMethodInfo);

        CLogging* GetLogging();

        // IUnknown
    public:
        BEGIN_COM_MAP(CProfilerManager)
            COM_INTERFACE_ENTRY(IProfilerManager)
            COM_INTERFACE_ENTRY(IProfilerManager2)
            COM_INTERFACE_ENTRY(IProfilerManager3)
            COM_INTERFACE_ENTRY(IProfilerManager4)
            COM_INTERFACE_ENTRY(IProfilerManager5)
            COM_INTERFACE_ENTRY(IProfilerManagerLogging)
            COM_INTERFACE_ENTRY(ICorProfilerCallback)
            COM_INTERFACE_ENTRY(ICorProfilerCallback2)
            COM_INTERFACE_ENTRY(ICorProfilerCallback3)
            COM_INTERFACE_ENTRY(ICorProfilerCallback4)
            COM_INTERFACE_ENTRY(ICorProfilerCallback5)
            COM_INTERFACE_ENTRY(ICorProfilerCallback6)
            COM_INTERFACE_ENTRY(ICorProfilerCallback7)
#ifndef PLATFORM_UNIX
            COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pFTM.p)
#endif
        END_COM_MAP()

        // Private Helpers
    private:
        DWORD CalculateEventMask(DWORD dwAdditionalFlags);

        // The CLR doesn't initialize com before calling the profiler, and the profiler manager cannot do so itself
        // as that would screw up the com state for the application thread. This thread allows the profiler manager
        // to co create a free threaded version of msxml on a thread that it owns to avoid this.
        //
        // lpParameter points to the this pointer so m_configFilePaths can be accessed. This is safe because the thread proc
        // blocks until this is complete.
        static DWORD WINAPI InstrumentationMethodThreadProc(
            _In_  LPVOID lpParameter
            );


        HRESULT LoadInstrumentationMethods(_In_ BSTR bstrConfigPath);

        HRESULT ApplyInstrumentationMethodInstrumentation(_In_ CMethodInfo* pMethodInfo);

        HRESULT DetermineClrVersion();

        // Set the default event mask. This is union'd with the event mask from instrumentation methods and the host.
        static DWORD GetDefaultEventMask();

        //Template method to either QI or simple copy the InstrumentionMethod ComPtr. These methods support CopyInstrumentationMethods.
        template<typename TInterfaceType>
        HRESULT GetInterfaceType(CComPtr<IInstrumentationMethod> pRawInstrumentationMethod, TInterfaceType** pInterface)
        {
            return pRawInstrumentationMethod->QueryInterface(__uuidof(TInterfaceType), (void**)pInterface);
        }

        template<>
        HRESULT GetInterfaceType(CComPtr<IInstrumentationMethod> pRawInstrumentationMethod, IInstrumentationMethod** pInterface)
        {
            return pRawInstrumentationMethod.CopyTo(pInterface);
        }

        template<typename TInterfaceType>
        HRESULT CopyInstrumentationMethods(std::vector<CComPtr<TInterfaceType>>& callbackVector)
        {
            CCriticalSectionHolder lock(&m_cs);
            HRESULT hr;
            // Holding the lock during the callback functions is dangerous since rentrant
            // events and calls will block. Copy the collection under the lock, then release it and finally call the callbacks
            for (auto pCurrInstrumentationMethod : m_instrumentationMethods)
            {
                CComPtr<IInstrumentationMethod> pRawInstrumentationMethod;
                IfFailRet(pCurrInstrumentationMethod.first->GetRawInstrumentationMethod(&pRawInstrumentationMethod));

                CComPtr<TInterfaceType> pInterface;
                if (SUCCEEDED(GetInterfaceType(pRawInstrumentationMethod, &pInterface)))
                {
                    callbackVector.push_back(pInterface);
                }
            }

            return S_OK;
        }

        template<typename TInterfaceType, typename TReturnType, typename... TParameters>
        HRESULT SendEventToInstrumentationMethods(TReturnType(__stdcall TInterfaceType::*method)(TParameters...), TParameters... parameters)
        {
            HRESULT hr = S_OK;

            vector<CComPtr<TInterfaceType>> callbackVector;
            IfFailRet(CopyInstrumentationMethods(callbackVector));

            // Send event to instrumentation methods
            for (CComPtr<TInterfaceType> pInstrumentationMethod : callbackVector)
            {
                CLogging::LogMessage(_T("Sending event to Instrumentation Method"));

                hr = (pInstrumentationMethod->*method)(parameters...);

                CLogging::LogMessage(_T("Finished Sending event to Instrumentation Method. hr=%04x"), hr);
            }

            return hr;
        }

        template<typename TFunc, typename... TParameters>
        HRESULT ForEachInstrumentationMethod(TFunc func, TParameters... parameters)
        {
            HRESULT hr = S_OK;
            vector<CComPtr<IInstrumentationMethod>> callbackVector;

            IfFailRet(CopyInstrumentationMethods(callbackVector));

            for (auto pInstrumentationMethod : callbackVector)
            {
                hr = func(pInstrumentationMethod, parameters...);
            }

            return hr;
        }

        template<typename TInterfaceType, typename TReturnType, typename... TParameters>
        HRESULT SendEventToRawProfilerCallback(TReturnType(__stdcall TInterfaceType::*method)(TParameters...), TParameters... parameters)
        {
            HRESULT hr = S_OK;

            CComPtr<TInterfaceType> pCallback;

            // Holding the lock during the callback functions is dangerous since rentrant
            // events and calls will block. Copy the collection under the lock, then release it and finally call the callbacks
            {
                CCriticalSectionHolder lock(&m_cs);

                if (m_profilerCallbackHolder != nullptr)
                {
                    pCallback = (TInterfaceType*)(m_profilerCallbackHolder->GetMemberForInterface(__uuidof(TInterfaceType)));
                }
            }

            if (pCallback != nullptr)
            {
                CLogging::LogMessage(_T("Sending event to raw ICorProfilerCallback"));

                hr = (pCallback->*method)(parameters...);

                CLogging::LogMessage(_T("Finished Sending event to raw ICorProfilerCallback. hr=%04x"), hr);
            }

            return S_OK;
        }

        HRESULT ConstructAppDomainInfo(
            _In_ AppDomainID appDomainId,
            _Out_ IAppDomainInfo** ppAppDomainInfo
            );

        HRESULT ConstructAssemblyInfo(
            _In_ AssemblyID assemblyId,
            _Out_ IAssemblyInfo** ppAssemblInfo
            );

        HRESULT ConstructModuleInfo(
            _In_ ModuleID moduleId,
            _Out_ IModuleInfo** ppModuleInfo
            );

        HRESULT CreateMethodInfoForRejit(
            _In_ ModuleID moduleId,
            _In_ mdMethodDef methodToken,
            _In_ ICorProfilerFunctionControl* pFunctionControl,
            _Out_ CMethodInfo** ppMethodInfo
            );

        HRESULT SendFabricatedModuleUnloads(_In_ CAppDomainInfo* pRawAppdomainInfo);
        HRESULT SendFabricatedAssemblyUnloads(_In_ CAppDomainInfo* pRawAppdomainInfo);

        HRESULT ModuleUnloadStartedImpl(_In_ ModuleID moduleId);
        HRESULT ModuleUnloadFinishedImpl(_In_ ModuleID moduleId, _In_ HRESULT hrStatus);

        HRESULT AssemblyUnloadStartedImpl(_In_ AssemblyID assemblyId);
        HRESULT AssemblyUnloadFinishedImpl(_In_ AssemblyID assemblyId, _In_ HRESULT hrStatus);

        // Returns the LoggingFlags parsed from their representation in wszRequestedFlagNames so long as they are contained by allowedFlags.
        LoggingFlags ExtractLoggingFlags(
            _In_ LPCWSTR wszRequestedFlagNames,
            _In_ LoggingFlags allowedFlags
            );
        // Returns the test flag if (1) it is a subset of the allowed flags and (2) the test flag name is contained by requested flag names.
        LoggingFlags ExtractLoggingFlag(
            _In_ LPCWSTR wszRequestedFlagNames,
            _In_ LoggingFlags allowedFlags,
            _In_ LPCWSTR wszSingleTestFlagName,
            _In_ LoggingFlags singleTestFlag
            );

        // IProfilerManager methods
    public:
        STDMETHOD(SetupProfilingEnvironment)(_In_reads_(numConfigPaths) BSTR bstrConfigPaths[], _In_ UINT numConfigPaths);

        STDMETHOD(AddRawProfilerHook)(
            _In_ IUnknown *pUnkProfilerCallback
            );

        STDMETHOD(RemoveRawProfilerHook)(
            );

        STDMETHOD(GetCorProfilerInfo)(
            _Outptr_ IUnknown **ppCorProfiler
            );

        STDMETHOD(GetRuntimeType)(_Out_ COR_PRF_RUNTIME_TYPE* pRuntimeType);

        // return the profiler host instance
        STDMETHOD(GetProfilerHost)(_Out_ IProfilerManagerHost** ppProfilerManagerHost);

        // Returns an instance of IProfilerManagerLogging which instrumentation methods can use
        // to log to the profiler manager or optionally to the profiler host.
        STDMETHOD(GetLoggingInstance)(_Out_ IProfilerManagerLogging** ppLogging);

        // By default, logging messages are written to the debug output port. However,
        // hosts can optionally signup to receive them through an instance of IProfilerManagerLoggingHost
        STDMETHOD(SetLoggingHost)(_In_opt_ IProfilerManagerLoggingHost* pLoggingHost);

        STDMETHOD(GetAppDomainCollection)(_Out_ IAppDomainCollection** ppAppDomainCollection);

        STDMETHOD(CreateSignatureBuilder)(_Out_ ISignatureBuilder ** ppSignatureBuilder);

        // Query the profiler manager for a pointer to another instrumentation method.
        // Consumers can call QueryInterface to obtain an interface specific to that
        // instrumentation method
        STDMETHOD(GetInstrumentationMethod)(_In_ REFGUID cslid, _Out_ IUnknown** ppUnknown);

        STDMETHOD(RemoveInstrumentationMethod(_In_ IInstrumentationMethod* pInstrumentationMethod));

		// Registers a new instrumentation method in the profiler manager. Also calls its Initialize() method.
		STDMETHOD(AddInstrumentationMethod)(_In_ BSTR bstrModulePath, _In_ BSTR bstrName, _In_ BSTR bstrDescription, _In_ BSTR bstrModule, _In_ BSTR bstrClassGuid, _In_ DWORD dwPriority, _Out_ IInstrumentationMethod** ppInstrumentationMethod);


    // IProfilerManager2 Methods
    public:
        STDMETHOD(DisableProfiling)();

        STDMETHOD(ApplyMetadata)(_In_ IModuleInfo* pMethodInfo);

    // IProfilerManager3 Methods
    public:
        STDMETHOD(GetApiVersion)(_Out_ DWORD* pApiVer);

    // IProfilerManager4 Methods
    public:
        STDMETHOD(GetGlobalLoggingInstance)(_Out_ IProfilerManagerLogging** ppLogging);

    // IProfilerManager5 Methods
    public:
        STDMETHOD(IsInstrumentationMethodRegistered)(_In_ REFGUID clsid, _Out_ BOOL* pfRegistered);

    // IProfilerManagerLogging Methods
    public:
        // If trace logging in enabled in the profiler manager, trace messages are sent to the
        // profiler manager through this function.
        STDMETHOD(LogMessage)(_In_ const WCHAR* wszMessage);

        // Errors detected during profiling will be sent to the host through this method
        STDMETHOD(LogError)(_In_ const WCHAR* wszError);

        // If instrumentation result tracing is enabled, the detailed results of each instrumented
        // method will be sent to the profiler manager host through this method.
        STDMETHOD(LogDumpMessage)(_In_ const WCHAR* wszMessage);

        // Called to cause logging to be written to the debug output port (via DebugOutputString) as well
        // as to the host.
        STDMETHOD(EnableDiagnosticLogToDebugPort)(_In_ BOOL enable);

        // Allows instrumentation methods and hosts to ask for the current logging flags
        STDMETHOD(GetLoggingFlags)(_Out_ LoggingFlags* pLoggingFlags);

        // Allows instrumentation methods and hosts to modify the current logging flags
        STDMETHOD(SetLoggingFlags)(_In_ LoggingFlags loggingFlags);

        // ICorProfilerCallback methods
    public:
        STDMETHOD(Initialize)(
            _In_ IUnknown *pICorProfilerInfoUnk);

        STDMETHOD(Shutdown)();

        STDMETHOD(AppDomainCreationStarted)(
            _In_ AppDomainID appDomainId
            );

        STDMETHOD(AppDomainCreationFinished)(
            _In_ AppDomainID appDomainId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(AppDomainShutdownStarted)(
            _In_ AppDomainID appDomainId
            );

        STDMETHOD(AppDomainShutdownFinished)(
            _In_ AppDomainID appDomainId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(AssemblyLoadStarted)(
            _In_ AssemblyID assemblyId
            );

        STDMETHOD(AssemblyLoadFinished)(
            _In_ AssemblyID assemblyId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(AssemblyUnloadStarted)(
            _In_ AssemblyID assemblyId
            );

        STDMETHOD(AssemblyUnloadFinished)(
            _In_ AssemblyID assemblyId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(ModuleLoadStarted)(
            _In_ ModuleID moduleId
            );

        STDMETHOD(ModuleLoadFinished)(
            _In_ ModuleID moduleId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(ModuleUnloadStarted)(
            _In_ ModuleID moduleId
            );

        STDMETHOD(ModuleUnloadFinished)(
            _In_ ModuleID moduleId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(ModuleAttachedToAssembly)(
            _In_ ModuleID moduleId,
            _In_ AssemblyID AssemblyId
            );

        STDMETHOD(ClassLoadStarted)(
            _In_ ClassID classId);

        STDMETHOD(ClassLoadFinished)(
            _In_ ClassID classId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(ClassUnloadStarted)(
            _In_ ClassID classId
            );

        STDMETHOD(ClassUnloadFinished)(
            _In_ ClassID classId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(FunctionUnloadStarted)(
            _In_ FunctionID functionId
            );

        STDMETHOD(JITCompilationStarted)(
            _In_ FunctionID functionId,
            _In_ BOOL fIsSafeToBlock
            );

        STDMETHOD(JITCompilationFinished)(
            _In_ FunctionID functionId,
            _In_ HRESULT hrStatus,
            _In_ BOOL fIsSafeToBlock
            );

        STDMETHOD(JITCachedFunctionSearchStarted)(
            _In_ FunctionID functionId,
            /* [out] */ BOOL *pbUseCachedFunction
            );

        STDMETHOD(JITCachedFunctionSearchFinished)(
            _In_ FunctionID functionId,
            _In_ COR_PRF_JIT_CACHE result
            );

        STDMETHOD(JITFunctionPitched)(
            _In_ FunctionID functionId
            );

        STDMETHOD(JITInlining)(
            _In_ FunctionID callerId,
            _In_ FunctionID calleeId,
            /* [out] */ BOOL *pfShouldInline
            );

        STDMETHOD(ThreadCreated)(
            _In_ ThreadID threadId
            );

        STDMETHOD(ThreadDestroyed)(
            _In_ ThreadID threadId
            );

        STDMETHOD(ThreadAssignedToOSThread)(
            _In_ ThreadID managedThreadId,
            _In_ DWORD osThreadId
            );

        STDMETHOD(RemotingClientInvocationStarted)();

        STDMETHOD(RemotingClientSendingMessage)(
            _In_ GUID *pCookie,
            _In_ BOOL fIsAsync);

        STDMETHOD(RemotingClientReceivingReply)(
            _In_ GUID *pCookie,
            _In_ BOOL fIsAsync);

        STDMETHOD(RemotingClientInvocationFinished)();

        STDMETHOD(RemotingServerReceivingMessage)(
            _In_ GUID *pCookie,
            _In_ BOOL fIsAsync
            );

        STDMETHOD(RemotingServerInvocationStarted)();

        STDMETHOD(RemotingServerInvocationReturned)();

        STDMETHOD(RemotingServerSendingReply)(
            _In_ GUID *pCookie,
            _In_ BOOL fIsAsync
            );

        STDMETHOD(UnmanagedToManagedTransition)(
            _In_ FunctionID functionId,
            _In_ COR_PRF_TRANSITION_REASON reason
            );

        STDMETHOD(ManagedToUnmanagedTransition)(
            _In_ FunctionID functionId,
            _In_ COR_PRF_TRANSITION_REASON reason
            );

        STDMETHOD(RuntimeSuspendStarted)(
            _In_ COR_PRF_SUSPEND_REASON suspendReason
            );

        STDMETHOD(RuntimeSuspendFinished)();

        STDMETHOD(RuntimeSuspendAborted)();

        STDMETHOD(RuntimeResumeStarted)();

        STDMETHOD(RuntimeResumeFinished)();

        STDMETHOD(RuntimeThreadSuspended)(
            _In_ ThreadID threadId);

        STDMETHOD(RuntimeThreadResumed)(
            _In_ ThreadID threadId
            );

        STDMETHOD(MovedReferences)(
            _In_ ULONG cMovedObjectIDRanges,
            _In_reads_(cMovedObjectIDRanges) ObjectID oldObjectIDRangeStart[],
            _In_reads_(cMovedObjectIDRanges) ObjectID newObjectIDRangeStart[],
            _In_reads_(cMovedObjectIDRanges) ULONG cObjectIDRangeLength[]
            );

        STDMETHOD(ObjectAllocated)(
            _In_ ObjectID objectId,
            _In_ ClassID classId
            );

        STDMETHOD(ObjectsAllocatedByClass)(
            _In_ ULONG cClassCount,
            _In_reads_(cClassCount) ClassID classIds[],
            _In_reads_(cClassCount) ULONG cObjects[]
            );

        STDMETHOD(ObjectReferences)(
            _In_ ObjectID objectId,
            _In_ ClassID classId,
            _In_ ULONG cObjectRefs,
            _In_reads_(cObjectRefs) ObjectID objectRefIds[]
            );

        STDMETHOD(RootReferences)(
            _In_ ULONG cRootRefs,
            _In_reads_(cRootRefs) ObjectID rootRefIds[]
            );

        STDMETHOD(ExceptionThrown)(
            _In_ ObjectID thrownObjectId
            );

        STDMETHOD(ExceptionSearchFunctionEnter)(
            _In_ FunctionID functionId
            );

        STDMETHOD(ExceptionSearchFunctionLeave)();

        STDMETHOD(ExceptionSearchFilterEnter)(
            _In_ FunctionID functionId);

        STDMETHOD(ExceptionSearchFilterLeave)();

        STDMETHOD(ExceptionSearchCatcherFound)(
            _In_ FunctionID functionId
            );

        STDMETHOD(ExceptionOSHandlerEnter)(
            _In_ UINT_PTR __unused
            );

        STDMETHOD(ExceptionOSHandlerLeave)(
            _In_ UINT_PTR __unused
            );

        STDMETHOD(ExceptionUnwindFunctionEnter)(
            _In_ FunctionID functionId
            );

        STDMETHOD(ExceptionUnwindFunctionLeave)();

        STDMETHOD(ExceptionUnwindFinallyEnter)(
            _In_ FunctionID functionId
            );

        STDMETHOD(ExceptionUnwindFinallyLeave)();

        STDMETHOD(ExceptionCatcherEnter)(
            _In_ FunctionID functionId,
            _In_ ObjectID objectId
            );

        STDMETHOD(ExceptionCatcherLeave)();

        STDMETHOD(COMClassicVTableCreated)(
            _In_ ClassID wrappedClassId,
            _In_ REFGUID implementedIID,
            _In_ void *pVTable,
            _In_ ULONG cSlots
            );

        STDMETHOD(COMClassicVTableDestroyed)(
            _In_ ClassID wrappedClassId,
            _In_ REFGUID implementedIID,
            _In_ void *pVTable
            );

        STDMETHOD(ExceptionCLRCatcherFound)();

        STDMETHOD(ExceptionCLRCatcherExecute)();

        // ICorProfilerCallback2 methods
    public:
        STDMETHOD(ThreadNameChanged)(
            _In_ ThreadID threadId,
            _In_ ULONG cchName,
            _In_reads_opt_(cchName) WCHAR name[]
            );

        STDMETHOD(GarbageCollectionStarted)(
            _In_ int cGenerations,
            _In_reads_(cGenerations) BOOL generationCollected[],
            _In_ COR_PRF_GC_REASON reason
            );

        STDMETHOD(SurvivingReferences)(
            _In_ ULONG cSurvivingObjectIDRanges,
            _In_reads_(cSurvivingObjectIDRanges) ObjectID objectIDRangeStart[],
            _In_reads_(cSurvivingObjectIDRanges) ULONG cObjectIDRangeLength[]
            );

        STDMETHOD(GarbageCollectionFinished)();

        STDMETHOD(FinalizeableObjectQueued)(
            _In_ DWORD finalizerFlags,
            _In_ ObjectID objectID
            );

        STDMETHOD(RootReferences2)(
            _In_ ULONG cRootRefs,
            _In_reads_(cRootRefs) ObjectID rootRefIds[],
            _In_reads_(cRootRefs) COR_PRF_GC_ROOT_KIND rootKinds[],
            _In_reads_(cRootRefs) COR_PRF_GC_ROOT_FLAGS rootFlags[],
            _In_reads_(cRootRefs) UINT_PTR rootIds[]
            );

        STDMETHOD(HandleCreated)(
            _In_ GCHandleID handleId,
            _In_ ObjectID initialObjectId
            );

        STDMETHOD(HandleDestroyed)(
            _In_ GCHandleID handleId
            );

        // ICorProfilerCallback3 methods
    public:
        STDMETHOD(InitializeForAttach)(
            _In_ IUnknown *pCorProfilerInfoUnk,
            _In_ void *pvClientData,
            _In_ UINT cbClientData
            );

        STDMETHOD(ProfilerAttachComplete)(void);

        STDMETHOD(ProfilerDetachSucceeded)(void);

        // ICorProfilerCallback4 methods
    public:
        STDMETHOD(ReJITCompilationStarted)(
            _In_ FunctionID functionId,
            _In_ ReJITID rejitId,
            _In_ BOOL fIsSafeToBlock
            );

        STDMETHOD(GetReJITParameters)(
            _In_ ModuleID moduleId,
            _In_ mdMethodDef methodToken,
            _In_ ICorProfilerFunctionControl* pFunctionControl
            );

        STDMETHOD(ReJITCompilationFinished)(
            _In_ FunctionID functionId,
            _In_ ReJITID rejitId,
            _In_ HRESULT hrStatus,
            _In_ BOOL fIsSafeToBlock
            );

        STDMETHOD(ReJITError)(
            _In_ ModuleID moduleId,
            _In_ mdMethodDef methodId,
            _In_ FunctionID functionId,
            _In_ HRESULT hrStatus
            );

        STDMETHOD(MovedReferences2)(
            _In_ ULONG cMovedObjectIDRanges,
            _In_reads_(cMovedObjectIDRanges) ObjectID oldObjectIDRangeStart[],
            _In_reads_(cMovedObjectIDRanges) ObjectID newObjectIDRangeStart[],
            _In_reads_(cMovedObjectIDRanges) SIZE_T cObjectIDRangeLength[]
            );

        STDMETHOD(SurvivingReferences2)(
            _In_ ULONG cSurvivingObjectIDRanges,
            _In_reads_(cSurvivingObjectIDRanges) ObjectID objectIDRangeStart[],
            _In_reads_(cSurvivingObjectIDRanges) SIZE_T cObjectIDRangeLength[]
            );


        // ICorProfilerCallback5 methods
    public:
        STDMETHOD(ConditionalWeakTableElementReferences)(
            _In_ ULONG cRootRefs,
            _In_reads_(cRootRefs) ObjectID keyRefIds[],
            _In_reads_(cRootRefs) ObjectID valueRefIds[],
            _In_reads_(cRootRefs) GCHandleID rootIds[]
            );

        //ICorProfilerCallback6 methods
    public:
        STDMETHOD(GetAssemblyReferences)(
            _In_ const WCHAR *wszAssemblyPath,
            _In_ ICorProfilerAssemblyReferenceProvider *pAsmRefProvider);

        //ICorProfilerCallback7 methods
    public:
        STDMETHOD(ModuleInMemorySymbolsUpdated)(
            _In_ ModuleID moduleId);

     private:
         // Call ShouldInstrument on each instrumentation method. Return those that return true in pToInstrument
         HRESULT CallShouldInstrumentOnInstrumentationMethods(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit, _Inout_ vector<CComPtr<IInstrumentationMethod>>* pToInstrument);

         // Call CallBeforeInstrumentMethodOnInstrumentationMethods on each instrumentation method.
         HRESULT CallBeforeInstrumentMethodOnInstrumentationMethods(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit, _In_ vector<CComPtr<IInstrumentationMethod>>& toInstrument);

         // Call InstrumentMethod on each instrumentation method.
         HRESULT CallInstrumentOnInstrumentationMethods(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit, _In_ vector<CComPtr<IInstrumentationMethod>>& toInstrument);

         // Call InstrumentMethod on each instrumentation method.
         HRESULT CallOnInstrumentationComplete(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit);

         HRESULT CallAllowInlineOnInstrumentationMethods(_In_ IMethodInfo* pInlineeMethodInfo, _In_ IMethodInfo* pInlineSiteMethodInfo, _Out_ BOOL* pbShouldInline);

        // Registers a new instrumentation method in the profiler manager. Also calls its Initialize() method.
        HRESULT AddInstrumentationMethod(_In_ CInstrumentationMethod* method, _Out_ IInstrumentationMethod** ppInstrumentationMethod);

        HRESULT ClearILTransformationStatus(_In_ FunctionID functionId);

        HRESULT ClearILTransformationStatus(_In_ ModuleID, _In_ mdMethodDef functionToken);
    };

    class CInitializeHolder
    {
        CComPtr<CProfilerManager> m_pProfilerManager;

    public:
        CInitializeHolder(CProfilerManager* pProfilerManager)
        {
            m_pProfilerManager = pProfilerManager;
            m_pProfilerManager->SetIsInInitialize(true);
        }

        ~CInitializeHolder()
        {
            m_pProfilerManager->SetIsInInitialize(false);
        }
    };

    class CInitializeInstrumentationMethodHolder
    {
        CComPtr<CProfilerManager> m_pProfilerManager;

    public:
        CInitializeInstrumentationMethodHolder(CProfilerManager* pProfilerManager)
        {
            m_pProfilerManager = pProfilerManager;
            m_pProfilerManager->SetIsInitializingInstrumentationMethod(true);
        }

        ~CInitializeInstrumentationMethodHolder()
        {
            m_pProfilerManager->SetIsInitializingInstrumentationMethod(false);
        }
    };

#ifndef PLATFORM_UNIX
    OBJECT_ENTRY_AUTO(__uuidof(CProfilerManager), CProfilerManager);
#endif


    // function called when a profiler callback swallows an exception to send the error report to watson as if
    // the app crashed
    void SendErrorReport(EXCEPTION_POINTERS* pException);

    // Translator function that is called by the crt when an SEH exception happens so it can be converted into
    // a C++ exception. This is done so C++ functions can invoke watson on seh exceptions.
    void __cdecl SehTranslatorFunc(unsigned int u, EXCEPTION_POINTERS* pExp);

    // Strongly typed class thrown by SehTranslatorFunc and caught by the PROF_CALLBACK_END macro
    class CSehException
    {
    private:
        EXCEPTION_POINTERS* m_pExceptionPointers;

    public:
        CSehException(EXCEPTION_POINTERS* pExceptionPointers) : m_pExceptionPointers(pExceptionPointers)
        {

        }

        EXCEPTION_POINTERS* GetExceptionPointers()
        {
            return m_pExceptionPointers;
        }
    };
};

#ifndef PLATFORM_UNIX

class CSEHTranslatorHolder
{
    _se_translator_function m_oldSehTranslator;
    bool released;

public:
    CSEHTranslatorHolder()
        : released(false)
    {
        m_oldSehTranslator = _set_se_translator(MicrosoftInstrumentationEngine::SehTranslatorFunc);
    }

    void RestoreSEHTranslator()
    {
        if (!released)
        {
            released = true;
            _set_se_translator(m_oldSehTranslator);
        }
    }

    ~CSEHTranslatorHolder()
    {
        RestoreSEHTranslator();
    }
};


#define PROF_CALLBACK_BEGIN \
    CSEHTranslatorHolder translatorHolder; \
    try \
    { \
    CLogging::LogMessage(_T("Starting ProfilerCallback ") __FUNCTIONW__);

#define PROF_CALLBACK_END \
    CLogging::LogMessage(_T("Ending ProfilerCallback") __FUNCTIONW__); \
    translatorHolder.RestoreSEHTranslator(); \
    } \
    catch (CSehException sehException) \
    { \
    translatorHolder.RestoreSEHTranslator(); \
    CLogging::LogError(_T("Exception during profiler callback ") __FUNCTIONW__); \
    SendErrorReport(sehException.GetExceptionPointers()); \
    } \
    catch (...) \
    { \
    translatorHolder.RestoreSEHTranslator(); \
    CLogging::LogError(_T("Exception during profiler callback ") __FUNCTIONW__); \
    }
#else
#define PROF_CALLBACK_BEGIN \
    try \
    { \
    CLogging::LogMessage(_T("Starting ProfilerCallback ") WCHAR_SPEC, __FUNCTION__);


#define PROF_CALLBACK_END \
    CLogging::LogMessage(_T("Ending ProfilerCallback ") WCHAR_SPEC, __FUNCTION__); \
    } \
    catch (...) \
    { \
    CLogging::LogError(_T("Exception during profiler callback ") WCHAR_SPEC, __FUNCTION__); \
    }
#endif

#define IGNORE_IN_NET20_BEGIN \
	if (m_attachedClrVersion != ClrVersion_2) \
	{ \

#define IGNORE_IN_NET20_END \
	}