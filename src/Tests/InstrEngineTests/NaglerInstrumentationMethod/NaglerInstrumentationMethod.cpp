// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "NaglerInstrumentationMethod.h"
#pragma warning(push)
#pragma warning(disable: 4995) // disable so that memcpy, wmemcpy can be used
#include <sstream>
#pragma warning(pop)
#include "Util.h"
#include "InstrumentationEngineString.h"
#include "InstrStr.h"
#include "../../inc/clr/prof/corprof.h"

const WCHAR CInstrumentationMethod::TestOutputPathEnvName[] = L"Nagler_TestOutputPath";
const WCHAR CInstrumentationMethod::TestScriptFileEnvName[] = L"Nagler_TestScript";
const WCHAR CInstrumentationMethod::TestScriptFolder[] = L"TestScripts";
const WCHAR CInstrumentationMethod::IsRejitEnvName[] = L"Nagler_IsRejit";


#ifdef _WIN64
LPCWSTR k_wszEnteredFunctionProbeName = L"OnEnter";
LPCWSTR k_wszExitedFunctionProbeName = L"OnExit";
#else // Win32
LPCWSTR k_wszEnteredFunctionProbeName = L"OnEnter";
LPCWSTR k_wszExitedFunctionProbeName = L"OnExit";
#endif

// When pumping managed helpers into mscorlib, stick them into this pre-existing mscorlib type
LPCWSTR k_wszHelpersContainerType = L"System.CannotUnloadAppDomainException";

LPCWSTR ManagedTracingApiAssemblyNameClassic = L"Instana.ManagedTracing.Api";
LPCWSTR ManagedTracingAssemblyNameClassic = L"Instana.ManagedTracing.Base";
LPCWSTR ManagedTracingTracerClassNameClassic = L"Instana.ManagedTracing.Base.Tracer";

LPCWSTR ManagedTracingAssemblyNameCore = L"Instana.Tracing.Core";
LPCWSTR ManagedTracingAssemblyNameCoreCommon = L"Instana.Tracing.Core.Common";
LPCWSTR ManagedTracingAssemblyNameCoreTransport = L"Instana.Tracing.Core.Transport";
LPCWSTR ManagedTracingAssemblyNameApi = L"Instana.Tracing.Api";
LPCWSTR ManagedTracingTracerClassNameCore = L"Instana.Tracing.Tracer";

// Convenience macro for defining strings.
#define InstrStr(_V) CInstrumentationEngineString _V(m_pStringManager)

void AssertLogFailure(_In_ const WCHAR* wszError, ...)
{
    // Since this is a test libary, we will print to standard error and fail
    va_list argptr;
    va_start(argptr, wszError);
    vfwprintf(stderr, wszError, argptr);
    throw "Assert failed";
}

ILOpcodeInfo ilOpcodeInfo[] =
{
#define OPDEF(ord, code, name,  opcodeLen, operandLen, type, alt, flags, pop, push) \
    { name, (DWORD)opcodeLen, (DWORD)operandLen, ##type, alt, flags, (DWORD)pop, (DWORD)push},
#include "ILOpcodes.h"
#include "ModuleInfo.h"
#undef OPDEF
};

struct ComInitializer
{
    HRESULT _hr;
    ComInitializer(DWORD mode = COINIT_APARTMENTTHREADED)
    {
        _hr = CoInitializeEx(NULL, mode);
    }
    ~ComInitializer()
    {
        if (SUCCEEDED(_hr))
        {
            CoUninitialize();
        }
    }
};

HRESULT CInstrumentationMethod::Initialize(_In_ IProfilerManager* pProfilerManager)
{
    CComPtr<IProfilerManagerLogging> spGlobalLogger;
    CComQIPtr<IProfilerManager4> pProfilerManager4 = pProfilerManager;
    pProfilerManager4->GetGlobalLoggingInstance(&spGlobalLogger);

    spGlobalLogger->LogDumpMessage(_T("Test module initialize"));

    HRESULT hr;
    m_pProfilerManager = pProfilerManager;
    IfFailRet(m_pProfilerManager->QueryInterface(&m_pStringManager));
    spGlobalLogger->LogDumpMessage(_T("Test module QueryInterface"));

    CHandle hConfigThread;
    hConfigThread.Attach(CreateThread(NULL, 0, InstrumentationMethodThreadProc, this, 0, NULL));

    spGlobalLogger->LogDumpMessage(_T("Test module Attach"));

    DWORD retVal = WaitForSingleObject(hConfigThread, 15000);

    spGlobalLogger->LogDumpMessage(_T("Test module Thread is done"));

    if (m_bTestInstrumentationMethodLogging)
    {
        
        spGlobalLogger->LogDumpMessage(_T("<InstrumentationMethodLog>"));

        CComPtr<IProfilerManagerLogging> spLogger;
        pProfilerManager->GetLoggingInstance(&spLogger);
        spGlobalLogger->LogDumpMessage(_T("<Message>"));
        spLogger->LogMessage(_T("Message."));
        spGlobalLogger->LogDumpMessage(_T("</Message>"));

        spGlobalLogger->LogDumpMessage(_T("<Dump>"));
        spLogger->LogDumpMessage(_T("Success!"));
        spGlobalLogger->LogDumpMessage(_T("</Dump>"));

        spGlobalLogger->LogDumpMessage(_T("<Error>"));
        spLogger->LogError(_T("Error."));
        spGlobalLogger->LogDumpMessage(_T("</Error>"));
    }
    else if (m_bExceptionTrackingEnabled)
    {
        CComPtr<ICorProfilerInfo> pCorProfilerInfo;
        pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);
        pCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_ENABLE_STACK_SNAPSHOT);

        CComPtr<IProfilerManagerLogging> spLogger;
        pProfilerManager->GetLoggingInstance(&spLogger);
        spLogger->LogDumpMessage(_T("<ExceptionTrace>"));
    }

    return S_OK;
}

// The CLR doesn't initialize com before calling the profiler, and the profiler manager cannot do so itself
// as that would screw up the com state for the application's thread. Spin up a new thread to allow the instrumentation
// method to co create a free threaded version of msxml on a thread that it owns to avoid this.
DWORD WINAPI CInstrumentationMethod::InstrumentationMethodThreadProc(
    _In_  LPVOID lpParameter
)
{
    ComInitializer coInit;
    CInstrumentationMethod* pThis = (CInstrumentationMethod*)lpParameter;

    if (FAILED(pThis->LoadTestScript()))
    {
        return (DWORD)-1;
    }

    return 0;
}

HRESULT CInstrumentationMethod::LoadTestScript()
{
    HRESULT hr = S_OK;

    WCHAR testScript[MAX_PATH] = { 0 };
    DWORD dwRes = GetEnvironmentVariableW(TestScriptFileEnvName, testScript, MAX_PATH);
    if (dwRes == 0)
    {
        ATLASSERT(!L"Failed to get testScript file name");
        return E_FAIL;
    }

    WCHAR testOutputPath[MAX_PATH] = { 0 };
    dwRes = GetEnvironmentVariableW(TestOutputPathEnvName, testOutputPath, MAX_PATH);
    if (dwRes == 0)
    {
        ATLASSERT(!L"Failed to get test path");
        return E_FAIL;
    }
    m_strBinaryDir = testOutputPath;
    CComPtr<CXmlDocWrapper> pDocument;
    pDocument.Attach(new CXmlDocWrapper());

    hr = pDocument->LoadFile(testScript);
    if (hr != S_OK)
    {
        ATLASSERT(!L"Failed to load test configuration");
        return -1;
    }

    CComPtr<CXmlNode> pDocumentNode;
    IfFailRet(pDocument->GetRootNode(&pDocumentNode));

    tstring strDocumentNodeName;
    IfFailRet(pDocumentNode->GetName(strDocumentNodeName));

    if (wcscmp(strDocumentNodeName.c_str(), L"InstrumentationTestScript") != 0)
    {
        return -1;
    }


    CComPtr<CXmlNode> pChildNode;
    IfFailRet(pDocumentNode->GetChildNode(&pChildNode));

    while (pChildNode != nullptr)
    {
        tstring strCurrNodeName;
        IfFailRet(pChildNode->GetName(strCurrNodeName));

        if (wcscmp(strCurrNodeName.c_str(), L"InstrumentMethod") == 0)
        {
            IfFailRet(ProcessInstrumentMethodNode(pChildNode));
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"ExceptionTracking") == 0)
        {
            m_bExceptionTrackingEnabled = true;
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"InjectAssembly") == 0)
        {
            shared_ptr<CInjectAssembly> spNewInjectAssembly(new CInjectAssembly());
            ProcessInjectAssembly(pChildNode, spNewInjectAssembly);
            m_spInjectAssembly = spNewInjectAssembly;
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"MethodLogging") == 0)
        {
            m_bTestInstrumentationMethodLogging = true;
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"#comment") == 0)
        {
            // do nothing
        }
        else
        {
            ATLASSERT(!L"Invalid configuration. Element should be InstrumentationMethod");
            return -1;
        }

        pChildNode = pChildNode->Next();
    }

    return 0;
}

HRESULT CInstrumentationMethod::ProcessInstrumentMethodNode(CXmlNode* pNode)
{
    HRESULT hr = S_OK;

    WCHAR isRejitEnvValue[MAX_PATH];
    DWORD dwRes = GetEnvironmentVariableW(IsRejitEnvName, isRejitEnvValue, MAX_PATH);
    if (dwRes == 0)
    {
        ATLASSERT(!L"Failed to get isrejit variable");
        return E_FAIL;
    }

    bool rejitAllInstruMethods = (wcscmp(isRejitEnvValue, L"True") == 0);

    tstring moduleName;
    tstring methodName;
    BOOL bIsRejit = rejitAllInstruMethods;
    vector<shared_ptr<CInstrumentInstruction>> instructions;
    vector<CLocalType> locals;
    vector<COR_IL_MAP> corIlMap;
    BOOL isReplacement = FALSE;
    BOOL isSingleRetFirst = FALSE;
    BOOL isSingleRetLast = FALSE;
    BOOL isAddExceptionHandler = FALSE;
    shared_ptr<CInstrumentMethodPointTo> spPointTo(nullptr);

    CComPtr<CXmlNode> pChildNode;
    IfFailRet(pNode->GetChildNode(&pChildNode));

    while (pChildNode != nullptr)
    {
        tstring strCurrNodeName;
        IfFailRet(pChildNode->GetName(strCurrNodeName));

        if (wcscmp(strCurrNodeName.c_str(), L"ModuleName") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            IfFailRet(pChildValue->GetStringValue(moduleName));
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"MethodName") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));
            IfFailRet(pChildValue->GetStringValue(methodName));
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"Instructions") == 0)
        {
            tstring baselineAttr;
            pChildNode->GetAttribute(L"Baseline", baselineAttr);
            isReplacement = !baselineAttr.empty();

            ProcessInstructionNodes(pChildNode, instructions, isReplacement != 0);
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"IsRejit") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            tstring nodeValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));
            IfFailRet(pChildValue->GetStringValue(nodeValue));
            bIsRejit = (wcscmp(nodeValue.c_str(), L"True") == 0);
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"AddExceptionHandler") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring nodeValue;
            IfFailRet(pChildValue->GetStringValue(nodeValue));
            isAddExceptionHandler = (wcscmp(nodeValue.c_str(), L"True") == 0);
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"CorIlMap") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring varNodeValue;
            IfFailRet(pChildValue->GetStringValue(varNodeValue));

            wstringstream corIlMapString(varNodeValue);
            DWORD oldOffset = 0;
            DWORD newOffset = 0;

            while ((bool)(corIlMapString >> oldOffset >> newOffset))
            {
                COR_IL_MAP map;
                map.fAccurate = true;
                map.oldOffset = oldOffset;
                map.newOffset = newOffset;
                corIlMap.push_back(map);
            }
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"Locals") == 0)
        {
            ProcessLocals(pChildNode, locals);
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"PointTo") == 0)
        {
            std::shared_ptr<CInstrumentMethodPointTo> spPointer(new CInstrumentMethodPointTo());
            spPointTo = spPointer;
            ProcessPointTo(pChildNode, *spPointTo);
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"MakeSingleRet") == 0)
        {
            // Allow the single return instrumentation to execute
            // both before and after the instruction instrumentation.
            if (instructions.empty())
            {
                isSingleRetFirst = true;
            }
            else
            {
                isSingleRetLast = true;
            }
        }
        else if (wcscmp(strCurrNodeName.c_str(), L"#comment") == 0)
        {
            // do nothing. Get next node.
        }
        else
        {
            ATLASSERT(!L"Invalid configuration. Unknown Element");
            return E_FAIL;
        }

        pChildNode = pChildNode->Next();
    }

    if ((moduleName.empty()) ||
        (methodName.empty()))
    {
        ATLASSERT(!L"Invalid configuration. Missing child element");
        return E_FAIL;
    }

    // if it is a replacement method, there will be no instructions
    // do the single ret after the method is replaced.
    if (isReplacement && isSingleRetFirst)
    {
        isSingleRetFirst = false;
        isSingleRetLast = true;
    }

    shared_ptr<CInstrumentMethodEntry> pMethod = make_shared<CInstrumentMethodEntry>(moduleName, methodName, bIsRejit, isSingleRetFirst, isSingleRetLast, isAddExceptionHandler);
    if (spPointTo != nullptr)
    {
        pMethod->SetPointTo(spPointTo);
    }
    pMethod->AddInstrumentInstructions(instructions);
    if (corIlMap.size() > 0)
    {
        pMethod->AddCorILMap(corIlMap);
    }
    pMethod->SetReplacement(isReplacement);
    m_instrumentMethodEntries.push_back(pMethod);
    pMethod->AddLocals(locals);
    return S_OK;
}

