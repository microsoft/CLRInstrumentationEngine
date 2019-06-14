// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "DataContainer.h"
#include "ModuleInfo.h"
#include "InstructionGraph.h"
#include "ExceptionSection.h"
#include "InstructionFactory.h"
#include "LocalVariableCollection.h"
#include "../InstrumentationEngine.Lib/SharedArray.h"

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CModuleInfo;
    class CInstructionGraph;
    class CProfilerManager;
    class CExceptionSection;
    class CLocalVariableCollection;
    class CInstructionFactory;

    class __declspec(uuid("CDD3824F-B876-4450-9459-885BA1C21540"))
    CMethodInfo : public IMethodInfo2, public CDataContainer
    {
    private:
        // static map of method infos by function id. This is needed becasue some raw profiler
        // callbacks only take a FunctionID instead of a moduleid / functionid
        // these are cleaned up after instrumentation for the method is over.
        static std::unordered_map <FunctionID, CComPtr<CMethodInfo>> s_methodInfos;

        // True if this method info is not shared across calls and therefore not stored in s_methodInfos or within the containing module info
        // This ensures unrelated calls do not stomp on the lifetime of methodinfos.
        bool m_bIsStandaloneMethodInfo;

        FunctionID m_functionId;
        mdToken m_tkFunction;

        ClassID m_classId;
        CComPtr<CModuleInfo> m_pModuleInfo;

        // Clr callback for rejit scenarios. Not set for non-rejit.
        CComPtr<ICorProfilerFunctionControl> m_pFunctionControl;

        // metadata token of the containing class
        mdTypeDef m_tkTypeDef;

        // declaring type
        CComPtr<IType> m_pDeclaringType;

        // return value type
        CComPtr<IType> m_pReturnType;

        // NOTE: Not initialized until a call to GetName is made.
        // then the value is cached
        CComBSTR m_bstrMethodName;

        // NOTE: Not initialized until a call to GetFullName is made.
        // then the value is cached
        CComBSTR m_bstrMethodFullName;

        CorMethodAttr m_methodAttr;
        CorMethodImpl m_implFlags;
        ULONG m_rva;

        // method signatures
        PCOR_SIGNATURE m_pSig;
        ULONG m_cbSigBlob;
        bool m_bDeleteCorSig;

        // method parameters
        std::vector<CComPtr<IMethodParameter>> m_parameters;
        std::vector<CComPtr<IType>> m_genericParameters;

        CComPtr<CInstructionGraph> m_pInstructionGraph;

        // All instrumented methods are converted to fat header
        COR_ILMETHOD_FAT m_newCorHeader;
        bool m_bIsHeaderInitialized;

        // Do not use CComPtr here! Local variables are tied to the lifetime of MethodInfo.
        // Local vars
        CLocalVariableCollection* m_localVariables;

        // Read-only original local vars.
        CLocalVariableCollection* m_origLocalVariables;

        // Raw IL stream. Set after instrumentation methods are finished, but before raw profiler callbacks.
        // This does not include method headers or exception handlers
        CAutoVectorPtr<BYTE> m_pILStream;
        DWORD m_dwILStreamLen;

        // map of old offsets to new offsets. Originally set when instrumentation methods have finished.
        // Updated if raw callbacks modify il. Note, this is a CSharedArray because it is also cached by this
        // method's ModuleInfo
        CSharedArray<COR_IL_MAP> m_pCorILMap;

        // The rendered method body including headers and exception handlers after instrumenation methods have finished, but before
        // raw profilers execute. This includes the updated core header, the il body, the exception ranges etc...
        CAutoVectorPtr<BYTE> m_pIntermediateRenderedMethod;
        DWORD m_cbIntermediateRenderedMethod;

        // The final rendered method body after all instrumenation methods and raw profiler hooks have instrumented
        // NOTE: a seperate copy is needed because the clr requires a callee destroyed buffer in the rejit cases, and
        // some hosts, including MMA, will try to consume the buffer after the set to calculate the cor il map.
        CAutoVectorPtr<BYTE> m_pFinalRenderedMethod;
        DWORD m_cbFinalRenderedMethod;

        // Set to true if any client has instrumented the method.
        bool m_bIsInstrumented;

        CComPtr<CExceptionSection> m_pExceptionSection;

        CComPtr<CInstructionFactory> m_pInstructionFactory;

        // Code gen flags used during a rejit
        DWORD m_dwRejitCodeGenFlags;

        bool m_bMethodSignatureInitialized;

        bool m_bCorAttributesInitialized;

        bool m_bGenericParametersInitialized;

        // Set to false after the before instrumentation pass to ensure instrumentation methods
        // do not set the baseline at incorrect times.
        bool m_bIsCreateBaselineEnabled;

        bool m_bIsRejit;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CMethodInfo);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CMethodInfo);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IMethodInfo2*>(this),
                static_cast<IMethodInfo*>(this),
                static_cast<IDataContainer*>(static_cast<CDataContainer*>(this)),
                riid,
                ppvObject
                );
        }
    public:
        CMethodInfo(
            _In_ FunctionID functionId,
            _In_ mdToken functionToken,
            _In_ ClassID classId,
            _In_ CModuleInfo* pModuleInfo,
            _In_opt_ ICorProfilerFunctionControl* pFunctionControl
            );

        ~CMethodInfo();

        HRESULT Initialize(_In_ bool bAddToMethodInfoMap, _In_ bool isRejit);

        // Called after instrumentation is finished with the method. This removes it from the
        // static map. Only used for the non-rejit instrumentation code path. Not for any other
        // scenario.
        HRESULT Cleanup();

        static HRESULT GetMethodInfoById(_In_ FunctionID functionId, _In_ CMethodInfo** ppMethodInfo);

        HRESULT ApplyIntermediateMethodInstrumentation();

        HRESULT CreateILFunctionBody();

        HRESULT GetIntermediateRenderedFunctionBody(
            _Out_ LPCBYTE* ppMethodHeader,
            _Out_ ULONG* pcbMethodSize
            );

        // Called after the raw profiler hook has instrumented the function. Sets the IL transformation
        // status of this method to true.
        HRESULT SetFinalRenderedFunctionBody(
            _In_ LPCBYTE pMethodHeader,
            _In_ ULONG cbMethodSize
            );

         bool IsInstrumented()
         {
             return m_bIsInstrumented;
         }

         // Applys all IL transformations and sets the IL cached transformation status of this method to true.
         HRESULT ApplyFinalInstrumentation();

         HRESULT MergeILInstrumentedCodeMap(_In_ ULONG cILMapEntries, _In_reads_(cILMapEntries) COR_IL_MAP rgILMapEntries[]);

        // Recreate the instruction graph, exception section, and cor_il_map from the
        // raw results. This is used by the diagnostic logging engine.
        HRESULT GetInstrumentationResults(
            _Out_ CInstructionGraph** ppInstructionGraph,
            _Out_ CExceptionSection** ppExceptionSection,
            _Out_ COR_IL_MAP** ppCorILMap,
            _Out_ DWORD* dwCorILMapmLen
            );

        static bool s_bIsDumpingMethod;

        void LogMethodInfo();
        void LogInstructionGraph(_In_ CInstructionGraph* pInstructionGraph);
        void LogExceptionSection(_In_ CExceptionSection* pExceptionSection);
        void LogCorIlMap(_In_ COR_IL_MAP* pCorIlMap, _In_ DWORD dwCorILMapmLen);
        tstring GetCorElementTypeString(_In_ IType* pType);


        CModuleInfo* GetModuleInfo();

        bool IsCreateBaselineEnabled();
        void DisableCreateBaseline();
        bool IsRejit()
        {
            return m_bIsRejit;
        }

        // IMethodInfo methods
    public:
        virtual HRESULT __stdcall GetModuleInfo(_Out_ IModuleInfo** ppModuleInfo) override;

        virtual HRESULT __stdcall GetName(_Out_ BSTR* pbstrName) override;
        virtual HRESULT __stdcall GetFullName(_Out_ BSTR* pbstrFullName) override;

        // Obtain the graph of instructions.  Manipulating this graph will result in changes to the code
        // a the end of jit or rejit.
        virtual HRESULT __stdcall GetInstructions(_Out_ IInstructionGraph** ppInstructionGraph) override;

        // Obtain the collection of local variables
        virtual HRESULT __stdcall GetLocalVariables(_Out_ ILocalVariableCollection** ppLocalVariables) override;

        virtual HRESULT __stdcall GetClassId(_Out_ ClassID* pClassId) override;
        virtual HRESULT __stdcall GetFunctionId(_Out_ FunctionID* pFunctionID) override;
        virtual HRESULT __stdcall GetMethodToken(_Out_ mdToken* pToken) override;
        virtual HRESULT __stdcall GetGenericParameterCount(_Out_ DWORD* pCount) override;
        virtual HRESULT __stdcall GetIsStatic(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsPublic(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsPrivate(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsPropertyGetter(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsPropertySetter(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsFinalizer(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsConstructor(_Out_ BOOL* pbValue) override;
        virtual HRESULT __stdcall GetIsStaticConstructor(_Out_ BOOL* pbValue) override;

        virtual HRESULT __stdcall GetParameters(_Out_ IEnumMethodParameters** ppMethodArgs) override;
        virtual HRESULT __stdcall GetDeclaringType(_Out_ IType** ppType) override;
        virtual HRESULT __stdcall GetReturnType(_Out_ IType** ppType) override;

        virtual HRESULT __stdcall GetCorSignature(_In_ DWORD cbBuffer, _Out_opt_ BYTE* corSignature, _Out_ DWORD* pcbSignature) override;

        virtual HRESULT __stdcall GetLocalVarSigToken(_Out_ mdToken* pToken) override;
        virtual HRESULT __stdcall SetLocalVarSigToken(_In_ mdToken token) override;

        virtual HRESULT __stdcall GetAttributes(_Out_ /*CorMethodAttr*/ DWORD* pCorMethodAttr) override;
        virtual HRESULT __stdcall GetRejitCodeGenFlags(_Out_ DWORD* pRefitFlags) override;
        virtual HRESULT __stdcall GetCodeRva(_Out_ DWORD* pRva) override;
        virtual HRESULT __stdcall MethodImplFlags(_Out_ /*CorMethodImpl*/ UINT* pCorMethodImpl) override;

        // Allow callers to adjust optimizations during a rejit. For instance, disable all optimizations against a method
        virtual HRESULT __stdcall SetRejitCodeGenFlags(_In_ DWORD dwFlags) override;

        virtual HRESULT __stdcall GetExceptionSection(_Out_ IExceptionSection** ppExceptionSection) override;

        virtual HRESULT __stdcall GetInstructionFactory(_Out_ IInstructionFactory** ppInstructionFactory) override;

        // Return the running count of the number of rejits for this methodinfo
        virtual HRESULT __stdcall GetRejitCount(_Out_ DWORD* pdwRejitCount) override;

        virtual HRESULT __stdcall GetMaxStack(_Out_ DWORD* pMaxStack) override;

        virtual HRESULT __stdcall GetSingleRetDefaultInstrumentation(_Out_ ISingleRetDefaultInstrumentation** ppSingleRetDefaultInstrumentation) override;

    // IMethodInfo2 methods
    public:
        virtual HRESULT __stdcall GetOriginalLocalVariables(_Out_ ILocalVariableCollection** ppLocalVariables) override;

    private:
        HRESULT InitializeName(_In_ mdToken tkFunction);
        HRESULT InitializeCorAttributes(_In_ mdToken tkFunction);
        HRESULT InitializeMethodSignature(_In_ mdToken tkFunction);
        HRESULT InitializeHeader(
            _Out_opt_ IMAGE_COR_ILMETHOD** ppMethodHeader = nullptr,
            _Out_opt_ ULONG* pcbMethodSize = 0);
        HRESULT InitializeInstructionsAndExceptions();
        HRESULT InitializeGenericParameters(mdToken tkFunction);

        HRESULT GetFinalInstrumentation(_Out_ DWORD* pcbMethodBody, _Out_ BYTE** ppMethodBody);

        HRESULT InitializeFullName();

    };
}

