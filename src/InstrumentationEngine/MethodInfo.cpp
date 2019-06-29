// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "MethodInfo.h"
#include "InstructionGraph.h"
#include "ProfilerManager.h"
#include "LocalVariableCollection.h"
#include "EnumeratorImpl.h"
#include "MethodParameter.h"
#include "TypeCreator.h"
#include <sstream>
#include "SingleRetDefaultInstrumentation.h"
#include "Util.h"

//static
std::unordered_map<FunctionID, CComPtr<MicrosoftInstrumentationEngine::CMethodInfo>> MicrosoftInstrumentationEngine::CMethodInfo::s_methodInfos;

bool MicrosoftInstrumentationEngine::CMethodInfo::s_bIsDumpingMethod = false;


MicrosoftInstrumentationEngine::CMethodInfo::CMethodInfo(
    _In_ FunctionID functionId,
    _In_ mdToken functionToken,
    _In_ ClassID classId,
    _In_ CModuleInfo* pModuleInfo,
    _In_opt_ ICorProfilerFunctionControl* pFunctionControl
    ) :
    m_bIsStandaloneMethodInfo(false),
    m_functionId(functionId),
    m_tkFunction(functionToken),
    m_classId(classId),
    m_pModuleInfo(pModuleInfo),
    m_pFunctionControl(pFunctionControl),
    m_methodAttr((CorMethodAttr)-1),
    m_implFlags(miMaxMethodImplVal),
    m_rva(0),
    m_pSig(nullptr),
    m_cbSigBlob(0),
    m_dwILStreamLen(0),
    m_localVariables(nullptr),
    m_origLocalVariables(nullptr),
    m_cbIntermediateRenderedMethod(0),
    m_cbFinalRenderedMethod(0),
    m_bIsInstrumented(false),
    m_dwRejitCodeGenFlags(0),
    m_bDeleteCorSig(false),
    m_bMethodSignatureInitialized(false),
    m_bCorAttributesInitialized(false),
    m_bGenericParametersInitialized(false),
    m_bIsCreateBaselineEnabled(true),
    m_bIsHeaderInitialized(false),
    m_bIsRejit(false)
{
    DEFINE_REFCOUNT_NAME(CMethodInfo);

    ZeroMemory(&m_newCorHeader, sizeof(m_newCorHeader));
}