HRESULT CInstrumentationMethod::ProcessPointTo(CXmlNode* pNode, CInstrumentMethodPointTo& pointTo)
{
    HRESULT hr = S_OK;

    CComPtr<CXmlNode> pChildNode;
    IfFailRet(pNode->GetChildNode(&pChildNode));

    while (pChildNode != nullptr)
    {
        //<AssemblyName>InjectToMscorlibTest_Release</AssemblyName>
        //<TypeName>InstruOperationsTests.Program</TypeName>
        //<MethodName>MethodToCallInstead</MethodName>
        tstring nodeName;
        IfFailRet(pChildNode->GetName(nodeName));
        if (wcscmp(nodeName.c_str(), L"#comment") == 0)
        {
            // do nothing, get next node.
        }
        else if (wcscmp(nodeName.c_str(), L"AssemblyName") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring varNodeValue;
            IfFailRet(pChildValue->GetStringValue(pointTo.m_assemblyName));
        }
        else if (wcscmp(nodeName.c_str(), L"TypeName") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring varNodeValue;
            IfFailRet(pChildValue->GetStringValue(pointTo.m_typeName));
        }
        else if (wcscmp(nodeName.c_str(), L"MethodName") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring varNodeValue;
            IfFailRet(pChildValue->GetStringValue(pointTo.m_methodName));
        }

        pChildNode = pChildNode->Next();
    }
    return hr;
}

HRESULT CInstrumentationMethod::ProcessInjectAssembly(CXmlNode* pNode, shared_ptr<CInjectAssembly>& injectAssembly)
{
    HRESULT hr = S_OK;

    CComPtr<CXmlNode> pChildNode;
    IfFailRet(pNode->GetChildNode(&pChildNode));

    while (pChildNode != nullptr)
    {
        //<Source>C:\tfs\pioneer2\VSClient1\src\edev\diagnostics\intellitrace\InstrumentationEngine\Tests\InstrEngineTests\Binary\InjectToMscorlibTest_Debug.exe</Source>
        //<Target>mscorlib</Target>
        tstring nodeName;
        IfFailRet(pChildNode->GetName(nodeName));
        if (wcscmp(nodeName.c_str(), L"#comment") == 0)
        {
            // do nothing. Get next node.
        }
        else if (wcscmp(nodeName.c_str(), L"Target") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            IfFailRet(pChildValue->GetStringValue(injectAssembly->m_targetAssemblyName));
        }
        else if (wcscmp(nodeName.c_str(), L"Source") == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring varNodeValue;
            IfFailRet(pChildValue->GetStringValue(injectAssembly->m_sourceAssemblyName));
        }

        pChildNode = pChildNode->Next();
    }
    return hr;
}

HRESULT CInstrumentationMethod::ProcessLocals(CXmlNode* pNode, vector<CLocalType>& locals)
{
    HRESULT hr = S_OK;

    CComPtr<CXmlNode> pChildNode;;
    IfFailRet(pNode->GetChildNode(&pChildNode));

    while (pChildNode != nullptr)
    {
        tstring nodeName;
        IfFailRet(pChildNode->GetName(nodeName));
        if (wcscmp(nodeName.c_str(), L"#comment") == 0)
        {
           // do nothing.
        }
        else
        {
            if (wcscmp(nodeName.c_str(), L"Local") != 0)
            {
                ATLASSERT(!L"Invalid element");
                return E_UNEXPECTED;
            }

            tstring typeName;
            IfFailRet(pChildNode->GetAttribute(L"Type", typeName));

            locals.push_back(CLocalType(typeName));
        }

        pChildNode = pChildNode->Next();
    }
    return hr;
}

HRESULT CInstrumentationMethod::ProcessInstructionNodes(CXmlNode* pNode, vector<shared_ptr<CInstrumentInstruction>>& instructions, bool isBaseline)
{
    HRESULT hr = S_OK;

    CComPtr<CXmlNode> pChildNode;
    IfFailRet(pNode->GetChildNode(&pChildNode));

    while (pChildNode != nullptr)
    {
        tstring currNodeName;
        IfFailRet(pChildNode->GetName(currNodeName));

        if (wcscmp(currNodeName.c_str(), L"#comment") == 0)
        {
            // do nothing
        }
        else if (wcscmp(currNodeName.c_str(), L"Instruction") == 0)
        {
            CComPtr<CXmlNode> pInstructionChildNode;
            IfFailRet(pChildNode->GetChildNode(&pInstructionChildNode));

            bool bOpcodeSet = false;
            bool bOffsetSet = false;
            DWORD dwOffset = 0;
            ILOrdinalOpcode opcode;
            ILOpcodeInfo opcodeInfo;
            InstrumentationType instrType = InsertBefore;
            DWORD dwRepeatCount = 1;

            // NOTE: only support integer operands for the time being.
            INT64 operand = 0;
            while (pInstructionChildNode != nullptr)
            {
                tstring currInstructionNodeName;
                IfFailRet(pInstructionChildNode->GetName(currInstructionNodeName));

                if (wcscmp(currInstructionNodeName.c_str(), L"Opcode") == 0)
                {
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strOpcode;
                    IfFailRet(pChildValue->GetStringValue(strOpcode));

                    IfFailRet(ConvertOpcode(strOpcode.c_str(), &opcode, &opcodeInfo));
                    bOpcodeSet = true;
                }
                else if (wcscmp(currInstructionNodeName.c_str(), L"Offset") == 0)
                {
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strOffset;
                    IfFailRet(pChildValue->GetStringValue(strOffset));
                    dwOffset = _wtoi(strOffset.c_str());
                    bOffsetSet = true;
                }
                else if (wcscmp(currInstructionNodeName.c_str(), L"Operand") == 0)
                {
                    if (opcodeInfo.m_operandLength == 0)
                    {
                        ATLASSERT(!L"Invalid configuration. Opcode should not have an operand");
                    }
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strOperand;
                    IfFailRet(pChildValue->GetStringValue(strOperand));
                    operand = _wtoi64(strOperand.c_str());

                    // For now, only support integer operands
                    ATLASSERT(opcodeInfo.m_type == ILOperandType_None ||
                        opcodeInfo.m_type == ILOperandType_Byte ||
                        opcodeInfo.m_type == ILOperandType_Int ||
                        opcodeInfo.m_type == ILOperandType_UShort ||
                        opcodeInfo.m_type == ILOperandType_Long ||
                        opcodeInfo.m_type == ILOperandType_Token
                        );


                }
                else if (wcscmp(currInstructionNodeName.c_str(), L"InstrumentationType") == 0)
                {
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strInstrType;
                    IfFailRet(pChildValue->GetStringValue(strInstrType));

                    if (wcscmp(strInstrType.c_str(), L"InsertBefore") == 0)
                    {
                        instrType = InsertBefore;
                    }
                    else if (wcscmp(strInstrType.c_str(), L"InsertAfter") == 0)
                    {
                        instrType = InsertAfter;
                    }
                    else if (wcscmp(strInstrType.c_str(), L"Replace") == 0)
                    {
                        instrType = Replace;
                    }
                    else if (wcscmp(strInstrType.c_str(), L"Remove") == 0)
                    {
                        instrType = Remove;
                    }
                    else if (wcscmp(strInstrType.c_str(), L"RemoveAll") == 0)
                    {
                        instrType = RemoveAll;
                    }
                }
                else if (wcscmp(currInstructionNodeName.c_str(), L"RepeatCount") == 0)
                {
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strRepeatCount;
                    IfFailRet(pChildValue->GetStringValue(strRepeatCount));

                    dwRepeatCount = _wtoi(strRepeatCount.c_str());
                }
                else
                {
                    ATLASSERT(!L"Invalid configuration. Unknown Element");
                    return E_FAIL;
                }

                pInstructionChildNode = pInstructionChildNode->Next();
            }

            if ( (!isBaseline) && (instrType != Remove) && (instrType != RemoveAll) && (!bOpcodeSet || !bOffsetSet))
            {
                ATLASSERT(!L"Invalid configuration. Instruction must have an offset");
                return E_FAIL;
            }

            if (((instrType == Replace) || (instrType == Remove) || (instrType == RemoveAll)) && (dwRepeatCount != 1))
            {
                ATLASSERT(!L"Invalid configuration. Incorrect repeat count");
                return E_FAIL;
            }

            shared_ptr<CInstrumentInstruction> pInstrumentInstruction = make_shared<CInstrumentInstruction>(dwOffset, opcode, opcodeInfo, operand, instrType, dwRepeatCount);
            instructions.push_back(pInstrumentInstruction);
        }
        else
        {
            ATLASSERT(!L"Invalid configuration. Unknown Element");
            return E_FAIL;
        }

        pChildNode = pChildNode->Next();
    }
    return S_OK;
}

HRESULT CInstrumentationMethod::ConvertOpcode(LPCWSTR zwOpcode, ILOrdinalOpcode* pOpcode, ILOpcodeInfo* pOpcodeInfo)
{
    // Horribly slow solution to mapping opcode name to opcode.
    // This should be okay though since the tests should only have a few instructions

    const DWORD cOpcodes = 0x0124;
    for (DWORD i = 0; i < 0x0124; i++)
    {
        if (wcscmp(ilOpcodeInfo[i].m_name, zwOpcode) == 0)
        {
            *pOpcode = (ILOrdinalOpcode)i;
            *pOpcodeInfo = ilOpcodeInfo[i];
            break;
        }
    }
    return S_OK;
}

HRESULT CInstrumentationMethod::OnAppDomainCreated(
    _In_ IAppDomainInfo *pAppDomainInfo)
{
    return S_OK;
}

HRESULT CInstrumentationMethod::OnAppDomainShutdown(
    _In_ IAppDomainInfo *pAppDomainInfo)
{
    return S_OK;
}

HRESULT CInstrumentationMethod::OnAssemblyLoaded(_In_ IAssemblyInfo* pAssemblyInfo)
{
    return S_OK;
}

HRESULT CInstrumentationMethod::OnAssemblyUnloaded(_In_ IAssemblyInfo* pAssemblyInfo)
{
    return S_OK;
}


ModuleInfo moduleInfo;
HRESULT CInstrumentationMethod::OnModuleLoaded(_In_ IModuleInfo* pModuleInfo)
{
        CComPtr<IProfilerManagerLogging> spLogger;
        CComQIPtr<IProfilerManager4> pProfilerManager4 = m_pProfilerManager;
        pProfilerManager4->GetGlobalLoggingInstance(&spLogger);
    try
    {
        ModuleID moduleID;
        pModuleInfo->GetModuleID(&moduleID);

        

        LPCBYTE pbBaseLoadAddr;
        WCHAR wszName[512];
        ULONG cchNameIn = _countof(wszName);
        ULONG cchNameOut;
        AssemblyID assemblyID;
        DWORD dwModuleFlags;
        HRESULT hr = E_FAIL;

        spLogger->LogMessage(_T("Krecemoooo!!!!"));

        ICorProfilerInfo4* m_pProfilerInfo;
        try
        {

            hr = m_pProfilerManager->GetCorProfilerInfo((IUnknown**)&m_pProfilerInfo);
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("Failed!!!!"));
                return hr;
            }
                hr = m_pProfilerInfo->GetModuleInfo2(
                moduleID,
                &pbBaseLoadAddr,
                cchNameIn,
                &cchNameOut,
                wszName,
                &assemblyID,
                &dwModuleFlags);
        }
        catch (...)
        {
            spLogger->LogMessage(_T("Error getting ModuleInfo"));
            return S_OK;
        }
        if (hr != S_OK)
        {
            spLogger->LogMessage(_T("Could not get Module info!"));
        }

        if (FAILED(hr))
        {
            spLogger->LogMessage(_T("GetModuleInfo failed for module {}", moduleID));
            return S_OK;
        }

        // adjust the buffer's size, call again
        if (cchNameIn <= cchNameOut)
        {
            spLogger->LogMessage(_T("The reserved buffer for the module's name was not large enough. Skipping."));
            return S_OK;
        }

        std::string logMessage2("Module loaded:  ");
        std::wstring lm2 = std::wstring(logMessage2.begin(), logMessage2.end());
        lm2 = lm2 + wszName;
        spLogger->LogMessage(lm2.c_str());

        if ((dwModuleFlags & COR_PRF_MODULE_WINDOWS_RUNTIME) != 0)
        {
            // Ignore any Windows Runtime modules.  We cannot obtain writeable metadata
            // interfaces on them or instrument their IL
            spLogger->LogMessage(_T("This is a runtime windows module, skipping"));
            return S_OK;
        }

        AppDomainID appDomainID;
        ModuleID modIDDummy;
        WCHAR assemblyName[255];
        ULONG assemblyNameLength = 0;

        hr = m_pProfilerInfo->GetAssemblyInfo(
            assemblyID,
            _countof(assemblyName),          // cchName,
            &assemblyNameLength,       // pcchName,
            assemblyName,       // szName[] ,
            &appDomainID,
            &modIDDummy);

        if (FAILED(hr))
        {
            spLogger->LogMessage(_T("GetAssemblyInfo failed for module {}", wszName));
            return S_OK;
        }

        LPCWSTR mscorlibName = L"mscorlib";
        if (_wcsicmp(assemblyName, mscorlibName) == 0)
        {
            spLogger->LogMessage(_T("this is mscorlib, noone does anything here..."));
            return S_OK;
        }

        LPCWSTR libName = L"TestClassLibrary";
        if (_wcsicmp(assemblyName, libName) != 0)
        {
            spLogger->LogMessage(_T("this is not TestClassLibrary..."));
            return S_OK;
        }

        bool instrumentationLoaded = false;
        // make sure we have the advices loaded from the config-file
        if (!instrumentationLoaded)
        {
            instrumentationLoaded = true;
        }

        // check whether the module being loaded is one that we are interested in. If not, abort
        /*bool instrumentThisModule = false;
        if (_metaUtil.ContainsAtEnd(wszName, L"Instana.Tracing.Core.dll") ||
            _metaUtil.ContainsAtEnd(wszName, L"Instana.Tracing.Core.Common.dll") ||
            _metaUtil.ContainsAtEnd(wszName, L"Instana.Tracing.Core.Transport.dll") ||
            _metaUtil.ContainsAtEnd(wszName, L"Instana.Tracing.Api.dll") ||
            _metaUtil.ContainsAtEnd(wszName, L"Instana.Tracing.Core.Instrumentation.dll") ||
            _metaUtil.ContainsAtEnd(wszName, L"Instana.ManagedTracing.dll") ||
            _metaUtil.ContainsAtEnd(wszName, L"Instana.ManagedTracing.Api.dll") ||
            _metaUtil.ContainsAtEnd(wszName, L"Instana.ManagedTracing.Base.dll") ||
            _metaUtil.ContainsAtEnd(wszName, L"Instana.ManagedTracing.Modules.dll"))
        {
            return S_OK;
        }*/
        std::string logMessage("Will instrument "); // initialized elsewhere
        std::wstring lm = std::wstring(logMessage.begin(), logMessage.end());
        lm = lm + assemblyName;
        spLogger->LogMessage(lm.c_str());

        CComPtr<IMetaDataEmit> pEmit;
        {
            CComPtr<IUnknown> pUnk;

            hr = m_pProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataEmit, &pUnk);
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("GetModuleMetaData failed for module {}", moduleID));
                return S_OK;
            }

            hr = pUnk->QueryInterface(IID_IMetaDataEmit, (LPVOID*)&pEmit);
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("QueryInterface IID_IMetaDataEmit failed for module {}", moduleID));
                return S_OK;
            }
        }

        CComPtr<IMetaDataImport> pImport;
        {
            CComPtr<IUnknown> pUnk;

            hr = m_pProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataImport, &pUnk);
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("GetModuleMetaData failed for module {}", moduleID));
                return S_OK;
            }

            hr = pUnk->QueryInterface(IID_IMetaDataImport, (LPVOID*)&pImport);
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("QueryInterface IID_IMetaDataImport failed for module {}", moduleID));
                return S_OK;
            }
        }

        moduleInfo = { 0 };
        moduleInfo.m_pImport = pImport;
        moduleInfo.m_pImport->AddRef();
        //moduleInfo.m_pMethodDefToLatestVersionMap = new MethodDefToLatestVersionMap();

        if (wcscpy_s(moduleInfo.m_wszModulePath, _countof(moduleInfo.m_wszModulePath), wszName) != 0)
        {
            spLogger->LogMessage(_T("Failed to store module path", wszName));
            return S_OK;
        }

        if (wcscpy_s(moduleInfo.m_assemblyName, _countof(moduleInfo.m_assemblyName), assemblyName) != 0)
        {
            spLogger->LogMessage(_T("Failed to store assemblyName {}", assemblyName));
            return S_OK;
        }

        // Add the references to our helper methods.
        CComPtr<IMetaDataAssemblyEmit> pAssemblyEmit;
        {
            CComPtr<IUnknown> pUnk;

            hr = m_pProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataAssemblyEmit, &pUnk);
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("GetModuleMetaData IID_IMetaDataAssemblyEmit failed for module {}", moduleID));
                return S_OK;
            }


            hr = pUnk->QueryInterface(IID_IMetaDataAssemblyEmit, (LPVOID*)&pAssemblyEmit);
            if (FAILED(hr))
            {
                //g_logger->error("IID_IMetaDataEmit: QueryInterface failed for ModuleID {}", moduleID);			LogInfo(L"QueryInterface IID_IMetaDataImport failed for module {}", moduleID);
                spLogger->LogMessage(_T("QueryInterface IID_IMetaDataAssemblyEmit failed for module {}", moduleID));
                return S_OK;
            }
        }

        CComPtr<IMetaDataAssemblyImport> pAssemblyImport;
        {
            CComPtr<IUnknown> pUnk;

            hr = m_pProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyImport, &pUnk);
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("GetModuleMetaData IID_IMetaDataAssemblyImport failed for module {}", moduleID));
                return S_OK;
            }

            hr = pUnk->QueryInterface(IID_IMetaDataAssemblyImport, (LPVOID*)&pAssemblyImport);
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("QueryInterface IID_IMetaDataAssemblyImport failed for module {}", moduleID));
                return S_OK;
            }

        }
        bool instrumentThisModule = true;

        moduleInfo.m_pAssemblyImport = pAssemblyImport;
        moduleInfo.m_pAssemblyImport->AddRef();
        bool relevant = AddMemberRefs(pAssemblyImport, pAssemblyEmit, pEmit, &moduleInfo, instrumentThisModule);
        if (relevant)
        {
            spLogger->LogMessage(_T("Add member refs is done"));
            //LogInfo(L"Applying instrumentations to {}", moduleInfo.m_assemblyName);
            //m_moduleIDToInfoMap.Update(moduleID, moduleInfo);
            //_instrumentationCoordinator.registerModule(moduleID, moduleInfo);
            //****************ApplyInstrumentations(moduleID, moduleInfo, instrumentThisModule);


            // Append to the list!

            // If we already rejitted functions in other modules with a matching path, then
            // pre-rejit those functions in this module as well.  This takes care of the case
            // where we rejitted functions in a module loaded in one domain, and we just now
            // loaded the same module (unshared) into another domain.  We must explicitly ask to
            // rejit those functions in this domain's copy of the module, since it's identified
            // by a different ModuleID.

            std::vector<ModuleID> rgModuleIDs;
            std::vector<mdToken> rgMethodDefs;


        }
    }
    catch (std::exception& e)
    {
        spLogger->LogMessage(_T("Exception on loading module: {}", e.what()));
    }

    return S_OK;
}

HRESULT GetTypeRef(LPCWSTR typeName, mdToken sourceLibraryReference, IMetaDataEmit* pEmit, mdTypeRef* ptr)
{
    HRESULT hr = pEmit->DefineTypeRefByName(sourceLibraryReference, typeName, ptr);
    return hr;
}