MicrosoftInstrumentationEngine::CMethodInfo::~CMethodInfo()
{
    if (m_bDeleteCorSig)
    {
        delete[] m_pSig;
        m_pSig = NULL;
        m_bDeleteCorSig = false;
    }
    if (m_localVariables)
    {
        //Calling the base class Release will destroy the local variables object, and properly record termination.
        //Note we cannot call Release directly, as it will effect MethodInfo instead.
        m_localVariables->CRefCount::Release();
        m_localVariables = nullptr;
    }
    if (m_origLocalVariables)
    {
        m_origLocalVariables->CRefCount::Release();
        m_origLocalVariables = nullptr;
    }
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::Initialize(_In_ bool bAddToMethodInfoMap, _In_ bool isRejit)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::Initialize"));

    // Do the minimum required amount of initialization.
    // The rest is deferred until needed

    // During a rejit, the function id will not be valid
    if (bAddToMethodInfoMap)
    {
        if (m_functionId == 0)
        {
            CLogging::LogError(_T("CMethodInfo::Initialize - cannot add to method info map without a function id"));
            return E_FAIL;
        }
        s_methodInfos.insert({ m_functionId, this });
    }

    if (isRejit)
    {
        if (m_functionId != 0)
        {
            CLogging::LogError(_T("CMethodInfo::Initialize - expected function id of 0 for rejitted method."));
        }

        IfFailRet(m_pModuleInfo->IncrementMethodRejitCount(m_tkFunction));
    }

    m_bIsRejit = isRejit;

    // Standalone method infos are not appropriate for instrumentation so warn via a log message
    // if one is used this way.
    if (!bAddToMethodInfoMap && !isRejit)
    {
        m_bIsStandaloneMethodInfo = true;
    }

    CLogging::LogMessage(_T("End CMethodInfo::Initialize"));
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::Cleanup()
{
    if (!m_bIsStandaloneMethodInfo)
    {
        m_pModuleInfo->ReleaseMethodInfo(m_functionId);

        s_methodInfos.erase(m_functionId);
    }

    // Don't touch the this pointer after this point.

    return S_OK;
}

//static
HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetMethodInfoById(_In_ FunctionID functionId, _In_ CMethodInfo** ppMethodInfo)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetMethodById"));
    IfNullRetPointer(ppMethodInfo);
    *ppMethodInfo = NULL;

    CComPtr<CMethodInfo> pMethodInfo = s_methodInfos[functionId];
    if (pMethodInfo != NULL)
    {
        *ppMethodInfo = pMethodInfo.Detach();
    }
    else
    {
        CLogging::LogMessage(_T("CMethodInfo::GetMethodById - No method info found"));
        return E_FAIL;
    }

    CLogging::LogMessage(_T("End CMethodInfo::GetMethodById"));
    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetModuleInfo(_Out_ IModuleInfo** ppModuleInfo)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetModuleInfo"));
    IfNullRetPointer(ppModuleInfo);
    *ppModuleInfo = nullptr;

    *ppModuleInfo = (IModuleInfo*)m_pModuleInfo;
    (*ppModuleInfo)->AddRef();

    CLogging::LogMessage(_T("End CMethodInfo::GetModuleInfo"));
    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetName(_Out_ BSTR* pbstrName)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetName"));
    IfNullRetPointer(pbstrName);

    IfFailRet(InitializeName(m_tkFunction));

    hr = m_bstrMethodName.CopyTo(pbstrName);

    CLogging::LogMessage(_T("End CMethodInfo::GetName"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetFullName(_Out_ BSTR* pbstrFullName)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetFullName"));
    IfNullRetPointer(pbstrFullName);

    IfFailRet(InitializeFullName());

    hr = m_bstrMethodFullName.CopyTo(pbstrFullName);

    CLogging::LogMessage(_T("End CMethodInfo::GetFullName"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::InitializeFullName()
{
    HRESULT hr = S_OK;

    if (m_bstrMethodFullName.Length() != 0)
    {
        return S_OK;
    }

    //Initialize generic parameters. Must occur before GetNameAndCorAttributes
    InitializeGenericParameters(m_tkFunction);

    // Get the sig and name
    InitializeName(m_tkFunction);

    CComPtr<IType> declaringType;

    CComPtr<ITypeCreator> pTypeFactory;
    IfFailRet(m_pModuleInfo->CreateTypeFactory(&pTypeFactory));

    BOOL isValueType = FALSE;
    IfFailRet(static_cast<CTypeCreator*>(pTypeFactory.p)->IsValueType(m_tkTypeDef, &isValueType));
    IfFailRet(pTypeFactory->FromToken(isValueType ? ELEMENT_TYPE_VALUETYPE : ELEMENT_TYPE_CLASS, m_tkTypeDef, &m_pDeclaringType));

    //For generic, make the declaring type be an instantiation, instead of the definition (List`1<!0> vs List`1)
    /*if (_pDeclaringType)
    {
    TypeDef const *pGenericTypeDef = dynamic_cast<TypeDef const*>(_pDeclaringType);
    if(pGenericTypeDef && pGenericTypeDef->get_GenericArgumentCount()>0)
    {
    DWORD cGenericArguments = pGenericTypeDef->get_GenericArgumentCount ();
    Pseudo::Array<const Type*> genericArguments(cGenericArguments);
    for (DWORD i = 0; i < cGenericArguments; i++)
    {
    genericArguments[i] = GenericArgumentType::GetGenericArgument(i);
    }
    _pDeclaringType = GenericInstanceType::GetGenericInstance(_pDeclaringType,genericArguments);
    }
    }*/

    //Build fullName
    tstringstream nameBuilder;
    if (m_pDeclaringType)
    {
        CComBSTR declaringTypeName;
        if (SUCCEEDED(m_pDeclaringType->GetName(&declaringTypeName)) && declaringTypeName.Length() > 0)
        {
            nameBuilder << (LPWSTR)declaringTypeName << _T(".");
        }
    }
    nameBuilder << (LPWSTR)m_bstrMethodName;
    m_bstrMethodFullName = nameBuilder.str().c_str();

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::InitializeMethodSignature(_In_ mdToken tkFunction)
{
    HRESULT hr = S_OK;

    if (m_bMethodSignatureInitialized)
    {
        return S_OK;
    }

    IfFailRet(InitializeCorAttributes(m_tkFunction));

    DWORD sizeUsed = 0;
    CComPtr<IEnumTypes> pEnumTypes;
    DWORD genericParameterCount = 0;

    CComPtr<ITypeCreator> pTypeFactory;
    IfFailRet(m_pModuleInfo->CreateTypeFactory(&pTypeFactory));
    CComQIPtr<ISignatureParser> pSignatureParser(pTypeFactory);
    IfNullRet(pSignatureParser);

    hr = pSignatureParser->ParseMethodSignature(
        m_pSig,
        m_cbSigBlob,
        nullptr,
        &m_pReturnType,
        &pEnumTypes,
        &genericParameterCount,
        &sizeUsed);

    if (SUCCEEDED(hr))
    {
        if (m_cbSigBlob != sizeUsed)
        {
            CLogging::LogError(_T("Failed to parse entire signature. Expected %d actual %d"), m_cbSigBlob, sizeUsed);
        }

        ULONG cTypes;
        CComPtr<IType> pType;
        while (S_OK == (hr = pEnumTypes->Next(1, &pType, &cTypes)))
        {
            CComPtr<IMethodParameter> pMethod;
            pMethod.Attach(new CMethodParameter(pType));
            if (pMethod == nullptr)
            {
                //TODO fail due to out of memory
                break;
            }
            m_parameters.push_back(pMethod);

            pType.Release();
        }
        IfFailRet(hr);
        hr = S_OK;

        m_bMethodSignatureInitialized = true;
    }
    else
    {
        CLogging::LogError(_T("Failed to parse methodsignature for %s with error code %x"), m_bstrMethodFullName.m_str, hr);
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::InitializeCorAttributes(_In_ mdToken tkFunction)
{
    HRESULT hr = S_OK;

    if (m_bCorAttributesInitialized)
    {
        return S_OK;
    }

    CComPtr<IMetaDataImport2> pMetaDataImport;
    IfFailRet(m_pModuleInfo->GetMetaDataImport((IUnknown**)&pMetaDataImport));

    if (TypeFromToken(m_tkFunction) == mdtMethodDef)
    {
        DWORD cbMethodName;
        IfFailRet(pMetaDataImport->GetMethodProps(m_tkFunction, nullptr, nullptr, 0, &cbMethodName, nullptr, nullptr, nullptr, nullptr, nullptr));
        std::vector<WCHAR> nameBuilder(cbMethodName);
        IfFailRet(pMetaDataImport->GetMethodProps(tkFunction, &m_tkTypeDef, nameBuilder.data(), cbMethodName, &cbMethodName, (DWORD *)&m_methodAttr, (PCCOR_SIGNATURE*)&m_pSig, &m_cbSigBlob, &m_rva, (DWORD *)&m_implFlags));
        m_bstrMethodName = nameBuilder.data();
    }
    else if (TypeFromToken(m_tkFunction) == mdtMemberRef)
    {
        DWORD cbMethodName;
        IfFailRet(pMetaDataImport->GetMemberRefProps(m_tkFunction, nullptr, nullptr, 0, &cbMethodName, nullptr, nullptr));
        std::vector<WCHAR> nameBuilder(cbMethodName);
        IfFailRet(pMetaDataImport->GetMemberRefProps(tkFunction, &m_tkTypeDef, nameBuilder.data(), cbMethodName, &cbMethodName, (PCCOR_SIGNATURE*)&m_pSig, &m_cbSigBlob));
        m_bstrMethodName = nameBuilder.data();
    }
    else
    {
        mdMethodDef tkMethodDef;
        IfFailRet(pMetaDataImport->GetMethodSpecProps(tkFunction, &tkMethodDef, nullptr, nullptr));
        // Get the method def name
        IfFailRet(InitializeCorAttributes(tkMethodDef));
    }

    m_bCorAttributesInitialized = true;

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::ApplyIntermediateMethodInstrumentation()
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::ApplyIntermediateMethodInstrumentation"));

    if (m_pInstructionGraph == NULL)
    {
        IfFailRet(InitializeInstructionsAndExceptions());
    }

    ATL::CAutoVectorPtr<COR_IL_MAP> pCorILMap;
    DWORD mapSize;
    IfFailRet(m_pInstructionGraph->EncodeIL(&m_pILStream, &m_dwILStreamLen, &pCorILMap, &mapSize));
    IfFailRet(MergeILInstrumentedCodeMap(mapSize, pCorILMap));

    if (m_localVariables)
    {
        IfFailRet(m_localVariables->CommitSignature());
    }

    // ready to create a new function's body
    IfFailRet(CreateILFunctionBody());

    m_bIsInstrumented = true;

    CLogging::LogMessage(_T("End CMethodInfo::ApplyIntermediateMethodInstrumentation"));
    return hr;
}

#define FAT_HEADER_SIZE 0xC /* 12 bytes = WORD + WORD + DWORD + DWORD */

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::CreateILFunctionBody()
{
    HRESULT hr = S_OK;

    if (m_pIntermediateRenderedMethod != nullptr)
    {
        CLogging::LogError(_T("CMethodInfo::CreateILFunctionBody - intermediate method body has already been set."));
        return E_FAIL;
    }

    // extra bytes occupied by the old method (e.g., exception table).
    // so, cbSehExtra = header + IL + (pLast - pFirst) - (header + IL) = (pLast - pFirst);
    ULONG cbSehExtra = 0;

    // "real" method size incl. method header, i.e. header + IL
    ULONG cbNewMethodSize = 0;

    // get "real" method size
    // get header size  (currently 12 bytes)
    // (4 * Size) size in DWORDs of this structure (currently 3)
    cbNewMethodSize = m_newCorHeader.Size * 4;

    // get method IL body size
    cbNewMethodSize += m_dwILStreamLen; // IL code size

    // new method: delta between last IL byte and first SEH header
    int nNewDelta = (4 - ((cbNewMethodSize) & 0x3)) & 3;

    CAutoVectorPtr<BYTE> pSectEhBuffer;
    DWORD cbSectEhBuffer = 0;
    IfFailRet(m_pExceptionSection->CreateExceptionHeader((BYTE**)&pSectEhBuffer, &cbSectEhBuffer));
    COR_ILMETHOD_SECT_EH_FAT* pSectEh = (COR_ILMETHOD_SECT_EH_FAT*)(pSectEhBuffer.m_p);
    cbSehExtra = sizeof(IMAGE_COR_ILMETHOD_SECT_EH_FAT);
    if(pSectEh != NULL)
    {
        cbSehExtra += pSectEh->DataSize;
    }

    // new method size (including header)
    ULONG cbNewMethodSizeWithoutHeader = (cbNewMethodSize - FAT_HEADER_SIZE);

    cbNewMethodSize = cbNewMethodSize + /* header + IL */ nNewDelta + cbSehExtra /* SEH stuff */;

    // allocate memory
    m_pIntermediateRenderedMethod.Allocate(cbNewMethodSize);
    m_cbIntermediateRenderedMethod = cbNewMethodSize;

    COR_ILMETHOD_FAT* pNewMethod = (COR_ILMETHOD_FAT*)(m_pIntermediateRenderedMethod.m_p);

    // copy old header.
    *pNewMethod = m_newCorHeader;

    // Calculate the updated maxstack
    DWORD maxStack = 0;
    IfFailRet(m_pInstructionGraph->CalculateMaxStack(&maxStack));
    pNewMethod->MaxStack = maxStack;

    // specify the new method size
    pNewMethod->CodeSize = cbNewMethodSizeWithoutHeader;

    // Sect EH flag if needed
    if(pSectEh)
    {
        pNewMethod->Flags |= CorILMethod_MoreSects;
    }

    // copy the function body
    if  (cbNewMethodSizeWithoutHeader > 0)
    {
        IfFailRetErrno(memcpy_s((BYTE*)pNewMethod + FAT_HEADER_SIZE, cbNewMethodSizeWithoutHeader, m_pILStream, cbNewMethodSizeWithoutHeader));
    }

    // add SEH sections
    if  (pSectEh)
    {
        IfFailRetErrno(memcpy_s((BYTE*)pNewMethod + FAT_HEADER_SIZE + cbNewMethodSizeWithoutHeader + nNewDelta, cbSehExtra, pSectEh, cbSehExtra));
    }


    return S_OK;
}

// Obtain the graph of instructions.  Manipulating this graph will result in changes to the code
// a the end of jit or rejit.
HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetInstructions(_Out_ IInstructionGraph** ppInstructionGraph)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CMethodInfo::GetInstructions"));

    if (m_bIsStandaloneMethodInfo)
    {
        // If this is a stand alone method info (one not being used for instrumentation) complain if someone accesses the instruction graph.
        // This is because a jit can be occurring at the the same time which would result in the instruction graph being wrong.
        // The current use of stanalone method infos is during the exception callbacks when instrumentation wouldn't be allowed anyway.
        // NOTE: Current tests examine the instruction graph however, so don't fail the method.
        CLogging::LogError(_T("CMethodInfo::GetInstructions - standalone method infos are not configured properly for instrumentation."));
    }

    if (m_pInstructionGraph == NULL)
    {
        IfFailRet(InitializeInstructionsAndExceptions());
    }

    *ppInstructionGraph = m_pInstructionGraph;
    (*ppInstructionGraph)->AddRef();

    CLogging::LogMessage(_T("End CMethodInfo::GetInstructions"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::InitializeHeader(
    _Out_opt_ IMAGE_COR_ILMETHOD** ppMethodHeader,
    _Out_opt_ ULONG* pcbMethodSize)
{
    HRESULT hr = S_OK;

    if (ppMethodHeader)
    {
        *ppMethodHeader = nullptr;
    }
    if (pcbMethodSize)
    {
        *pcbMethodSize = 0;
    }

    CComPtr<CProfilerManager> pProfilerManager;
    IfFailRet(CProfilerManager::GetProfilerManagerInstance(&pProfilerManager));

    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    IfFailRet(pProfilerManager->GetRealCorProfilerInfo(&pCorProfilerInfo));

    IMAGE_COR_ILMETHOD* pMethodHeader = nullptr;
    ULONG cbMethodSize = 0;
    IfFailRet(m_pModuleInfo->GetMethodIl(pCorProfilerInfo, m_tkFunction, &pMethodHeader, &cbMethodSize));

    // Prevent reinitialization of the header when called multiple times.
    if (!m_bIsHeaderInitialized)
    {
        // Convert the method header to FAT if it isn't already and extract that raw il instructions.
        ULONG codeSize = 0;
        if (((COR_ILMETHOD *)pMethodHeader)->Tiny.IsTiny())
        {
            m_newCorHeader.Flags = CorILMethod_FatFormat | CorILMethod_InitLocals;
            m_newCorHeader.Size = sizeof(IMAGE_COR_ILMETHOD_FAT) / sizeof(DWORD);
            m_newCorHeader.MaxStack = 8;
            m_newCorHeader.CodeSize = ((COR_ILMETHOD_TINY*)pMethodHeader)->GetCodeSize();
            m_newCorHeader.LocalVarSigTok = 0;
        }
        else
        {
            memcpy_s(&m_newCorHeader, sizeof(COR_ILMETHOD_FAT), pMethodHeader, sizeof(COR_ILMETHOD_FAT));
        }

        m_newCorHeader.Flags |= CorILMethod_InitLocals;

        m_bIsHeaderInitialized = true;
    }

    if (ppMethodHeader)
    {
        *ppMethodHeader = pMethodHeader;
    }
    if (pcbMethodSize)
    {
        *pcbMethodSize = cbMethodSize;
    }

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::InitializeInstructionsAndExceptions()
{
    HRESULT hr = S_OK;

    if (m_pInstructionGraph)
    {
        return S_OK;
    }

    CLogging::LogMessage(_T("CMethodInfo::InitializeInstructionsAndExceptions - Initializing Instruction Graph"));

    // Lazily init the instruction graph when it is asked for.
    m_pInstructionGraph.Attach(new CInstructionGraph());
    if (!m_pInstructionGraph)
    {
        return E_OUTOFMEMORY;
    }

    IfFailRet(m_pInstructionGraph->Initialize(this));

    ModuleID moduleId;
    IfFailRet(m_pModuleInfo->GetModuleID(&moduleId));

    IMAGE_COR_ILMETHOD* pMethodHeader = nullptr;
    ULONG cbMethodSize = 0;
    IfFailRet(InitializeHeader(
        &pMethodHeader,
        &cbMethodSize));

    LPCBYTE pMethodBody;
    ULONG codeSize = 0;
    if (((COR_ILMETHOD *)pMethodHeader)->Tiny.IsTiny())
    {
        codeSize = m_newCorHeader.CodeSize;

        cbMethodSize += sizeof (IMAGE_COR_ILMETHOD_FAT) - sizeof (IMAGE_COR_ILMETHOD_TINY);
        pMethodBody = (LPCBYTE) pMethodHeader + sizeof (IMAGE_COR_ILMETHOD_TINY);
    }
    else
    {
        pMethodBody = (LPCBYTE)pMethodHeader + sizeof(IMAGE_COR_ILMETHOD_FAT);
        codeSize = ((COR_ILMETHOD_FAT*)pMethodHeader)->GetCodeSize();
    }

    LPCBYTE pMethodEnd = pMethodBody + codeSize;

    // Convert the raw instructions to an instance of the instruction graph.
    IfFailRet(m_pInstructionGraph->DecodeInstructions(pMethodBody, pMethodEnd));

    // Initialize the exception section;
    // The instruction graph is passed in directly rather than being obtained from the method info
    // because during the diagnostic dumping logic, the graph will be from the rendered function
    // body and not directly from the method info.
    m_pExceptionSection.Attach(new CExceptionSection(this));
    IfFailRet(m_pExceptionSection->Initialize(pMethodHeader, cbMethodSize, m_pInstructionGraph));

    // Convert branches to the larger form, this also calculates the current offsets for instructions
    // NOTE: this must be done after the exception sections are initialized as they depend on the original
    // form of the instructions.
    IfFailRet(m_pInstructionGraph->ExpandBranches());

    return S_OK;
}

// Obtain the collection of local variables
HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetLocalVariables(_Out_ ILocalVariableCollection** ppLocalVariables)
{
    IfNullRetPointer(ppLocalVariables);

    if (m_localVariables == nullptr)
    {
        //Does not AddRef. CRefCount field does start at 1, but we track the lifetime of this object with the method info instead.
        m_localVariables = new CLocalVariableCollection(this, false);
        if (m_localVariables == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    //Outstanding callers increase our ref count by 1.
    return m_localVariables->QueryInterface(__uuidof(ILocalVariableCollection), (void**)ppLocalVariables);
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetOriginalLocalVariables(_Out_ ILocalVariableCollection** ppLocalVariables)
{
    IfNullRetPointer(ppLocalVariables);

    if (m_origLocalVariables == nullptr)
    {
        m_origLocalVariables = new CLocalVariableCollection(this, true);
        if (m_origLocalVariables == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    return m_origLocalVariables->QueryInterface(__uuidof(ILocalVariableCollection), (void**)ppLocalVariables);
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetClassId(_Out_ ClassID* pClassId)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetClassId"));
    IfNullRetPointer(pClassId);

    if (m_classId == 0)
    {
        CLogging::LogError(_T("CMethodInfo::GetClassId - Class id is not available during a rejit."));
        return E_FAIL;
    }

    *pClassId = m_classId;

    CLogging::LogMessage(_T("End CMethodInfo::GetClassId"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetFunctionId(_Out_ FunctionID* pFunctionID)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetFunctionId"));
    IfNullRetPointer(pFunctionID);

    if (m_functionId == 0)
    {
        CLogging::LogError(_T("CMethodInfo::GetFunctionId - FunctionID is not available during a rejit."));
        return E_FAIL;
    }

    *pFunctionID = m_functionId;

    CLogging::LogMessage(_T("End CMethodInfo::GetFunctionId"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetMethodToken(_Out_ mdToken* pToken)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetMethodDefToken"));
    IfNullRetPointer(pToken);

    *pToken = m_tkFunction;

    CLogging::LogMessage(_T("End CMethodInfo::GetMethodDefToken"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetGenericParameterCount(_Out_ DWORD* pCount)
{
    HRESULT hr = S_OK;

    IfFailRet(InitializeGenericParameters(m_tkFunction));

    *pCount = (DWORD)m_genericParameters.size();
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIsStatic(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetIsStatic"));

    IfNullRetPointer(pbValue);

    IfFailRet(InitializeCorAttributes(m_tkFunction));

    *pbValue = IsFlagSet(m_methodAttr, mdStatic);

    CLogging::LogMessage(_T("End CMethodInfo::GetIsStatic %04x"), *pbValue);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIsPublic(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetIsPublic"));

    IfNullRetPointer(pbValue);

    IfFailRet(InitializeCorAttributes(m_tkFunction));

    *pbValue = IsFlagSet(m_methodAttr, mdPublic);

    CLogging::LogMessage(_T("End CMethodInfo::GetIsPublic %04x"), *pbValue);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIsPrivate(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetIsPrivate"));

    IfNullRetPointer(pbValue);

    IfFailRet(InitializeCorAttributes(m_tkFunction));

    *pbValue = IsFlagSet(m_methodAttr, mdPrivate);

    CLogging::LogMessage(_T("End CMethodInfo::GetIsPrivate %04x"), *pbValue);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIsPropertyGetter(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetIsPropertyGetter"));

    IfNullRetPointer(pbValue);

    IfFailRet(InitializeName(m_tkFunction));

#ifndef PLATFORM_UNIX
    if (wcsncmp(m_bstrMethodName, _T("get_"), 4) == 0)
#else
    if (PAL_wcsncmp(m_bstrMethodName, _T("get_"), 4) == 0)
#endif
    {
        *pbValue = TRUE;
    }
    else
    {
        *pbValue = FALSE;
    }

    CLogging::LogMessage(_T("End CMethodInfo::GetIsPropertyGetter %04x"), *pbValue);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIsPropertySetter(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetIsPropertySetter"));

    IfNullRetPointer(pbValue);

    IfFailRet(InitializeName(m_tkFunction));

#ifndef PLATFORM_UNIX
    if (wcsncmp(m_bstrMethodName, _T("get_"), 4) == 0)
#else
    if (PAL_wcsncmp(m_bstrMethodName, _T("get_"), 4) == 0)
#endif
    {
        *pbValue = TRUE;
    }
    else
    {
        *pbValue = FALSE;
    }

    CLogging::LogMessage(_T("End CMethodInfo::GetIsPropertySetter %04x"), *pbValue);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIsFinalizer(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetIsFinalizer"));

    IfNullRetPointer(pbValue);

    IfFailRet(InitializeName(m_tkFunction));

#ifndef PLATFORM_UNIX
    if (wcscmp(m_bstrMethodName, _T("Finalize")) == 0)
#else
    if (PAL_wcscmp(m_bstrMethodName, _T("Finalize")) == 0)
#endif
    {
        *pbValue = TRUE;
    }
    else
    {
        *pbValue = FALSE;
    }

    CLogging::LogMessage(_T("End CMethodInfo::GetIsFinalizer %04x"), *pbValue);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIsConstructor(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetIsConstructor"));

    IfNullRetPointer(pbValue);

    IfFailRet(InitializeName(m_tkFunction));

#ifndef PLATFORM_UNIX
    if (wcscmp(m_bstrMethodName, COR_CTOR_METHOD_NAME_W) == 0)
#else
    if (PAL_wcscmp(m_bstrMethodName, _T(".ctor")) == 0)
#endif
    {
        *pbValue = TRUE;
    }
    else
    {
        *pbValue = FALSE;
    }

    CLogging::LogMessage(_T("End CMethodInfo::GetIsConstructor %04x"), *pbValue);

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIsStaticConstructor(_Out_ BOOL* pbValue)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetIsStaticConstructor"));

    IfNullRetPointer(pbValue);

    IfFailRet(InitializeName(m_tkFunction));

#ifndef PLATFORM_UNIX
    if (wcscmp(m_bstrMethodName, COR_CCTOR_METHOD_NAME_W) == 0)
#else
    if (PAL_wcscmp(m_bstrMethodName, _T(".cctor")) == 0)
#endif
    {
        *pbValue = TRUE;
    }
    else
    {
        *pbValue = FALSE;
    }

    CLogging::LogMessage(_T("End CMethodInfo::GetIsStaticConstructor %04x"), *pbValue);

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetParameters(_Out_ IEnumMethodParameters** ppMethodArgs)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(ppMethodArgs);
    *ppMethodArgs = nullptr;

    IfFailRet(InitializeMethodSignature(m_tkFunction));

    CComPtr<CEnumerator<IEnumMethodParameters, IMethodParameter>> parameterEnum;

    parameterEnum.Attach(new CEnumerator<IEnumMethodParameters, IMethodParameter>());
    if (parameterEnum == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    IfFailRet(parameterEnum->Initialize(m_parameters));
    *ppMethodArgs = parameterEnum.Detach();
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetDeclaringType(_Out_ IType** ppType)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetDeclaringType"));

    InitializeFullName();

    IfNullRetPointer(ppType);

    hr = m_pDeclaringType.CopyTo(ppType);

    CLogging::LogMessage(_T("End CMethodInfo::GetDeclaringType"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetReturnType(_Out_ IType** ppType)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetReturnType"));

    IfNullRetPointer(ppType);

    IfFailRet(InitializeMethodSignature(m_tkFunction));

    hr = m_pReturnType.CopyTo(ppType);

    CLogging::LogMessage(_T("End CMethodInfo::GetReturnType"));

    return hr;
}


HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetCorSignature(_In_ DWORD cbBuffer, _Out_opt_ BYTE* pCorSignature, _Out_ DWORD* pcbSignature)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetCorSignature"));
    IfNullRetPointer(pcbSignature);

    IfFailRet(InitializeMethodSignature(m_tkFunction));

    *pcbSignature = m_cbSigBlob;

    if (pCorSignature != nullptr)
    {
        if (cbBuffer >= m_cbSigBlob)
        {
            memcpy_s(pCorSignature, cbBuffer, m_pSig, m_cbSigBlob);
            return S_OK;
        }
        else
        {
            CLogging::LogError(_T("End CMethodInfo::GetCorSignature"));
            return E_FAIL;
        }
    }

    CLogging::LogMessage(_T("End CMethodInfo::GetCorSignature"));

    return S_FALSE;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetLocalVarSigToken(_Out_ mdToken* pToken)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetLocalVarSigToken"));
    IfNullRetPointer(pToken);

    IfFailRet(InitializeHeader());

    *pToken = m_newCorHeader.LocalVarSigTok;

    CLogging::LogMessage(_T("End CMethodInfo::GetLocalVarSigToken"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::SetLocalVarSigToken(_In_ mdToken token)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::SetLocalVarSigToken"));

    IfFailRet(InitializeHeader());

    m_newCorHeader.LocalVarSigTok = token;

    CLogging::LogMessage(_T("End CMethodInfo::SetLocalVarSigToken"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetAttributes(_Out_ /*CorMethodAttr*/ DWORD* pCorMethodAttr)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetAttributes"));
    IfNullRetPointer(pCorMethodAttr);

    IfFailRet(InitializeCorAttributes(m_tkFunction));

    *pCorMethodAttr = m_methodAttr;

    CLogging::LogMessage(_T("End CMethodInfo::GetAttributes"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetRejitCodeGenFlags(_Out_ DWORD* pRefitFlags)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pRefitFlags);

    *pRefitFlags = 0;

    CLogging::LogMessage(_T("Starting CMethodInfo::GetRejitCodeGenFlags"));

    *pRefitFlags = m_dwRejitCodeGenFlags;

    CLogging::LogMessage(_T("Starting CMethodInfo::GetRejitCodeGenFlags"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetCodeRva(_Out_ DWORD* pRva)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetCodeRva"));
    IfNullRetPointer(pRva);

    IfFailRet(InitializeCorAttributes(m_tkFunction));

    *pRva = m_rva;

    CLogging::LogMessage(_T("End CMethodInfo::GetCodeRva"));
    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::MethodImplFlags(_Out_ /*CorMethodImpl*/ UINT* pCorMethodImpl)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::MethodImplFlags"));
    IfNullRetPointer(pCorMethodImpl);

    IfFailRet(InitializeCorAttributes(m_tkFunction));

    *pCorMethodImpl = m_implFlags;

    CLogging::LogMessage(_T("End CMethodInfo::MethodImplFlags"));
    return hr;
}


// Allow callers to adjust optimizations during a rejit. For instance, disable all optimizations against a method
HRESULT MicrosoftInstrumentationEngine::CMethodInfo::SetRejitCodeGenFlags(_In_ DWORD dwFlags)
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Starting CMethodInfo::SetRejitCodeGenFlags"));

    m_dwRejitCodeGenFlags = dwFlags;

    CLogging::LogMessage(_T("Starting CMethodInfo::SetRejitCodeGenFlags"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetExceptionSection(_Out_ IExceptionSection** ppExceptionSection)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetExceptionSection"));
    IfNullRetPointer(ppExceptionSection);
    *ppExceptionSection = NULL;

    if (m_pExceptionSection == NULL)
    {
        if (m_pInstructionGraph == NULL)
        {
            IfFailRet(InitializeInstructionsAndExceptions());
        }
    }

    if (m_pExceptionSection == NULL)
    {
        return E_FAIL;
    }

    *ppExceptionSection = (IExceptionSection*)(m_pExceptionSection.p);
    (*ppExceptionSection)->AddRef();

    CLogging::LogMessage(_T("End CMethodInfo::GetExceptionSection"));

    return S_OK;
}


HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetInstructionFactory(_Out_ IInstructionFactory** ppInstructionFactory)
{
    HRESULT hr = S_OK;
    CLogging::LogMessage(_T("Starting CMethodInfo::GetInstructionFactory"));
    IfNullRetPointer(ppInstructionFactory);
    *ppInstructionFactory = NULL;

    if (m_pInstructionFactory == NULL)
    {
        m_pInstructionFactory.Attach(new CInstructionFactory);
    }

    *ppInstructionFactory = (IInstructionFactory*)(m_pInstructionFactory.p);
    (*ppInstructionFactory)->AddRef();

    CLogging::LogMessage(_T("End CMethodInfo::GetInstructionFactory"));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::InitializeName(_In_ mdToken tkFunction)
{
    HRESULT hr = S_OK;

    if (m_bstrMethodName.Length() != 0)
    {
        return S_OK;
    }

    IfFailRet(InitializeCorAttributes(tkFunction));

    IfFailRet(InitializeGenericParameters(tkFunction));

    if (!m_genericParameters.empty())
    {
        tstringstream nameBuilder;

        nameBuilder << (LPWSTR)m_bstrMethodName << _T("<");

        for (ULONG i = 0; i < m_genericParameters.size(); i++)
        {
            WCHAR szIndex[11] = { 0 }; // ULONG probably can contain 4,294,967,295 so make space for 10 digits + \0.
            _ultow_s(i, szIndex, 11, 10);
            nameBuilder << _T("!!") << szIndex;

            if (i < m_genericParameters.size() - 1)
            {
                nameBuilder << _T(",");
            }
        }
        nameBuilder << _T(">");

        tstring value = nameBuilder.str();

        m_bstrMethodName = value.c_str();
    }

    return S_OK;
}


HRESULT MicrosoftInstrumentationEngine::CMethodInfo::InitializeGenericParameters(mdToken tkFunction)
{
    HRESULT hr = S_OK;

    if (m_bGenericParametersInitialized)
    {
        return S_OK;
    }

    CComPtr<IMetaDataImport2> pMetaDataImport;
    IfFailRet(m_pModuleInfo->GetMetaDataImport((IUnknown**)&pMetaDataImport));

    CComPtr<ITypeCreator> pTypeFactory;
    IfFailRet(m_pModuleInfo->CreateTypeFactory(&pTypeFactory));

    if (TypeFromToken(tkFunction) == mdtMethodSpec)
    {
        PCCOR_SIGNATURE pSigBlob;
        DWORD cbSigBlob;
        IfFailRet(pMetaDataImport->GetMethodSpecProps(tkFunction, nullptr, &pSigBlob, &cbSigBlob));

        if (m_pModuleInfo)
        {
            PCCOR_SIGNATURE sigBlob = pSigBlob;
            // get a method signature, which looks like this: "calling Conv" + "argument Cnt" + "gen arg1" + "gen arg2" + ...
            CorCallingConvention callConv = IMAGE_CEE_CS_CALLCONV_MAX; // = 0x10 - first invalid calling convention;
            sigBlob += CorSigUncompressData(sigBlob, (ULONG*)&callConv);

            // get the generic argument count
            ULONG nArgGeneric;
            sigBlob += CorSigUncompressData(sigBlob, &nArgGeneric);

            // get the params
            for (ULONG i = 0; sigBlob && i < nArgGeneric; ++i)
            {
                DWORD currentOffset = static_cast<DWORD>(sigBlob - pSigBlob);
                CComPtr<ITypeCreator> typeFactory;
                IfFailRet(m_pModuleInfo->CreateTypeFactory(&typeFactory));
                CComPtr<IType> parameter;

                DWORD used;
                IfFailRet(typeFactory->FromSignature(cbSigBlob - currentOffset, &pSigBlob[currentOffset], &parameter, &used));
                currentOffset += used;
                m_genericParameters.push_back(parameter);
            }
        }
    }
    else if (m_functionId)
    {
        CComPtr<CProfilerManager> pProfilerManager;
        IfFailRet(CProfilerManager::GetProfilerManagerInstance(&pProfilerManager));

        CComPtr<ICorProfilerInfo> pCorProfilerInfo;
        IfFailRet(pProfilerManager->GetRealCorProfilerInfo(&pCorProfilerInfo));

        CComPtr<ICorProfilerInfo2> pCorProfilerInfo2;
        IfFailRet(pCorProfilerInfo->QueryInterface(&pCorProfilerInfo2));

        // Get the real args for the function.
        ULONG32 cGenericArguments;
        ClassID classId = 0;
        IfFailRet(pCorProfilerInfo2->GetFunctionInfo2(m_functionId, 0, nullptr, nullptr, nullptr, 0, &cGenericArguments, nullptr));
        if (cGenericArguments)
        {
            auto classIdList = make_unique<ClassID[]>(cGenericArguments);
            IfFailRet(pCorProfilerInfo2->GetFunctionInfo2(m_functionId, 0, nullptr, nullptr, nullptr, cGenericArguments, &cGenericArguments, classIdList.get()));

            for (ULONG i = 0; i < cGenericArguments; i++)
            {
                CComPtr<IType> genericParameter;
                //TODO expand class to actual type from the classId. We may not be able to get accurate data due to missing frame information
                IfFailRet(pTypeFactory->FromCorElement(ELEMENT_TYPE_OBJECT, &genericParameter));

                m_genericParameters.push_back(CComPtr<IType>(genericParameter));
            }
        }
    }
    else
    {
        CMetadataEnumCloser<IMetaDataImport2> spHCorEnum(pMetaDataImport, NULL);

        ULONG cGenericParams = 0;
        mdGenericParam genericParams[4];
        while (pMetaDataImport->EnumGenericParams(spHCorEnum.Get(), tkFunction, genericParams, _countof(genericParams), &cGenericParams) == S_OK)
        {
            for (ULONG i = 0; i < cGenericParams; i++)
            {
                CComPtr<IType> genericParameter;
                if (FAILED(pTypeFactory->FromCorElement(ELEMENT_TYPE_OBJECT, &genericParameter)))
                {
                    //TODO really should use CMetadataEnumCloser
                    break;
                }
                m_genericParameters.push_back(CComPtr<IType>(genericParameter));

            }
        }
    }

    m_bGenericParametersInitialized = true;

    return hr;
}

// Called by the profiler info wrapper when a raw callback requests a function's il
HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetIntermediateRenderedFunctionBody(
        _Out_ LPCBYTE* ppMethodHeader,
        _Out_ ULONG* pcbMethodSize
        )
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Start CMethodInfo::GetIntermediateRenderedFunctionBody"));
    IfNullRetPointer(ppMethodHeader);
    IfNullRetPointer(pcbMethodSize);

    if (!this->IsInstrumented() || m_pIntermediateRenderedMethod == nullptr)
    {
        CLogging::LogError(_T("CMethodInfo::GetIntermediateRenderedFunctionBody should only be called if a method body has been set for this function"));
        return E_FAIL;
    }

    *ppMethodHeader = (LPCBYTE)m_pIntermediateRenderedMethod;
    *pcbMethodSize = m_cbIntermediateRenderedMethod;

    CLogging::LogMessage(_T("End CMethodInfo::GetIntermediateRenderedFunctionBody"));

    return hr;
}

// Called by the profiler info wrapper when a raw callback sets a function's il
HRESULT MicrosoftInstrumentationEngine::CMethodInfo::SetFinalRenderedFunctionBody(
    _In_ LPCBYTE pMethodHeader,
    _In_ ULONG cbMethodSize
    )
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Start CMethodInfo::SetFinalRenderedFunctionBody"));
    IfNullRetPointer(pMethodHeader);

    m_bIsInstrumented = true;

    if (m_pFinalRenderedMethod != nullptr)
    {
        CLogging::LogError(_T("CMethodInfo::SetFinalRenderedFunctionBody - final method body should only be called once."));
        return E_FAIL;
    }

    m_pModuleInfo->SetMethodIsTransformed(m_tkFunction, true);
    m_pFinalRenderedMethod.Allocate(cbMethodSize);
    IfFailRetErrno(memcpy_s(m_pFinalRenderedMethod, cbMethodSize, pMethodHeader, cbMethodSize));

    m_cbFinalRenderedMethod = cbMethodSize;

    CLogging::LogMessage(_T("End CMethodInfo::SetFinalRenderedFunctionBody"));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::ApplyFinalInstrumentation()
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Start CMethodInfo::ApplyFinalInstrumentation"));

    if (!this->IsInstrumented())
    {
        CLogging::LogError(_T("CMethodInfo::ApplyFinalInstrumentation should only be called if a method body has been set for this function"));
        return E_FAIL;
    }

    CComPtr<CProfilerManager> pProfilerManager;
    IfFailRet(CProfilerManager::GetProfilerManagerInstance(&pProfilerManager));

    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    IfFailRet(pProfilerManager->GetRealCorProfilerInfo(&pCorProfilerInfo));

    if (!IsRejit())
    {
        CLogging::LogMessage(_T("CMethodInfo::ApplyFinalInstrumentation - Non-rejit case"));

        ModuleID moduleId;
        IfFailRet(m_pModuleInfo->GetModuleID(&moduleId));

        CComPtr<IMethodMalloc> pMalloc;
        IfFailRet(pCorProfilerInfo->GetILFunctionBodyAllocator(moduleId, &pMalloc));

        DWORD cbMethodBody = 0;
        BYTE* pMethodBody = nullptr;
        IfFailRet(GetFinalInstrumentation(&cbMethodBody, &pMethodBody));

        PVOID pFunction = pMalloc->Alloc(cbMethodBody);
        IfFailRetErrno(memcpy_s(pFunction, cbMethodBody, pMethodBody, cbMethodBody));

        LogMethodInfo();

        IfFailRet(pCorProfilerInfo->SetILFunctionBody(moduleId, m_tkFunction, (LPCBYTE)pFunction));

        m_pModuleInfo->SetMethodIsTransformed(m_tkFunction, true);

        // This will fail if no entries in CorILMap, plus there's no point in setting this in such a case anyway.
        if (m_pCorILMap.Count() > 0)
        {
            IfFailRet(pCorProfilerInfo->SetILInstrumentedCodeMap(m_functionId, TRUE, (ULONG)m_pCorILMap.Count(), m_pCorILMap.Get()));
        }
    }
    else
    {
        CLogging::LogMessage(_T("CMethodInfo::ApplyFinalInstrumentation - rejit case"));

        if (m_pFunctionControl == NULL)
        {
            CLogging::LogError(_T("CMethodInfo::ApplyFinalInstrumentation - m_pFunctionControl should not be null during a rejit"));
            return E_FAIL;
        }

        LogMethodInfo();

        DWORD cbMethodBody = 0;
        BYTE* pMethodBody = nullptr;
        IfFailRet(GetFinalInstrumentation(&cbMethodBody, &pMethodBody));

        IfFailRet(m_pFunctionControl->SetILFunctionBody(cbMethodBody, pMethodBody));

        m_pModuleInfo->SetMethodIsTransformed(m_tkFunction, true);

        IfFailRet(m_pFunctionControl->SetCodegenFlags(m_dwRejitCodeGenFlags));

        // If the function has a rejit count, append it to the end of the cor_il_map
        // This allows the debugger to tell exactly which version of the pdb should
        // be used for this function
        // NOTE: This is a hack workaround. Hopefully, the clr can give us a better way
        // to do this
        DWORD rejitCount = 0;
        GetRejitCount(&rejitCount);
        if (rejitCount != 0)
        {
            DWORD corILMapmLen = (DWORD)m_pCorILMap.Count();
            corILMapmLen++;
            CSharedArray<COR_IL_MAP> pTempCorILMap(corILMapmLen);
            size_t capacity = m_pCorILMap.Count() * sizeof(COR_IL_MAP);
            IfFailRetErrno(memcpy_s(pTempCorILMap.Get(), capacity, m_pCorILMap.Get(), capacity));

            pTempCorILMap[corILMapmLen - 1].fAccurate = true;
            pTempCorILMap[corILMapmLen - 1].oldOffset = 0xfeefee;
            pTempCorILMap[corILMapmLen - 1].newOffset = rejitCount;

            // NOTE: CLR's earlier than 4.5.2 won't have this implemented and will return E_NOTIMPL.
            // 4.5.2 is a prereq for rejit.
            m_pFunctionControl->SetILInstrumentedCodeMap(corILMapmLen, pTempCorILMap.Get());
        }
        else
        {
            // NOTE: CLR's earlier than 4.5.2 won't have this implemented and will return E_NOTIMPL.
            // 4.5.2 is a prereq for rejit.
            m_pFunctionControl->SetILInstrumentedCodeMap((ULONG)m_pCorILMap.Count(), m_pCorILMap.Get());
        }
    }

    CLogging::LogMessage(_T("End CMethodInfo::ApplyFinalInstrumentation"));

    return hr;
}

// static
void MicrosoftInstrumentationEngine::CMethodInfo::LogInstructionGraph(_In_ CInstructionGraph* pInstructionGraph)
{
    if (!CLogging::AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    CComPtr<IInstruction> pInstruction;

    CLogging::LogDumpMessage(_T("[TestIgnore]<OriginalInstructions><![CDATA[\r\n"));

    pInstructionGraph->GetOriginalFirstInstruction(&pInstruction);

    while (pInstruction != NULL)
    {
        ((CInstruction*)pInstruction.p)->LogInstruction(true);

        CComPtr<IInstruction> pTemp = pInstruction;
        pInstruction.Release();
        pTemp->GetOriginalNextInstruction(&pInstruction);
    }

    CLogging::LogDumpMessage(_T("[TestIgnore]]]></OriginalInstructions>\r\n"));

    CLogging::LogDumpMessage(_T("    <Instructions><![CDATA[\r\n"));

    pInstructionGraph->GetFirstInstruction(&pInstruction);

    while (pInstruction != NULL)
    {
        ((CInstruction*)pInstruction.p)->LogInstruction(false);

        CComPtr<IInstruction> pTemp = pInstruction;
        pInstruction.Release();
        pTemp->GetNextInstruction(&pInstruction);
    }

    CLogging::LogDumpMessage(_T("    ]]></Instructions>\r\n"));
}

// static
void MicrosoftInstrumentationEngine::CMethodInfo::LogExceptionSection(_In_ CExceptionSection* pExceptionSection)
{
    if (!CLogging::AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    CLogging::LogDumpMessage(_T("    <ExceptionClauses>\r\n"));

    CComPtr<IEnumExceptionClauses> pEnum;
    pExceptionSection->GetExceptionClauses(&pEnum);

    DWORD cActual = 0;
    CComPtr<IExceptionClause> pExceptionClause;
    pEnum->Next(1, &pExceptionClause, &cActual);

    while (pExceptionClause != NULL)
    {
        DWORD flags = 0;
        CComPtr<IInstruction> pFirstTryInstruction;
        CComPtr<IInstruction> pLastTryInstruction;
        CComPtr<IInstruction> pFirstHandlerInstruction;
        CComPtr<IInstruction> pLastHandlerInstruction;
        CComPtr<IInstruction> pFilterInstruction;

        pExceptionClause->GetFlags(&flags);
        pExceptionClause->GetTryFirstInstruction(&pFirstTryInstruction);
        pExceptionClause->GetTryLastInstruction(&pLastTryInstruction);
        pExceptionClause->GetHandlerFirstInstruction(&pFirstHandlerInstruction);
        pExceptionClause->GetHandlerLastInstruction(&pLastHandlerInstruction);
        pExceptionClause->GetFilterFirstInstruction(&pFilterInstruction);

        DWORD offsetFirstTry = 0;
        DWORD offsetLastTry = 0;
        DWORD offsetFirstHandler = 0;
        DWORD offsetLastHandler = 0;
        DWORD offsetFilter = 0;

        if (pFirstTryInstruction != NULL)
        {
            pFirstTryInstruction->GetOffset(&offsetFirstTry);
        }

        if (pLastTryInstruction != NULL)
        {
            pLastTryInstruction->GetOffset(&offsetLastTry);
        }

        if (pFirstHandlerInstruction != NULL)
        {
            pFirstHandlerInstruction->GetOffset(&offsetFirstHandler);
        }

        if (pLastHandlerInstruction != NULL)
        {
            pLastHandlerInstruction->GetOffset(&offsetLastHandler);
        }

        if (pFilterInstruction != NULL)
        {
            pFilterInstruction->GetOffset(&offsetFilter);
        }

        mdToken tokException;
        // The token of the type in the catch block
        pExceptionClause->GetExceptionHandlerType(&tokException);

        CLogging::LogDumpMessage(_T("        <ExceptionClause>\r\n"));
        CLogging::LogDumpMessage(_T("            <Flags>0x%08x</Flags>\r\n"), flags);
        CLogging::LogDumpMessage(_T("            <FirstTryOffset>0x%08x</FirstTryOffset>\r\n"), offsetFirstTry);
        CLogging::LogDumpMessage(_T("            <LastTryOffset>0x%08x</LastTryOffset>\r\n"), offsetLastTry);
        CLogging::LogDumpMessage(_T("            <FirstHandlerOffset>0x%08x</FirstHandlerOffset>\r\n"), offsetFirstHandler);
        CLogging::LogDumpMessage(_T("            <LastHandlerOffset>0x%08x</LastHandlerOffset>\r\n"), offsetLastHandler);
        CLogging::LogDumpMessage(_T("            <FilterOffset>0x%08x</FilterOffset>\r\n"), offsetFilter);
        CLogging::LogDumpMessage(_T("            <ExceptionToken>0x%08x</ExceptionToken>\r\n"), tokException);
        CLogging::LogDumpMessage(_T("        </ExceptionClause>\r\n"));

        pExceptionClause.Release();
        pEnum->Next(1, &pExceptionClause, &cActual);
    }

    CLogging::LogDumpMessage(_T("    </ExceptionClauses>\r\n"));
}

// static
void MicrosoftInstrumentationEngine::CMethodInfo::LogCorIlMap(_In_ COR_IL_MAP* pCorIlMap, _In_ DWORD dwCorILMapmLen)
{
    if (!CLogging::AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    CLogging::LogDumpMessage(_T("    <CorIlMap>\r\n"));

    for (DWORD i = 0; i < dwCorILMapmLen; i++)
    {
        CLogging::LogDumpMessage(_T("        <CorIlMapEntry>\r\n"));
        CLogging::LogDumpMessage(_T("            <fAccurate>%01d</fAccurate>\r\n"), pCorIlMap[i].fAccurate);
        CLogging::LogDumpMessage(_T("            <oldOffset>0x%08x</oldOffset>\r\n"), pCorIlMap[i].oldOffset);
        CLogging::LogDumpMessage(_T("            <newOffset>0x%08x</newOffset>\r\n"), pCorIlMap[i].newOffset);
        CLogging::LogDumpMessage(_T("        </CorIlMapEntry>\r\n"));
    }

    CLogging::LogDumpMessage(_T("    </CorIlMap>\r\n"));
}

// static
void MicrosoftInstrumentationEngine::CMethodInfo::LogMethodInfo()
{
    if (!CLogging::AllowLogEntry(LoggingFlags_InstrumentationResults))
    {
        return;
    }

    s_bIsDumpingMethod = true;

    CComBSTR bstrMethodName;
    this->GetName(&bstrMethodName);

    CComBSTR bstrMethodFullName;
    this->GetFullName(&bstrMethodFullName);

    ClassID classId = 0;
    FunctionID functionId = 0;

    if (!IsRejit())
    {
        this->GetClassId(&classId);
        this->GetFunctionId(&functionId);
    }

    mdToken methodToken;
    this->GetMethodToken(&methodToken);

    DWORD genericParamCount = 0;
    this->GetGenericParameterCount(&genericParamCount);

    BOOL isStatic;
    this->GetIsStatic(&isStatic);

    BOOL isPublic;
    this->GetIsPublic(&isPublic);

    BOOL isPrivate;
    this->GetIsPrivate(&isPrivate);

    BOOL isPropGetter;
    this->GetIsPropertyGetter(&isPropGetter);

    BOOL isPropSetter;
    this->GetIsPropertySetter(&isPropSetter);

    BOOL isFinalizer;
    this->GetIsFinalizer(&isFinalizer);

    BOOL isConstructor;
    this->GetIsConstructor(&isConstructor);

    BOOL isStaticConstructor;
    this->GetIsStaticConstructor(&isStaticConstructor);

    CComPtr<IType> pDeclaringType;
    this->GetDeclaringType(&pDeclaringType);
    CComPtr<ITokenType> pDeclaringTokenType;
    pDeclaringType->QueryInterface(__uuidof(ITokenType), (LPVOID*)&pDeclaringTokenType);
    mdToken declaringTypeToken = mdTokenNil;
    pDeclaringTokenType->GetToken(&declaringTypeToken);

    CComPtr<IType> pReturnType;
    this->GetReturnType(&pReturnType);

    mdToken retTypeToken = mdTokenNil;
    CComPtr<ITokenType> pReturnTokenType;
    pReturnType->QueryInterface(__uuidof(ITokenType), (LPVOID*)&pReturnTokenType);

    if (pReturnTokenType != nullptr)
    {
        pReturnTokenType->GetToken(&retTypeToken);
    }

    DWORD cbCorSig = 0;
    DWORD cbCorSigActual = 0;
    this->GetCorSignature(cbCorSig, NULL, &cbCorSigActual);

    CAutoVectorPtr<BYTE> pCorSig;
    pCorSig.Allocate(cbCorSigActual);
    cbCorSig = cbCorSigActual;
    this->GetCorSignature(cbCorSig, pCorSig, &cbCorSigActual);

    CorMethodAttr corMethodAttr;
    this->GetAttributes((DWORD*)(&corMethodAttr));

    DWORD rva;
    this->GetCodeRva(&rva);

    CorMethodImpl methodImpl;
    this->MethodImplFlags((UINT*)(&methodImpl));

    CComPtr<CInstructionGraph> pInstructionGraph;
    CComPtr<CExceptionSection> pExceptionSection;
    CAutoVectorPtr<COR_IL_MAP> pCorILMap;
    DWORD dwCorILMapmLen;

    CComPtr<ILocalVariableCollection> pLocals;
    CComPtr<ISignatureBuilder> pSignatureBuilder;
    const BYTE* pLocalSig = nullptr;
    DWORD dwLocalSigCount = 0;
    if (SUCCEEDED(this->GetLocalVariables(&pLocals)))
    {
        if (SUCCEEDED(pLocals->GetCorSignature(&pSignatureBuilder)))
        {
            pSignatureBuilder->GetCorSignaturePtr(&pLocalSig);
            pSignatureBuilder->GetSize(&dwLocalSigCount);
        }
    }

    DWORD maxStack;
    this->GetMaxStack(&maxStack);

    CLogging::LogDumpMessage(_T("<?xml version=\"1.0\"?>\r\n"));
    CLogging::LogDumpMessage(_T("[TestIgnore]<Pid>%5d</Pid>\r\n"), GetCurrentProcessId());
    CLogging::LogDumpMessage(_T("<InstrumentedMethod>\r\n"));
    CLogging::LogDumpMessage(_T("    <Name>%s</Name>\r\n"), bstrMethodName.m_str);
    CLogging::LogDumpMessage(_T("    <FullName>%s</FullName>\r\n"), bstrMethodFullName.m_str);
    CLogging::LogDumpMessage(_T("    <ClassID Volatile=\"True\">0x%08x</ClassID>\r\n"), classId);
    CLogging::LogDumpMessage(_T("    <FunctionID>0x%08x</FunctionID>\r\n"), isStatic);
    CLogging::LogDumpMessage(_T("    <MethodToken>0x%08x</MethodToken>\r\n"), methodToken);
    CLogging::LogDumpMessage(_T("    <GenericParameterCount>0x%08x</GenericParameterCount>\r\n"), genericParamCount);
    CLogging::LogDumpMessage(_T("    <IsStatic>%1d</IsStatic>\r\n"), isStatic);
    CLogging::LogDumpMessage(_T("    <IsPublic>%1d</IsPublic>\r\n"), isPublic);
    CLogging::LogDumpMessage(_T("    <IsPrivate>%1d</IsPrivate>\r\n"), isPrivate);
    CLogging::LogDumpMessage(_T("    <IsPropGetter>%1d</IsPropGetter>\r\n"), isPropGetter);
    CLogging::LogDumpMessage(_T("    <IsPropSetter>%1d</IsPropSetter>\r\n"), isPropSetter);
    CLogging::LogDumpMessage(_T("    <IsFinalizer>%1d</IsFinalizer>\r\n"), isFinalizer);
    CLogging::LogDumpMessage(_T("    <IsConstructor>%1d</IsConstructor>\r\n"), isConstructor);
    CLogging::LogDumpMessage(_T("    <IsStaticConstructor>%1d</IsStaticConstructor>\r\n"), isConstructor & isStatic);
    CLogging::LogDumpMessage(_T("    <DeclaringTypeToken>0x%08x</DeclaringTypeToken>\r\n"), declaringTypeToken);

    tstring strRetValType = GetCorElementTypeString(pReturnType);
    CLogging::LogDumpMessage(_T("    <RetValueCorElementType>%s</RetValueCorElementType>\r\n"), strRetValType.c_str());
    CLogging::LogDumpMessage(_T("    <RetTypeToken>0x%08x</RetTypeToken>\r\n"), retTypeToken);

    CLogging::LogDumpMessage(_T("    <CorSignature>"));
    for (DWORD i = 0; i < cbCorSigActual; i++)
    {
        CLogging::LogDumpMessage(_T("0x%02x "), pCorSig[i]);
    }
    CLogging::LogDumpMessage(_T("</CorSignature>\r\n"), retTypeToken);
    CLogging::LogDumpMessage(_T("    <CorMethodAttr>0x%08x</CorMethodAttr>\r\n"), corMethodAttr);
    CLogging::LogDumpMessage(_T("    <CodeRVA>0x%08x</CodeRVA>\r\n"), rva);
    CLogging::LogDumpMessage(_T("    <MethodImpl>0x%08x</MethodImpl>\r\n"), methodImpl);
    CLogging::LogDumpMessage(_T("    <LocalSignature>"));
    for (DWORD i = 0; i < dwLocalSigCount; i++)
    {
        CLogging::LogDumpMessage(_T("0x%02x "), pLocalSig[i]);
    }
    CLogging::LogDumpMessage(_T("</LocalSignature>\r\n"));
    CLogging::LogDumpMessage(_T("    <MaxStack>0x%08x</MaxStack>\r\n"), maxStack);

    if (SUCCEEDED(this->GetInstrumentationResults(&pInstructionGraph, &pExceptionSection, (COR_IL_MAP**)&pCorILMap, &dwCorILMapmLen)))
    {
        LogInstructionGraph(pInstructionGraph);
        LogExceptionSection(pExceptionSection);
        LogCorIlMap(pCorILMap, dwCorILMapmLen);
    }

    CLogging::LogDumpMessage(_T("</InstrumentedMethod>\r\n"));

    s_bIsDumpingMethod = false;
}


// Print the cor element type and modifers for an IType.
// NOTE: IType will have already removed any modifier prefixes.
// static
tstring MicrosoftInstrumentationEngine::CMethodInfo::GetCorElementTypeString(_In_ IType* pType)
{
    CorElementType corElementType;
    pType->GetCorElementType(&corElementType);

    tstring strElementType;

    if (ELEMENT_TYPE_VOID == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_VOID");
    }
    else if (ELEMENT_TYPE_BOOLEAN == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_BOOLEAN");
    }
    else if (ELEMENT_TYPE_CHAR == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_CHAR");
    }
    else if (ELEMENT_TYPE_I1 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_I1");
    }
    else if (ELEMENT_TYPE_U1 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_U1");
    }
    else if (ELEMENT_TYPE_I2 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_I2");
    }
    else if (ELEMENT_TYPE_U2 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_U2");
    }
    else if (ELEMENT_TYPE_I4 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_I4");
    }
    else if (ELEMENT_TYPE_U4 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_U4");
    }
    else if (ELEMENT_TYPE_I8 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_I8");
    }
    else if (ELEMENT_TYPE_U8 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_U8");
    }
    else if (ELEMENT_TYPE_R4 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_R4");
    }
    else if (ELEMENT_TYPE_R8 == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_R8");
    }
    else if (ELEMENT_TYPE_STRING == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_STRING");
    }
    else if (ELEMENT_TYPE_PTR == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_PTR");
    }
    else if (ELEMENT_TYPE_BYREF == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_BYREF");
    }
    else if (ELEMENT_TYPE_VALUETYPE == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_VALUETYPE");
    }
    else if (ELEMENT_TYPE_CLASS == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_CLASS");
    }
    else if (ELEMENT_TYPE_VAR == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_VAR");
    }
    else if (ELEMENT_TYPE_ARRAY == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_ARRAY");
    }
    else if (ELEMENT_TYPE_GENERICINST == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_GENERICINST");
    }
    else if (ELEMENT_TYPE_TYPEDBYREF == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_TYPEDBYREF");
    }
    else if (ELEMENT_TYPE_I == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_I");
    }
    else if (ELEMENT_TYPE_U == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_U");
    }
    else if (ELEMENT_TYPE_FNPTR == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_FNPTR");
    }
    else if (ELEMENT_TYPE_OBJECT == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_OBJECT");
    }
    else if (ELEMENT_TYPE_SZARRAY == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_SZARRAY");
    }
    else if (ELEMENT_TYPE_MVAR == corElementType)
    {
        strElementType = _T("ELEMENT_TYPE_MVAR");
    }
    else
    {
        strElementType = _T("Unknown CorElementType");
    }

    return strElementType;
}



// Obtain the final version of the method body. This can be in one of two buffers depending on if the instrumentation methods,
// or the raw profiler callback, or both instrumented the function.
// NOTE: a seperate buffer is needed because the clr requires a callee destroyed buffer in the rejit cases, and
// some hosts, including MMA, will try to consume the buffer after the set to calculate the cor il map.
// using the same buffer on set would invalidate the pointer that such hosts already obtained.
HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetFinalInstrumentation(_Out_ DWORD* pcbMethodBody, _Out_ BYTE** ppMethodBody)
{
    HRESULT hr = S_OK;
    IfNullRet(pcbMethodBody);
    IfNullRet(ppMethodBody);

    if (m_pFinalRenderedMethod == nullptr)
    {
        // raw profiler did not instrument the method. Use the intermediate rendered method
        // from the instrumentation methods.
        *pcbMethodBody = m_cbIntermediateRenderedMethod;
        *ppMethodBody = m_pIntermediateRenderedMethod;
    }
    else
    {
        // raw profiler did instrument the method. Use the final rendered method
        *pcbMethodBody = m_cbFinalRenderedMethod;
        *ppMethodBody = m_pFinalRenderedMethod;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::MergeILInstrumentedCodeMap(
    _In_ ULONG cILMapEntries,
    _In_reads_(cILMapEntries) COR_IL_MAP rgILMapEntries[]
    )
{
    HRESULT hr = S_OK;

    CLogging::LogMessage(_T("Start CMethodInfo::MergeILInstrumentedCodeMap"));

    if (!m_pCorILMap.IsEmpty())
    {
        // No need to merge if we are just setting to the exact same pointer.
        if (rgILMapEntries != m_pCorILMap.Get())
        {
            // The raw callbacks will assume the updated il stream that came out of the instrumentation methods and/or
            // other raw callbacks. The maps must be merged.
            //
            // Merge algorithm is as follows:
            // 1) For each entry in the new map, look up the original offset in the old map's new offsets
            // 2) If a match is found, replace the old offset in the new map with the original offset from the old map.
            // 3) If no match is found, then this offset did not exist in the original map and its entry should be removed.

            // First index the old entries by new offset.
            std::unordered_map<ULONG32, COR_IL_MAP*> oldMapNewOffsetToEntryMap;
            size_t currEntries = m_pCorILMap.Count();
            for (DWORD i = 0; i < currEntries; i++)
            {
                COR_IL_MAP* pCurr = &(m_pCorILMap[i]);
                oldMapNewOffsetToEntryMap[pCurr->newOffset] = pCurr;
            }

            // Next, iterate over the new map doing the conversion.
            std::vector<COR_IL_MAP*> updatedEntries;
            for (DWORD k = 0; k < cILMapEntries; k++)
            {
                COR_IL_MAP* pCurr = &(rgILMapEntries[k]);

                std::unordered_map<ULONG32, COR_IL_MAP*>::iterator iter = oldMapNewOffsetToEntryMap.find(pCurr->oldOffset);

                if (iter != oldMapNewOffsetToEntryMap.end())
                {
                    COR_IL_MAP* pOldEntry = iter->second;

                    // Entry was found in the original map.
                    pCurr->oldOffset = pOldEntry->oldOffset;

                    updatedEntries.push_back(pCurr);
                }
                else
                {
                    // No entry was found. Skip this entry. It must have been a new instruction in the old map.
                }
            }

            m_pCorILMap = CSharedArray<COR_IL_MAP>(updatedEntries.size());

            for (DWORD x = 0; x < m_pCorILMap.Count(); x++)
            {
                m_pCorILMap[x] = *(updatedEntries[x]);
            }
        }
    }
    else
    {
        m_pCorILMap = CSharedArray<COR_IL_MAP>(cILMapEntries);
        size_t capacity = cILMapEntries * sizeof(COR_IL_MAP);
        IfFailRetErrno(memcpy_s(m_pCorILMap.Get(), capacity, rgILMapEntries, capacity));
    }

    // Save the map with the module info so that it can be retrieved later by the JIT callbacks.
    m_pModuleInfo->SetILInstrumentationMap(this, m_pCorILMap);
    CLogging::LogMessage(_T("End CMethodInfo::MergeILInstrumentedCodeMap"));

    return S_OK;
}

// Recreate the instruction graph, exception section, and cor_il_map from the
// raw results. This is used by the diagnostic logging engine.
HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetInstrumentationResults(
    _Out_ CInstructionGraph** ppInstructionGraph,
    _Out_ CExceptionSection** ppExceptionSection,
    _Out_ COR_IL_MAP** ppCorILMap,
    _Out_ DWORD* dwCorILMapmLen
    )
{
    HRESULT hr = S_OK;

    DWORD cbMethodBody = 0;
    BYTE* pbMethodBody = nullptr;
    IfFailRet(GetFinalInstrumentation(&cbMethodBody, &pbMethodBody));

    IMAGE_COR_ILMETHOD* pMethodHeader = (IMAGE_COR_ILMETHOD*)(pbMethodBody);
    ULONG cbMethodSize = cbMethodBody;

    LPCBYTE pMethodBody;
    ULONG codeSize = 0;

    if (((COR_ILMETHOD *)pMethodHeader)->Tiny.IsTiny())
    {
        pMethodBody = (LPCBYTE) pMethodHeader + sizeof (IMAGE_COR_ILMETHOD_TINY);
        codeSize = ((COR_ILMETHOD_TINY*)pMethodHeader)->GetCodeSize();
    }
    else
    {
        pMethodBody = (LPCBYTE)pMethodHeader + sizeof(IMAGE_COR_ILMETHOD_FAT);
        codeSize = ((COR_ILMETHOD_FAT*)pMethodHeader)->GetCodeSize();
    }

    LPCBYTE pMethodEnd = pMethodBody + codeSize;

    CComPtr<CInstructionGraph> pInstructionGraph;
    pInstructionGraph.Attach(new CInstructionGraph);
    pInstructionGraph->Initialize(this);

    IfFailRet(pInstructionGraph->DecodeInstructions(pMethodBody, pMethodEnd));

    // Initialize the exception section;
    // The instruction graph is passed in directly rather than being obtained from the method info
    // because during the diagnostic dumping logic, the graph will be from the rendered function
    // body and not directly from the method info.
    CComPtr<CExceptionSection> pExceptionSection;
    pExceptionSection.Attach(new CExceptionSection(this));
    IfFailRet(pExceptionSection->Initialize(pMethodHeader, cbMethodSize, pInstructionGraph));

    *ppInstructionGraph = pInstructionGraph.Detach();
    *ppExceptionSection = pExceptionSection.Detach();

    *ppCorILMap = new COR_IL_MAP[m_pCorILMap.Count()];
    size_t capacity = m_pCorILMap.Count() * sizeof(COR_IL_MAP);
    IfFailRetErrno(memcpy_s(*ppCorILMap, capacity, m_pCorILMap.Get(), capacity));
    *dwCorILMapmLen = (DWORD)m_pCorILMap.Count();

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetRejitCount(_Out_ DWORD* pdwRejitCount)
{
    HRESULT hr = S_OK;

    IfNullRetPointer(pdwRejitCount);
    *pdwRejitCount = 0;

    // The method info only exists during the rejit or first instrumentation pass, so
    // there can only be one instance alive at a time. The critical section in top level
    // of the instrumentation callbacks makes this a requirement.
    IfFailRet(m_pModuleInfo->GetMethodRejitCount(m_tkFunction, pdwRejitCount));

    return hr;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetMaxStack(_Out_ DWORD* pMaxStack)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pMaxStack);

    // THREADING: methodinfo only exists during a profiler callback and is protected by the top-level critical section.
    if (m_pInstructionGraph == NULL)
    {
        IfFailRet(InitializeInstructionsAndExceptions());
    }

    // NOTE: not caching the max stack value because it can change at any time. I expect few callers of this if any.
    IfFailRet(m_pInstructionGraph->CalculateMaxStack(pMaxStack));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CMethodInfo::GetSingleRetDefaultInstrumentation(_Out_ ISingleRetDefaultInstrumentation** ppSingleRetDefaultInstrumentation)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppSingleRetDefaultInstrumentation);

    ATL::CComPtr<ISingleRetDefaultInstrumentation> spSingleRetDefaultInstrumentation(new MicrosoftInstrumentationEngine::CSingleRetDefaultInstrumentation());

    *ppSingleRetDefaultInstrumentation = spSingleRetDefaultInstrumentation;
    return S_OK;
}


MicrosoftInstrumentationEngine::CModuleInfo* MicrosoftInstrumentationEngine::CMethodInfo::GetModuleInfo()
{
    return m_pModuleInfo;
}

bool MicrosoftInstrumentationEngine::CMethodInfo::IsCreateBaselineEnabled()
{
    return m_bIsCreateBaselineEnabled;
}

void MicrosoftInstrumentationEngine::CMethodInfo::DisableCreateBaseline()
{
    m_bIsCreateBaselineEnabled = false;
}