BOOL FindMscorlibReference(IMetaDataAssemblyImport* pAssemblyImport, mdAssemblyRef* rgAssemblyRefs, ULONG cAssemblyRefs, mdAssemblyRef* parMscorlib)
{
    HRESULT hr;

    for (ULONG i = 0; i < cAssemblyRefs; i++)
    {
        const void* pvPublicKeyOrToken;
        ULONG cbPublicKeyOrToken;
        WCHAR wszName[512];
        ULONG cchNameReturned;
        ASSEMBLYMETADATA asmMetaData;
        ZeroMemory(&asmMetaData, sizeof(asmMetaData));
        const void* pbHashValue;
        ULONG cbHashValue;
        DWORD asmRefFlags;

        hr = pAssemblyImport->GetAssemblyRefProps(
            rgAssemblyRefs[i],
            &pvPublicKeyOrToken,
            &cbPublicKeyOrToken,
            wszName,
            _countof(wszName),
            &cchNameReturned,
            &asmMetaData,
            &pbHashValue,
            &cbHashValue,
            &asmRefFlags);

        if (FAILED(hr))
        {
            return FALSE;
        }

        LPCWSTR wszContainer = wszName;
        LPCWSTR wszProspectiveEnding = L"mscorlib";
        size_t cchContainer = wcslen(wszContainer);
        size_t cchEnding = wcslen(wszProspectiveEnding);


        if (_wcsicmp(
            wszProspectiveEnding,
            &(wszContainer[cchContainer - cchEnding])) != 0)
        {
            *parMscorlib = rgAssemblyRefs[i];
            return TRUE;
        }

        wszProspectiveEnding = L"netstandard";
        cchEnding = wcslen(wszProspectiveEnding);

        if (_wcsicmp(
            wszProspectiveEnding,
            &(wszContainer[cchContainer - cchEnding])) != 0)
        {
            *parMscorlib = rgAssemblyRefs[i];
            return TRUE;
        }

        wszProspectiveEnding = L"System.Runtime";
        cchEnding = wcslen(wszProspectiveEnding);

        if (_wcsicmp(
            wszProspectiveEnding,
            &(wszContainer[cchContainer - cchEnding])) != 0)
        {
            *parMscorlib = rgAssemblyRefs[i];
            return TRUE;
        }

        
    }

    return FALSE;
}

bool CInstrumentationMethod::AddMemberRefs(IMetaDataAssemblyImport* pAssemblyImport, IMetaDataAssemblyEmit* pAssemblyEmit, IMetaDataEmit* pEmit, ModuleInfo* pModuleInfo, bool explicitlyInstrumented)
{
    CComPtr<IProfilerManagerLogging> spLogger;
    CComQIPtr<IProfilerManager4> pProfilerManager4 = m_pProfilerManager;
    pProfilerManager4->GetGlobalLoggingInstance(&spLogger);

    LPCWSTR mscorlibName = L"mscorlib";
    if (_wcsicmp(pModuleInfo->m_assemblyName, mscorlibName) == 0)
    {
        //LogInfo(L"this is mscorlib, noone does anything here...");
        return false;
    }
    bool prepareInstrumentation = explicitlyInstrumented;
    //assert(pModuleInfo != NULL);

    IMetaDataImport* pImport = pModuleInfo->m_pImport;

    HRESULT hr;

    COR_SIGNATURE sigFunctionEnterProbe[] = {
        IMAGE_CEE_CS_CALLCONV_DEFAULT,      // default calling convention
        0x04,                               // number of arguments == 2
        ELEMENT_TYPE_OBJECT,                  // return type == object
        ELEMENT_TYPE_OBJECT,
        ELEMENT_TYPE_SZARRAY, ELEMENT_TYPE_OBJECT,
        ELEMENT_TYPE_STRING,				// type name
        ELEMENT_TYPE_STRING					// method name
    };

    COR_SIGNATURE sigFunctionExitProbe[] = {
        IMAGE_CEE_CS_CALLCONV_DEFAULT,      // default calling convention
        0x05,                               // number of arguments == 3
        ELEMENT_TYPE_VOID,                  // return type == void
        ELEMENT_TYPE_OBJECT,				// instrumented object
        ELEMENT_TYPE_OBJECT,				// context obtained by entering
        ELEMENT_TYPE_SZARRAY, ELEMENT_TYPE_OBJECT, // parameters
        ELEMENT_TYPE_STRING,				// type name
        ELEMENT_TYPE_STRING					// method name
    };

    mdAssemblyRef rgMSCAssemblyRefs[20];
    ULONG cMSCAssemblyRefsReturned;
    mdAssemblyRef msCorLibRef = NULL;

    HCORENUM hmscEnum = NULL;
    do
    {
        hr = pAssemblyImport->EnumAssemblyRefs(
            &hmscEnum,
            rgMSCAssemblyRefs,
            _countof(rgMSCAssemblyRefs),
            &cMSCAssemblyRefsReturned);

        if (FAILED(hr))
        {
            spLogger->LogMessage(_T("Could not enumerate assemblies"));
            return false;
        }

        if (cMSCAssemblyRefsReturned == 0)
        {
            spLogger->LogMessage(_T("No assemblies have been returned, mscorlib not found?!"));
            return false;
        }

    } while (!FindMscorlibReference(
        pAssemblyImport,
        rgMSCAssemblyRefs,
        cMSCAssemblyRefsReturned,
        &msCorLibRef));
    pAssemblyImport->CloseEnum(hmscEnum);
    hmscEnum = NULL;

    hr = GetTypeRef(L"System.Object", msCorLibRef, pEmit, &(pModuleInfo->m_objectTypeRef));
    hr = GetTypeRef(L"System.Exception", msCorLibRef, pEmit, &(pModuleInfo->m_exceptionTypeRef));
    hr = GetTypeRef(L"System.Int16", msCorLibRef, pEmit, &(pModuleInfo->m_int16TypeRef));
    hr = GetTypeRef(L"System.Int32", msCorLibRef, pEmit, &(pModuleInfo->m_int32TypeRef));
    hr = GetTypeRef(L"System.Int64", msCorLibRef, pEmit, &(pModuleInfo->m_int64TypeRef));
    hr = GetTypeRef(L"System.Single", msCorLibRef, pEmit, &(pModuleInfo->m_float32TypeRef));
    hr = GetTypeRef(L"System.Double", msCorLibRef, pEmit, &(pModuleInfo->m_float64TypeRef));
    hr = GetTypeRef(L"System.UInt16", msCorLibRef, pEmit, &(pModuleInfo->m_uint16TypeRef));
    hr = GetTypeRef(L"System.UInt32", msCorLibRef, pEmit, &(pModuleInfo->m_uint32TypeRef));
    hr = GetTypeRef(L"System.UInt64", msCorLibRef, pEmit, &(pModuleInfo->m_uint64TypeRef));
    hr = GetTypeRef(L"System.Byte", msCorLibRef, pEmit, &(pModuleInfo->m_byteTypeRef));
    hr = GetTypeRef(L"System.SByte", msCorLibRef, pEmit, &(pModuleInfo->m_signedByteTypeRef));
    hr = GetTypeRef(L"System.Boolean", msCorLibRef, pEmit, &(pModuleInfo->m_boolTypeRef));
    hr = GetTypeRef(L"System.IntPtr", msCorLibRef, pEmit, &(pModuleInfo->m_intPtrTypeRef));

    bool g_IsDotNetCoreProcess = false;

    // find out whether the module references a module interesting for inheritance-rules
    // and store it.
    prepareInstrumentation = true;
    if (prepareInstrumentation)
    {
        WCHAR wszLocale[MAX_PATH];
        wcscpy_s(wszLocale, L"");

        // Generate assemblyRef for our api-assembly
        mdAssemblyRef apiAssemblyRef = NULL;
        // 741315d1d1544b22
        BYTE rgbApiPublicKeyToken[] = { 0x74, 0x13, 0x15, 0xd1, 0xd1, 0x54, 0x4b, 0x22 };

        ASSEMBLYMETADATA apiAssemblyMetaData;
        ZeroMemory(&apiAssemblyMetaData, sizeof(apiAssemblyMetaData));
        apiAssemblyMetaData.usMajorVersion = 1;
        apiAssemblyMetaData.usMinorVersion = 0;
        apiAssemblyMetaData.usBuildNumber = 0;
        apiAssemblyMetaData.usRevisionNumber = 0;
        apiAssemblyMetaData.szLocale = wszLocale;
        apiAssemblyMetaData.cbLocale = _countof(wszLocale);


        hr = pAssemblyEmit->DefineAssemblyRef(
            (void*)rgbApiPublicKeyToken,
            sizeof(rgbApiPublicKeyToken),
            (g_IsDotNetCoreProcess == TRUE ? ManagedTracingAssemblyNameApi : ManagedTracingApiAssemblyNameClassic),
            &apiAssemblyMetaData,
            NULL,                   // hash blob
            NULL,                   // cb of hash blob
            0,                      // flags
            &apiAssemblyRef);

        if (g_IsDotNetCoreProcess)
        {
            mdAssemblyRef commonAssemblyRef = NULL;
            BYTE rgbCommonPublicKeyToken[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

            ASSEMBLYMETADATA commonAssemblyMetaData;
            ZeroMemory(&commonAssemblyMetaData, sizeof(commonAssemblyMetaData));
            commonAssemblyMetaData.usMajorVersion = 1;
            commonAssemblyMetaData.usMinorVersion = 0;
            commonAssemblyMetaData.usBuildNumber = 0;
            commonAssemblyMetaData.usRevisionNumber = 0;
            commonAssemblyMetaData.szLocale = wszLocale;
            commonAssemblyMetaData.cbLocale = _countof(wszLocale);

            hr = pAssemblyEmit->DefineAssemblyRef(
                (void*)rgbCommonPublicKeyToken,
                sizeof(rgbCommonPublicKeyToken),
                ManagedTracingAssemblyNameCoreCommon,
                &commonAssemblyMetaData,
                NULL,                   // hash blob
                NULL,                   // cb of hash blob
                0,                      // flags
                &commonAssemblyRef);

            if (hr == S_OK)
            {
                //DEBUGGER.WriteW(L"[PROFILERCALLBACK]\tLoad comon assembly\n");
            }

            mdAssemblyRef transportAssemblyRef = NULL;
            BYTE rgbTransportPublicKeyToken[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

            ASSEMBLYMETADATA transportAssemblyMetaData;
            ZeroMemory(&transportAssemblyMetaData, sizeof(transportAssemblyMetaData));
            transportAssemblyMetaData.usMajorVersion = 1;
            transportAssemblyMetaData.usMinorVersion = 0;
            transportAssemblyMetaData.usBuildNumber = 0;
            transportAssemblyMetaData.usRevisionNumber = 0;
            transportAssemblyMetaData.szLocale = wszLocale;
            transportAssemblyMetaData.cbLocale = _countof(wszLocale);

            hr = pAssemblyEmit->DefineAssemblyRef(
                (void*)rgbTransportPublicKeyToken,
                sizeof(rgbTransportPublicKeyToken),
                ManagedTracingAssemblyNameCoreTransport,
                &transportAssemblyMetaData,
                NULL,                   // hash blob
                NULL,                   // cb of hash blob
                0,                      // flags
                &transportAssemblyRef);

            if (hr == S_OK)
            {
                //DEBUGGER.WriteW(L"[PROFILERCALLBACK]\tLoad transport assembly\n");
            }
        }

        mdAssemblyRef assemblyRef = NULL;
        mdTypeRef typeRef = mdTokenNil;
        // Generate assemblyRef for our managed-tracing assembly
        BYTE rgbPublicKeyToken[] = { 0x82, 0x6a, 0x13, 0x90, 0x12, 0x13, 0xc9, 0x89 };

        ASSEMBLYMETADATA assemblyMetaData;
        ZeroMemory(&assemblyMetaData, sizeof(assemblyMetaData));
        assemblyMetaData.usMajorVersion = 1;
        assemblyMetaData.usMinorVersion = 0;
        assemblyMetaData.usBuildNumber = 0;
        assemblyMetaData.usRevisionNumber = 0;
        assemblyMetaData.szLocale = wszLocale;
        assemblyMetaData.cbLocale = _countof(wszLocale);


        hr = pAssemblyEmit->DefineAssemblyRef(
            (void*)rgbPublicKeyToken,
            sizeof(rgbPublicKeyToken),
            (g_IsDotNetCoreProcess == TRUE ? ManagedTracingAssemblyNameCore : ManagedTracingAssemblyNameClassic),
            &assemblyMetaData,
            NULL,                   // hash blob
            NULL,                   // cb of hash blob
            0,                      // flags
            &assemblyRef);

        // Generate typeRef to our managed tracer

        hr = pEmit->DefineTypeRefByName(
            assemblyRef,
            (g_IsDotNetCoreProcess == TRUE ? ManagedTracingTracerClassNameCore : ManagedTracingTracerClassNameClassic),
            &typeRef);

        hr = pEmit->DefineMemberRef(
            typeRef,
            k_wszEnteredFunctionProbeName,
            sigFunctionEnterProbe,
            sizeof(sigFunctionEnterProbe),
            &(pModuleInfo->m_mdEnterProbeRef));

        hr = pEmit->DefineMemberRef(
            typeRef,
            k_wszExitedFunctionProbeName,
            sigFunctionExitProbe,
            sizeof(sigFunctionExitProbe),
            &(pModuleInfo->m_mdExitProbeRef));

    }
    return prepareInstrumentation;
}


//HRESULT OnModuleLoaded2(_In_ IModuleInfo* pModuleInfo)
//{
//    HRESULT hr = S_OK;
//
//    InstrStr(clrieStrModuleName);
//    IfFailRet(pModuleInfo->GetModuleName(&clrieStrModuleName.m_bstr));
//
//        CComPtr<IProfilerManagerLogging> spLogger;
//        CComQIPtr<IProfilerManager4> pProfilerManager4 = m_pProfilerManager;
//        pProfilerManager4->GetGlobalLoggingInstance(&spLogger);
//        spLogger->LogMessage(_T("InjectAssembly"));
//        spLogger->LogMessage(m_spInjectAssembly->m_targetAssemblyName.c_str());
//        spLogger->LogMessage(m_spInjectAssembly->m_sourceAssemblyName.c_str());
//    if ((m_spInjectAssembly != nullptr) && (wcscmp(clrieStrModuleName, m_spInjectAssembly->m_targetAssemblyName.c_str()) == 0))
//    {
//
//        CComPtr<IMetaDataDispenserEx> pDisp;
//        ComInitializer coInit(COINIT_MULTITHREADED);
//        IfFailRet(CoCreateInstance(CLSID_CorMetaDataDispenser, NULL, CLSCTX_INPROC_SERVER,
//            IID_IMetaDataDispenserEx, (void **)&pDisp));
//
//        CComPtr<IMetaDataImport2> pSourceImport;
//        std::wstring path(m_spInjectAssembly->m_sourceAssemblyName);
//        IfFailRet(pDisp->OpenScope(path.c_str(), 0, IID_IMetaDataImport2, (IUnknown **)&pSourceImport));
//
//        // Loads the image with section alignment, but doesn't do any of the other steps to ready an image to run
//        // This is sufficient to read IL code from the image without doing RVA -> file offset mapping
//        std::shared_ptr<HINSTANCE__> spSourceImage(LoadLibraryEx(path.c_str(), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE), FreeLibrary);
//        if (spSourceImage == nullptr)
//        {
//            return HRESULT_FROM_WIN32(::GetLastError());
//        }
//        //Not sure if there is a better way to get the base address of a module loaded via LoadLibrary?
//        //We are cheating a bit knowing that module handles are actually base addresses with a few bits OR'ed in
//        LPCBYTE* pSourceImage = reinterpret_cast<LPCBYTE*>((ULONGLONG)spSourceImage.get() & (~2));
//
//        IfFailRet(pModuleInfo->ImportModule(pSourceImport, pSourceImage));
//    }
//
//    spLogger->LogMessage(_T("Assembly imported"));
//
//    CComPtr<IMetaDataImport> pIMetaDataImport;
//    IfFailRet(pModuleInfo->GetMetaDataImport(reinterpret_cast<IUnknown**>(&pIMetaDataImport)));
//
//    CComPtr<IMetaDataAssemblyImport> pIMetaDataAssemblyImport;
//    IfFailRet(pModuleInfo->GetMetaDataImport(reinterpret_cast<IUnknown**>(&pIMetaDataAssemblyImport)));
//
//    mdAssembly resolutionScope;
//    pIMetaDataAssemblyImport->GetAssemblyFromScope(&resolutionScope);
//
//    ULONG chName = MAX_PATH;
//    std::wstring strName(chName, wchar_t());
//    DWORD cTokens;
//    mdTypeRef currentTypeRef;
//    mdTypeRef targetTypeRef = mdTypeRefNil;
//    MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataImport> pHEnSourceTypeRefs(pIMetaDataImport, nullptr);
//    while (SUCCEEDED(pIMetaDataImport->EnumTypeRefs(pHEnSourceTypeRefs.Get(), &currentTypeRef, 1, &cTokens)) && cTokens > 0)
//    {
//        pIMetaDataImport->GetTypeRefProps(currentTypeRef, &resolutionScope, &strName[0], static_cast<ULONG>(strName.size()), &chName);
//
//        spLogger->LogMessage(_T("Test Type: "));
//        spLogger->LogMessage(strName.c_str());
//    }
//    // get the moduleId and method token for instrument method entry
//    for (shared_ptr<CInstrumentMethodEntry> pInstrumentMethodEntry : m_instrumentMethodEntries)
//    {
//        const tstring& instrModuleName = pInstrumentMethodEntry->GetModuleName();
//
//        if (wcscmp(clrieStrModuleName, instrModuleName.c_str()) == 0)
//        {
//            // search for method name inside all the types of the matching module
//            const tstring& methodName = pInstrumentMethodEntry->GetMethodName();
//
//            CComPtr<IMetaDataImport2> pMetadataImport;
//            IfFailRet(pModuleInfo->GetMetaDataImport((IUnknown**)&pMetadataImport));
//
//            BOOL bIsRejit = pInstrumentMethodEntry->GetIsRejit();
//
//            // Only request ReJIT if the method will modify code on ReJIT. On .NET Core, the runtime will
//            // only callback into the profiler for the ReJIT of the method if ReJIT is requested whereas the
//            // appropriate callbacks are made for both JIT and ReJIT on .NET Framework.
//            if (bIsRejit)
//            {
//                MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataImport2> spTypeDefEnum(pMetadataImport, nullptr);
//                mdTypeDef typeDef = mdTypeDefNil;
//                ULONG cTokens = 0;
//                while (S_OK == (hr = pMetadataImport->EnumTypeDefs(spTypeDefEnum.Get(), &typeDef, 1, &cTokens)))
//                {
//                    MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataImport2> spMethodEnum(pMetadataImport, nullptr);
//                    mdToken methodDefs[16];
//                    ULONG cMethod = 0;
//                    pMetadataImport->EnumMethodsWithName(spMethodEnum.Get(), typeDef, methodName.c_str(), methodDefs, _countof(methodDefs), &cMethod);
//
//                    if (cMethod > 0)
//                    {
//                        pModuleInfo->RequestRejit(methodDefs[0]);
//                        break;
//                    }
//                }
//            }
//        }
//    }
//
//    return S_OK;
//}

HRESULT CInstrumentationMethod::OnModuleUnloaded(_In_ IModuleInfo* pModuleInfo)
{
    return S_OK;
}

HRESULT CInstrumentationMethod::OnShutdown()
{
    if (m_bExceptionTrackingEnabled)
    {
        CComPtr<IProfilerManagerLogging> spLogger;
        m_pProfilerManager->GetLoggingInstance(&spLogger);
        spLogger->LogDumpMessage(_T("</ExceptionTrace>"));
    }
    else if (m_bTestInstrumentationMethodLogging)
    {
        CComPtr<IProfilerManagerLogging> spLogger;
        CComQIPtr<IProfilerManager4> pProfilerManager4 = m_pProfilerManager;
        pProfilerManager4->GetGlobalLoggingInstance(&spLogger);
        spLogger->LogDumpMessage(_T("</InstrumentationMethodLog>"));
    }

    return S_OK;
}

HRESULT CInstrumentationMethod::ShouldInstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit, _Out_ BOOL* pbInstrument)
{
    *pbInstrument = FALSE;
    HRESULT hr;
    CComPtr<IModuleInfo> pModuleInfo;
    pMethodInfo->GetModuleInfo(&pModuleInfo);

    InstrStr(clrieStrModule);
    IfFailRet(pModuleInfo->GetModuleName(&clrieStrModule.m_bstr));

    CComPtr<IProfilerManagerLogging> spLogger;
    CComQIPtr<IProfilerManager4> pProfilerManager4 = m_pProfilerManager;
    pProfilerManager4->GetGlobalLoggingInstance(&spLogger);
    spLogger->LogMessage(_T("ModuleName"));
    spLogger->LogMessage(clrieStrModule);

    InstrStr(clrieStrMethodName);
    pMethodInfo->GetName(&clrieStrMethodName.m_bstr);
    spLogger->LogMessage(_T("MethodName"));
    spLogger->LogMessage(clrieStrMethodName.m_bstr);

    tstring moduleName = clrieStrModule;

    for (shared_ptr<CInstrumentMethodEntry> pInstrumentMethodEntry : m_instrumentMethodEntries)
    {
        const tstring& enteryModuleName = pInstrumentMethodEntry->GetModuleName();

        spLogger->LogMessage(_T("enteryModuleName"));
        spLogger->LogMessage(enteryModuleName.c_str());

        if (wcscmp(enteryModuleName.c_str(), clrieStrModule) == 0)
        {
            // TODO: Eventually, this will need to use the full name and not the partial name
            //InstrStr(clrieStrMethodName);
            pMethodInfo->GetName(&clrieStrMethodName.m_bstr);

            const tstring& entryMethodName= pInstrumentMethodEntry->GetMethodName();

            spLogger->LogMessage(_T("entryMethodName"));
            spLogger->LogMessage(entryMethodName.c_str());

            if (wcscmp(entryMethodName.c_str(), clrieStrMethodName) == 0)
            {
                m_methodInfoToEntryMap[pMethodInfo] = pInstrumentMethodEntry;

                BOOL bRequireRejit = pInstrumentMethodEntry->GetIsRejit();
                *pbInstrument = true;

                //*pbInstrument = (bRequireRejit == isRejit);
                return S_OK;
            }
        }
    }

    return S_OK;
}

HRESULT CInstrumentationMethod::BeforeInstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit)
{
    HRESULT hr = S_OK;

    return hr;

    shared_ptr<CInstrumentMethodEntry> pMethodEntry = m_methodInfoToEntryMap[pMethodInfo];

    if (!pMethodEntry)
    {
        ATLASSERT(!L"CInstrumentationMethod::InstrumentMethod - why no method entry? It should have been set in ShouldInstrumentMethod");
        return E_FAIL;
    }

    BOOL bRequireRejit = pMethodEntry->GetIsRejit();
    if (bRequireRejit != isRejit)
    {
        return E_FAIL;
    }

    vector<shared_ptr<CInstrumentInstruction>> instructions = pMethodEntry->GetInstructions();

    CComPtr<IInstructionGraph> pInstructionGraph;
    IfFailRet(pMethodInfo->GetInstructions(&pInstructionGraph));

    if (pMethodEntry->IsReplacement())
    {
        vector<BYTE> ilCode;
        for (shared_ptr<CInstrumentInstruction> instruction : instructions)
        {
            IfFailRet(instruction->EmitIL(ilCode));
        }
        vector<COR_IL_MAP> corIlMap = pMethodEntry->GetCorILMap();

        // TODO: (wiktor) add sequence points to test
        vector<DWORD> baselineSequencePoints;
        baselineSequencePoints.resize(corIlMap.size());
        for (size_t i = 0; i < baselineSequencePoints.size(); i++)
        {
            baselineSequencePoints[i] = corIlMap[i].newOffset;
        }

        IfFailRet(pInstructionGraph->CreateBaseline(ilCode.data(), ilCode.data() + ilCode.size(), (DWORD)corIlMap.size(), corIlMap.data(), (DWORD)baselineSequencePoints.size(), baselineSequencePoints.data()));
    }

    return hr;
}

HRESULT CInstrumentationMethod::InstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit)
{
    HRESULT hr = S_OK;

    shared_ptr<CInstrumentMethodEntry> pMethodEntry = m_methodInfoToEntryMap[pMethodInfo];

    if (!pMethodEntry)
    {
        ATLASSERT(!L"CInstrumentationMethod::InstrumentMethod - why no method entry? It should have been set in ShouldInstrumentMethod");
        return E_FAIL;
    }

    CComPtr<IProfilerManagerLogging> spLogger;
    CComQIPtr<IProfilerManager4> pProfilerManager4 = m_pProfilerManager;
    pProfilerManager4->GetGlobalLoggingInstance(&spLogger);
    spLogger->LogMessage(_T("InstrumentMethod start"));


    vector<shared_ptr<CInstrumentInstruction>> instructions = pMethodEntry->GetInstructions();

    CComPtr<IInstructionGraph> pInstructionGraph;
    IfFailRet(pMethodInfo->GetInstructions(&pInstructionGraph));
    BSTR methodName = L"";
    pMethodInfo->GetFullName(&methodName);

    spLogger->LogMessage(_T("InstrumentMethod"));
    spLogger->LogMessage(methodName);

    if (pMethodEntry->IsSingleRetFirst())
    {
        PerformSingleReturnInstrumentation(pMethodInfo, pInstructionGraph);
    }


    CComPtr<IInstruction> sptrCurrent;
    if (pMethodEntry->GetPointTo() != nullptr)
    {
        spLogger->LogMessage(_T("point to"));
        std::shared_ptr<CInstrumentMethodPointTo> spPointTo = pMethodEntry->GetPointTo();
        BYTE pSignature[256] = {};
        DWORD cbSignature = 0;
        pMethodInfo->GetCorSignature(_countof(pSignature), pSignature, &cbSignature);


       /* mdToken tkMethodToken = mdTokenNil;

        CComPtr<IModuleInfo> spModuleInfo;
        IfFailRet(pMethodInfo->GetModuleInfo(&spModuleInfo));

        mdToken tkMscorlibRef = mdTokenNil;
        CComPtr<IMetaDataAssemblyImport> spIMetaDataAssemblyImport;
        IfFailRet(spModuleInfo->GetMetaDataAssemblyImport(
            reinterpret_cast<IUnknown**>(&spIMetaDataAssemblyImport)));

        const auto MaxReferenceCount = 256;
        std::vector<mdAssemblyRef> vecAssemblyRefs(MaxReferenceCount, mdAssemblyRefNil);
        ULONG ulReceivedCount = 0;
        MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataAssemblyImport> spEnum(spIMetaDataAssemblyImport, nullptr);
        IfFailRet(spIMetaDataAssemblyImport->EnumAssemblyRefs(
            spEnum.Get(),
            &vecAssemblyRefs[0],
            static_cast<ULONG>(vecAssemblyRefs.size()),
            &ulReceivedCount));

        vecAssemblyRefs.resize(ulReceivedCount);

        for (mdAssemblyRef tkAssemblyRef : vecAssemblyRefs)
        {
            LPCVOID        pbPublicKey = nullptr;
            ULONG        cbPublicKey = 0;
            LPCVOID        pbHashValue = nullptr;
            ULONG        cbHashValue = 0;
            ULONG        chName = 0;

            std::wstring strName(MAX_PATH, WCHAR());
            DWORD dwAsemblyRefFlags = 0;
            ASSEMBLYMETADATA asmMetadata = { 0 };

            if (SUCCEEDED(spIMetaDataAssemblyImport->GetAssemblyRefProps(
                tkAssemblyRef,
                &pbPublicKey,
                &cbPublicKey,
                &strName[0],
                static_cast<ULONG>(strName.size()),
                &chName,
                &asmMetadata,
                &pbHashValue,
                &cbHashValue,
                &dwAsemblyRefFlags)))
            {
                strName.resize(0 == chName ? chName : chName - 1);

                if (0 == strName.compare(spPointTo->m_assemblyName))
                {
                    tkMscorlibRef = tkAssemblyRef;

                    break;
                }
            }
        }

        CComPtr<IMetaDataImport> spIMetaDataImport;
        IfFailRet(spModuleInfo->GetMetaDataImport(
            reinterpret_cast<IUnknown**>(&spIMetaDataImport)));*/

        //mdToken tkTypeToken;

        //HRESULT hrFound = spIMetaDataImport->FindTypeRef(tkMscorlibRef, spPointTo->m_typeName.c_str(), &tkTypeToken);

        /*if (hrFound == CLDB_E_RECORD_NOTFOUND)
        {
            CComPtr<IMetaDataEmit> spIMetaDataEmit;
            IfFailRet(spModuleInfo->GetMetaDataEmit(
                reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));

            IfFailRet(spIMetaDataEmit->DefineTypeRefByName(tkMscorlibRef, spPointTo->m_typeName.c_str(), &tkTypeToken));
        }
        else if (FAILED(hrFound))
        {
            IfFailRet(hrFound);
        }*/

        //hrFound = spIMetaDataImport->FindMemberRef(tkTypeToken, spPointTo->m_methodName.c_str(), pSignature, cbSignature, &tkMethodToken);

        /*if (hrFound == CLDB_E_RECORD_NOTFOUND)
        {
            CComPtr<IMetaDataEmit> spIMetaDataEmit;
            IfFailRet(spModuleInfo->GetMetaDataEmit(
                reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));

            IfFailRet(spIMetaDataEmit->DefineMemberRef(tkTypeToken, spPointTo->m_methodName.c_str(), pSignature, cbSignature, &tkMethodToken));
        }
        else if (FAILED(hrFound))
        {
            IfFailRet(hrFound);
        }*/


        // Get arguments count
        if (cbSignature < 2)
            return S_FALSE;
        USHORT argsCount = pSignature[1];

        CComPtr<IInstructionFactory> sptrInstructionFactory;
        IfFailRet(pMethodInfo->GetInstructionFactory(&sptrInstructionFactory));

        sptrInstructionFactory->local

        CComPtr<IInstructionGraph> sptrInstructionGraph;
        IfFailRet(pMethodInfo->GetInstructions(&sptrInstructionGraph));
        sptrInstructionGraph->RemoveAll();

        
        IfFailRet(sptrInstructionGraph->GetFirstInstruction(&sptrCurrent));

        for (USHORT i = 0; i < argsCount; i++)
        {
            CComPtr<IInstruction> sptrLoadArg;

            IfFailRet(sptrInstructionFactory->CreateLoadArgInstruction(i, &sptrLoadArg));
            IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrLoadArg));

            sptrCurrent = sptrLoadArg;
        }

        CComPtr<IInstruction> sptrCallMethod;

        IfFailRet(sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Call, moduleInfo.m_mdEnterProbeRef, &sptrCallMethod));
        IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrCallMethod));
        sptrCurrent = sptrCallMethod;

        CComPtr<IInstruction> sptrReturn;

        IfFailRet(sptrInstructionFactory->CreateInstruction(Cee_Ret, &sptrReturn));
        IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrReturn));
        sptrCurrent = sptrReturn;

       /* for (USHORT i = 0; i < argsCount; i++)
        {
            CComPtr<IInstruction> sptrLoadArg2;

            IfFailRet(sptrInstructionFactory->CreateLoadArgInstruction(i, &sptrLoadArg2));
            IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrLoadArg2));

            sptrCurrent = sptrLoadArg2;
        }*/

        /*CComPtr<IInstruction> sptrCallMethod2;
        IfFailRet(sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Call, moduleInfo.m_mdEnterProbeRef, &sptrCallMethod2));
        IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrCallMethod));
        sptrCurrent = sptrCallMethod2;


        CComPtr<IInstruction> sptrReturn2;

        IfFailRet(sptrInstructionFactory->CreateInstruction(Cee_Ret, &sptrReturn2));
        IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrReturn2));
        sptrCurrent = sptrReturn2;*/

    }
    else if (!pMethodEntry->IsReplacement())
    {
    spLogger->LogMessage(_T("replacement"));
        CComPtr<IInstructionFactory> pInstructionFactory;
        IfFailRet(pMethodInfo->GetInstructionFactory(&pInstructionFactory));

        for (shared_ptr<CInstrumentInstruction> pInstruction : instructions)
        {
            ILOrdinalOpcode opcode = pInstruction->GetOpcode();
            ILOpcodeInfo opcodeInfo = pInstruction->GetOpcodeInfo();
            InstrumentationType instrType = pInstruction->GetInstrumentationType();

            if ((instrType == Remove) || (instrType == RemoveAll))
            {
                if (instrType == Remove)
                {
                    CComPtr<IInstruction> pOldInstruction;
                    IfFailRet(pInstructionGraph->GetInstructionAtOriginalOffset(pInstruction->GetOffset(), &pOldInstruction));
                    IfFailRet(pInstructionGraph->Remove(pOldInstruction));
                }
                else if (instrType == RemoveAll)
                {
                    IfFailRet(pInstructionGraph->RemoveAll());

                    // If the last instrumentation is remove all, we need to add a ret operation to avoid empty method
                    if (instructions.back() == pInstruction)
                    {
                        CComPtr<IInstruction> pNewInstruction;
                        IfFailRet(pInstructionFactory->CreateInstruction(Cee_Ret, &pNewInstruction));
                        IfFailRet(pInstructionGraph->InsertAfter(NULL, pNewInstruction));
                    }
                }
            }
            else
            {
                for (DWORD i = 0; i < pInstruction->GetRepeatCount(); ++i)
                {
                    CComPtr<IInstruction> pNewInstruction;

                    if (opcodeInfo.m_type == ILOperandType_None)
                    {
                        IfFailRet(pInstructionFactory->CreateInstruction(opcode, &pNewInstruction));
                    }
                    else if (opcodeInfo.m_type == ILOperandType_Switch)
                    {
                        ATLASSERT(!L"Switch not implemented yet in profiler host");
                        return E_FAIL;
                    }
                    else
                    {
                        if ((opcodeInfo.m_flags & ILOpcodeFlag_Branch) != 0)
                        {
                            // It's a branch!
                            DWORD branchTarget = (DWORD)pInstruction->GetOperand();

                            if (opcodeInfo.m_type == ILOperandType_Byte || opcodeInfo.m_type == ILOperandType_Int)
                            {
                                CComPtr<IInstruction> pBranchTarget;
                                IfFailRet(pInstructionGraph->GetInstructionAtOriginalOffset(branchTarget, &pBranchTarget));
                                IfFailRet(pInstructionFactory->CreateBranchInstruction(opcode, pBranchTarget, &pNewInstruction));
                            }
                            else
                            {
                                // Unexpected branch operand length
                                ATLASSERT(!L"Switch not implemented yet");
                                return E_FAIL;
                            }
                        }
                        else
                        {
                            INT64 operand = pInstruction->GetOperand();
                            if (opcodeInfo.m_type == ILOperandType_Byte)
                            {
                                IfFailRet(pInstructionFactory->CreateByteOperandInstruction(opcode, (BYTE)operand, &pNewInstruction));
                            }
                            else if (opcodeInfo.m_type == ILOperandType_Int)
                            {
                                IfFailRet(pInstructionFactory->CreateIntOperandInstruction(opcode, (INT32)operand, &pNewInstruction));
                            }
                            else if (opcodeInfo.m_type == ILOperandType_UShort)
                            {
                                IfFailRet(pInstructionFactory->CreateUShortOperandInstruction(opcode, (USHORT)operand, &pNewInstruction));
                            }
                            else if (opcodeInfo.m_type == ILOperandType_Long)
                            {
                                IfFailRet(pInstructionFactory->CreateLongOperandInstruction(opcode, (INT64)operand, &pNewInstruction));
                            }
                            else if (opcodeInfo.m_type == ILOperandType_Token)
                            {
                                IfFailRet(pInstructionFactory->CreateTokenOperandInstruction(opcode, (mdToken)operand, &pNewInstruction));
                            }
                            else if (opcodeInfo.m_type == ILOperandType_Single)
                            {
                                ATLASSERT(!L"Single not implemented yet");
                                return E_FAIL;
                            }
                            else if (opcodeInfo.m_type == ILOperandType_Double)
                            {
                                ATLASSERT(!L"Double not implemented yet");
                                return E_FAIL;
                            }
                            else
                            {
                                ATLASSERT(!L"Unexpected operand length");
                                return E_FAIL;
                            }
                        }
                    }

                    CComPtr<IInstruction> pOldInstruction;
                    IfFailRet(pInstructionGraph->GetInstructionAtOriginalOffset(pInstruction->GetOffset(), &pOldInstruction));

                    if (instrType == InsertBefore)
                    {
                        IfFailRet(pInstructionGraph->InsertBeforeAndRetargetOffsets(pOldInstruction, pNewInstruction));
                    }
                    else if (instrType == InsertAfter)
                    {
                        IfFailRet(pInstructionGraph->InsertAfter(pOldInstruction, pNewInstruction));
                    }
                    else if (instrType == Replace)
                    {
                        IfFailRet(pInstructionGraph->Replace(pOldInstruction, pNewInstruction));
                    }
                }
            }
        }
    }

    spLogger->LogMessage(_T("Start %04x", moduleInfo.m_mdEnterProbeRef));

    /*vector<CLocalType> locals;
    locals.push_back(CLocalType(_T("System.Object")));
    locals.push_back(CLocalType(_T("System.Object")));
    locals.push_back(CLocalType(_T("System.String")));
    locals.push_back(CLocalType(_T("System.String")));
    pMethodEntry->AddLocals(locals);

    IfFailRet(InstrumentLocals(pMethodInfo, pMethodEntry));*/

    //CComPtr<IProfilerManagerLogging> spLogger;
    //CComQIPtr<IProfilerManager4> pProfilerManager4 = m_pProfilerManager;
    //pProfilerManager4->GetGlobalLoggingInstance(&spLogger);



    //CComPtr<IInstructionFactory> sptrInstructionFactory;
    //IfFailRet(pMethodInfo->GetInstructionFactory(&sptrInstructionFactory));

    //CComPtr<IInstruction> firstInstruction;
    //IfFailRet(pInstructionGraph->GetFirstInstruction(&firstInstruction));

    //////LdNull
    //CComPtr<IInstruction> ldNull;
    //sptrInstructionFactory->CreateInstruction(Cee_Ldnull, &ldNull);
    //pInstructionGraph->InsertAfter(sptrCurrent, ldNull);

    ////StLoc
    //CComPtr<IInstruction> stLoc;
    //sptrInstructionFactory->CreateStoreLocalInstruction(2, &stLoc);
    //pInstructionGraph->InsertBefore(firstInstruction, stLoc);

    ////newArray
    //CComPtr<IInstruction> newArrayInst;
    //sptrInstructionFactory->CreateInstruction(Cee_Newarr, &newArrayInst);
    //pInstructionGraph->InsertBefore(firstInstruction, newArrayInst);

    //call Tracer.Enter
    /*CComPtr<IInstruction> callTracerEnterInst;
    sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Call, moduleInfo.m_mdEnterProbeRef, &callTracerEnterInst);
    pInstructionGraph->InsertBefore(sptrCurrent, callTracerEnterInst);*/

    spLogger->LogMessage(_T("Finish"));
    
    if (pMethodEntry->IsSingleRetLast())
    {
        IfFailRet(PerformSingleReturnInstrumentation(pMethodInfo, pInstructionGraph));
    }

    if (pMethodEntry->IsAddExceptionHandler())
    {
        AddExceptionHandler(pMethodInfo, pInstructionGraph);
    }

    return S_OK;
}

HRESULT CInstrumentationMethod::PerformSingleReturnInstrumentation(IMethodInfo* pMethodInfo, IInstructionGraph* pInstructionGraph)
{
    HRESULT hr;
    CComPtr<ISingleRetDefaultInstrumentation> pSingleRet;
    IfFailRet(pMethodInfo->GetSingleRetDefaultInstrumentation(&pSingleRet));
    IfFailRet(pSingleRet->Initialize(pInstructionGraph));
    IfFailRet(pSingleRet->ApplySingleRetDefaultInstrumentation());
    return S_OK;
}

// this function doesn't handle multiple returns or tailcalls
// we expect PerformSingleReturnInstrumentation to be applied before this function is called, so normally that won't be an issue
HRESULT CInstrumentationMethod::AddExceptionHandler(IMethodInfo* pMethodInfo, IInstructionGraph* pInstructionGraph)
{
    HRESULT hr;

    CComPtr<IInstruction> pCurrentInstruction;
    IfFailRet(pInstructionGraph->GetFirstInstruction(&pCurrentInstruction));
    DWORD returnCount = 0;
    DWORD tailcallCount = 0;

    CComPtr<IInstruction> pInstruction;
    IfFailRet(pInstructionGraph->GetFirstInstruction(&pInstruction));

    while (pInstruction != NULL)
    {
        ILOrdinalOpcode opCode;
        IfFailRet(pInstruction->GetOpCode(&opCode));

        if (opCode == Cee_Ret)
        {
            returnCount++;
        }
        if (opCode == Cee_Tailcall)
        {
            tailcallCount++;
        }

        CComPtr<IInstruction> pCurr = pInstruction;
        pInstruction.Release();
        pCurr->GetNextInstruction(&pInstruction);
    }

    if (tailcallCount > 0)
    {
        ATLASSERT(L"Trying to add exception handler to method with tailcall");
        return E_FAIL;
    }

    if (returnCount > 1) // note it would be valid to have method with zero returns, as we could end in a throw
    {
        ATLASSERT(L"Trying to add exception handler to method with more than one return");
        return E_FAIL;
    }

    IModuleInfo* pModuleInfo;
    IfFailRet(pMethodInfo->GetModuleInfo(&pModuleInfo));

    IType* pReturnType;
    IfFailRet(pMethodInfo->GetReturnType(&pReturnType));

    CorElementType returnCorElementType;
    IfFailRet(pReturnType->GetCorElementType(&returnCorElementType));

    CComPtr<ILocalVariableCollection> localVars;
    IfFailRet(pMethodInfo->GetLocalVariables(&localVars));

    DWORD returnIndex = -1;
    if (returnCorElementType != ELEMENT_TYPE_VOID)
    {
        IfFailRet(localVars->AddLocal(pReturnType, &returnIndex));
    }

    CComPtr<IMetaDataImport> pIMetaDataImport;
    IfFailRet(pModuleInfo->GetMetaDataImport(reinterpret_cast<IUnknown**>(&pIMetaDataImport)));

    CComPtr<IMetaDataAssemblyImport> pIMetaDataAssemblyImport;
    IfFailRet(pModuleInfo->GetMetaDataImport(reinterpret_cast<IUnknown**>(&pIMetaDataAssemblyImport)));

    mdAssembly resolutionScope;
    pIMetaDataAssemblyImport->GetAssemblyFromScope(&resolutionScope);

    std::wstring strExceptionTypeName(_T("System.Exception"));

    ULONG chName = MAX_PATH;
    std::wstring strName(chName, wchar_t());
    DWORD cTokens;
    mdTypeRef currentTypeRef;
    mdTypeRef targetTypeRef = mdTypeRefNil;
    MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataImport> pHEnSourceTypeRefs(pIMetaDataImport, nullptr);
    while (SUCCEEDED(pIMetaDataImport->EnumTypeRefs(pHEnSourceTypeRefs.Get(), &currentTypeRef, 1, &cTokens)) && cTokens > 0)
    {
        pIMetaDataImport->GetTypeRefProps(currentTypeRef, &resolutionScope, &strName[0], static_cast<ULONG>(strName.size()), &chName);
        if (0 == wcscmp(strName.c_str(), strExceptionTypeName.c_str()))
        {
            targetTypeRef = currentTypeRef;
            break;
        }
    }

    if (targetTypeRef == mdTypeRefNil)
    {
        return E_FAIL;
    }


    CComPtr<IInstructionFactory> pInstructionFactory;
    IfFailRet(pMethodInfo->GetInstructionFactory(&pInstructionFactory));

    CComPtr<IInstruction> pExitLabel;
    IfFailRet(pInstructionFactory->CreateInstruction(Cee_Nop, &pExitLabel));

    CComPtr<IInstruction> pRet;
    IfFailRet(pInstructionFactory->CreateInstruction(Cee_Ret, &pRet));

    CComPtr<IInstruction> pFirst;
    IfFailRet(pInstructionGraph->GetFirstInstruction(&pFirst));

    CComPtr<IInstruction> pLast;
    IfFailRet(pInstructionGraph->GetLastInstruction(&pLast));

    ILOrdinalOpcode lastOpCode;
    IfFailRet(pLast->GetOpCode(&lastOpCode));
    if (lastOpCode == Cee_Ret)
    {
        if (returnCorElementType == ELEMENT_TYPE_VOID)
        {
            // it might seem that return instruction could simply be removed, but it might be used as jump target, so better to replace with a nop
            CComPtr<IInstruction> pNewLast;
            pInstructionFactory->CreateInstruction(Cee_Nop, &pNewLast);
            pInstructionGraph->Replace(pLast, pNewLast);
            pLast = pNewLast;
        }
        else
        {
            CComPtr<IInstruction> pReplacementLast;
            pInstructionFactory->CreateStoreLocalInstruction(static_cast<USHORT>(returnIndex), &pReplacementLast);
            pInstructionGraph->Replace(pLast, pReplacementLast);
            pLast = pReplacementLast;
        }
    }

    CComPtr<IExceptionSection> pExceptionSection;
    IfFailRet(pMethodInfo->GetExceptionSection(&pExceptionSection));

    CComPtr<IInstruction> pLeave1;
    IfFailRet(pInstructionFactory->CreateBranchInstruction(Cee_Leave_S, pExitLabel, &pLeave1));
    IfFailRet(pInstructionGraph->InsertAfter(pLast, pLeave1));

    // --- handler body ---
    CComPtr<IInstruction> pPop;
    IfFailRet(pInstructionFactory->CreateInstruction(Cee_Pop, &pPop));
    IfFailRet(pInstructionGraph->InsertAfter(pLeave1, pPop));
    // --- handler body ---

    CComPtr<IInstruction> pLeave2;
    IfFailRet(pInstructionFactory->CreateBranchInstruction(Cee_Leave_S, pExitLabel, &pLeave2));
    IfFailRet(pInstructionGraph->InsertAfter(pPop, pLeave2));

    IfFailRet(pInstructionGraph->InsertAfter(pLeave2, pExitLabel));
    CComPtr<IInstruction> pPrev = pExitLabel;

    if (returnCorElementType != ELEMENT_TYPE_VOID)
    {
        CComPtr<IInstruction> pLoadResult;
        pInstructionFactory->CreateLoadLocalInstruction(static_cast<USHORT>(returnIndex), &pLoadResult);
        IfFailRet(pInstructionGraph->InsertAfter(pPrev, pLoadResult));
        pPrev = pLoadResult;
    }

    IfFailRet(pInstructionGraph->InsertAfter(pPrev, pRet));

    CComPtr<IExceptionClause> pExceptionClause;
    pExceptionSection->AddNewExceptionClause(0, pFirst, pLeave1, pPop, pLeave2, nullptr, targetTypeRef, &pExceptionClause);

    return S_OK;
}

HRESULT CInstrumentationMethod::InstrumentLocals(IMethodInfo* pMethodInfo, shared_ptr<CInstrumentMethodEntry> pMethodEntry)
{
    HRESULT hr = S_OK;

    const vector<CLocalType>& locals = pMethodEntry->GetLocals();

    if (locals.size() > 0)
    {
        CComPtr<IModuleInfo> pModuleInfo;
        IfFailRet(pMethodInfo->GetModuleInfo(&pModuleInfo));

        CComPtr<ILocalVariableCollection> pLocals;
        IfFailRet(pMethodInfo->GetLocalVariables(&pLocals));
        for (const CLocalType& local : locals)
        {
            CComPtr<IType> pType;
            IfFailRet(GetType(pModuleInfo, local, &pType));
            IfFailRet(pLocals->AddLocal(pType, nullptr));
        }
    }

    return hr;
}

HRESULT CInstrumentationMethod::GetType(IModuleInfo* pModuleInfo, const CLocalType& localType, IType** pType)
{
    HRESULT hr = S_OK;
    const wstring& typeName = localType.GetTypeName();

    CComPtr<ITypeCreator> pTypeFactory;
    IfFailRet(pModuleInfo->CreateTypeFactory(&pTypeFactory));

    auto element = m_typeMap.find(typeName);
    if (element == m_typeMap.end())
    {
        CComPtr<IMetaDataImport> pMetadata;
        IfFailRet(pModuleInfo->GetMetaDataImport((IUnknown**)&pMetadata));
        mdTypeDef typeDef = mdTokenNil;
        IfFailRet(pMetadata->FindTypeDefByName(typeName.c_str(), 0, &typeDef));

        //TODO For now, we only support finding classes in the same module.
        //In the future, add support for refs and other types
        IfFailRet(pTypeFactory->FromToken(ELEMENT_TYPE_CLASS, typeDef, pType));
    }
    else
    {
        IfFailRet(pTypeFactory->FromCorElement(element->second, pType));
    }

    return hr;
}

HRESULT CInstrumentationMethod::AllowInlineSite(_In_ IMethodInfo* pMethodInfoInlinee, _In_ IMethodInfo* pMethodInfoCaller, _Out_ BOOL* pbAllowInline)
{
    *pbAllowInline = FALSE;
    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionCatcherEnter(
    _In_ IMethodInfo* pMethodInfo,
    _In_ UINT_PTR   objectId
    )
{
    InstrStr(clrieStrFullName);
    pMethodInfo->GetFullName(&clrieStrFullName.m_bstr);

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);

    tstring strMessage(_T("    <ExceptionCatcherEnter>"));
    strMessage.append(clrieStrFullName);
    strMessage.append(_T("</ExceptionCatcherEnter>"));

    spLogger->LogDumpMessage(strMessage.c_str());

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionCatcherLeave()
{
    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogDumpMessage(_T("    <ExceptionCatcherLeave/>"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchCatcherFound(
    _In_ IMethodInfo* pMethodInfo
    )
{
    InstrStr(clrieStrFullName);
    pMethodInfo->GetFullName(&clrieStrFullName.m_bstr);

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);

    tstring strMessage(_T("    <ExceptionSearchCatcherFound>"));
    strMessage.append(clrieStrFullName);
    strMessage.append(_T("</ExceptionSearchCatcherFound>"));

    spLogger->LogDumpMessage(strMessage.c_str());

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchFilterEnter(
    _In_ IMethodInfo* pMethodInfo
    )
{
    InstrStr(clrieStrFullName);
    pMethodInfo->GetFullName(&clrieStrFullName.m_bstr);

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);

    tstring strMessage(_T("    <ExceptionSearchFilterEnter>"));
    strMessage.append(clrieStrFullName);
    strMessage.append(_T("</ExceptionSearchFilterEnter>"));

    spLogger->LogDumpMessage(strMessage.c_str());

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchFilterLeave()
{
    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogDumpMessage(_T("    <ExceptionCatcherLeave/>"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchFunctionEnter(
    _In_ IMethodInfo* pMethodInfo
    )
{
    InstrStr(clrieStrFullName);
    pMethodInfo->GetFullName(&clrieStrFullName.m_bstr);

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);

    tstring strMessage(_T("    <ExceptionSearchFunctionEnter>"));
    strMessage.append(clrieStrFullName);
    strMessage.append(_T("</ExceptionSearchFunctionEnter>"));

    spLogger->LogDumpMessage(strMessage.c_str());

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchFunctionLeave()
{
    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogDumpMessage(_T("    <ExceptionSearchFunctionLeave/>"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionThrown(
    _In_ UINT_PTR thrownObjectId
    )
{
    HRESULT hr = S_OK;
    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogDumpMessage(_T("    <ExceptionThrown>"));

    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    IfFailRet(m_pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo));

    CComPtr<ICorProfilerInfo2> pCorProfilerInfo2;
    pCorProfilerInfo->QueryInterface(__uuidof(ICorProfilerInfo2), (void**)&pCorProfilerInfo2);

    IfFailRet(pCorProfilerInfo2->DoStackSnapshot(
        NULL,
        StackSnapshotCallbackExceptionThrown,
        COR_PRF_SNAPSHOT_DEFAULT,
        this,
        nullptr,
        0));

    spLogger->LogDumpMessage(_T("    </ExceptionThrown>"));

    return S_OK;
}

// static
HRESULT CInstrumentationMethod::StackSnapshotCallbackExceptionThrown(
    _In_ FunctionID funcId,
    _In_ UINT_PTR ip,
    _In_ COR_PRF_FRAME_INFO frameInfo,
    _In_ ULONG32 contextSize,
    _In_reads_(contextSize) BYTE context[],
    _In_ void *clientData
    )
{
    HRESULT hr = S_OK;
    CInstrumentationMethod* pThis = (CInstrumentationMethod*)clientData;

    IfFailRet(pThis->HandleStackSnapshotCallbackExceptionThrown(
        funcId,
        ip,
        frameInfo,
        contextSize,
        context
        ));

    return S_OK;
}

HRESULT CInstrumentationMethod::HandleStackSnapshotCallbackExceptionThrown(
    _In_ FunctionID funcId,
    _In_ UINT_PTR ip,
    _In_ COR_PRF_FRAME_INFO frameInfo,
    _In_ ULONG32 contextSize,
    _In_reads_(contextSize) BYTE context[]
    )
{
    HRESULT hr = S_OK;

    if (funcId == 0)
    {
        // Can't do anything if there is no function id
        return S_OK;
    }

    CComPtr<IAppDomainCollection> pAppDomainCollection;
    IfFailRet(m_pProfilerManager->GetAppDomainCollection(&pAppDomainCollection));

    CComPtr<IMethodInfo> pMethodInfo;
    IfFailRet(pAppDomainCollection->GetMethodInfoById(funcId, &pMethodInfo));

    CInstrStr clrieStrMethodName;
    IfFailRet(pMethodInfo->GetName(&clrieStrMethodName.m_bstr));

    CComPtr<IModuleInfo> pModuleInfo;
    IfFailRet(pMethodInfo->GetModuleInfo(&pModuleInfo));

    CInstrStr clrieStrModuleName;
    IfFailRet(pModuleInfo->GetModuleName(&clrieStrModuleName.m_bstr));

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);

    tstring strMessage;
    // Add [TestIgnore] for the TestAppRunner. This module is used for bootstrapping
    // .NET Core test assemblies and is not intended to be used in test baselines.
    if (wcscmp(clrieStrModuleName, _T("TestAppRunner.dll")) == 0)
    {
        strMessage.append(_T("[TestIgnore]"));
    }
    strMessage.append(_T("        <MethodName>"));
    strMessage.append(clrieStrModuleName);
    strMessage.append(_T("!"));
    strMessage.append(clrieStrMethodName);
    strMessage.append(_T("</MethodName>"));

    spLogger->LogDumpMessage(strMessage.c_str());

    // verify other code paths that don't contribute to actual baseline
    // This is just to get coverage on althernative code paths to obtain method infos
    mdToken methodToken;
    IfFailRet(pMethodInfo->GetMethodToken(&methodToken));

    CComPtr<IMethodInfo> pMethodInfo2;
    IfFailRet(pModuleInfo->GetMethodInfoByToken(methodToken, &pMethodInfo2));

    CInstrStr clrieStrMethodName2;
    IfFailRet(pMethodInfo2->GetName(&clrieStrMethodName2.m_bstr));

    if (tstring(clrieStrMethodName) != tstring(clrieStrMethodName2))
    {
        spLogger->LogDumpMessage(_T("pModuleInfo->GetMethodInfoByToken did not return same method as pAppDomainCollection->GetMethodInfoById"));
        return S_OK;
    }

    CComPtr<IInstructionGraph> pInstructionGraph;
    IfFailRet(pMethodInfo2->GetInstructions(&pInstructionGraph));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionUnwindFinallyEnter(
    _In_ IMethodInfo* pMethodInfo
    )
{
    CInstrStr clrieStrFullName;
    pMethodInfo->GetFullName(&clrieStrFullName.m_bstr);

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);

    tstring strMessage(_T("    <ExceptionSearchFunctionEnter>"));
    strMessage.append(clrieStrFullName);
    strMessage.append(_T("</ExceptionSearchFunctionEnter>"));

    spLogger->LogDumpMessage(strMessage.c_str());

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionUnwindFinallyLeave()
{
    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogDumpMessage(_T("    <ExceptionUnwindFinallyLeave/>"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionUnwindFunctionEnter(
    _In_ IMethodInfo* pMethodInfo
    )
{
    CInstrStr clrieStrFullName;
    pMethodInfo->GetFullName(&clrieStrFullName.m_bstr);

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);

    tstring strMessage(_T("    <ExceptionUnwindFunctionEnter>"));
    strMessage.append(clrieStrFullName);
    strMessage.append(_T("</ExceptionUnwindFunctionEnter>"));

    spLogger->LogDumpMessage(strMessage.c_str());

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionUnwindFunctionLeave()
{
    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogDumpMessage(_T("    <ExceptionUnwindFunctionLeave/>"));

    return S_OK;
}
