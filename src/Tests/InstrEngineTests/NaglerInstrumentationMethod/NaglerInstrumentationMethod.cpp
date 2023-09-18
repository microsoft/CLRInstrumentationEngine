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
#include "SystemUtils.h"

#include <type_traits>

const WCHAR CInstrumentationMethod::TestOutputPathEnvName[] = _T("Nagler_TestOutputPath");
const WCHAR CInstrumentationMethod::TestScriptFileEnvName[] = _T("Nagler_TestScript");
const WCHAR CInstrumentationMethod::TestScriptFolder[] = _T("TestScripts");
const WCHAR CInstrumentationMethod::IsRejitEnvName[] = _T("Nagler_IsRejit");

//const WCHAR k_wszEnteredFunctionProbeName[] = _T("OnEnter");
//const WCHAR k_wszExitedFunctionProbeName[] = _T("OnExit");

// When pumping managed helpers into mscorlib, stick them into this pre-existing mscorlib type
const WCHAR k_wszHelpersContainerType[] = _T("System.CannotUnloadAppDomainException");

const WCHAR ManagedTracingApiAssemblyNameClassic[] = _T("Instana.ManagedTracing.Api");
const WCHAR ManagedTracingAssemblyNameClassic[] = _T("Instana.ManagedTracing.Base");
const WCHAR ManagedTracingTracerClassNameClassic[] = _T("Instana.ManagedTracing.Base.Tracer");

const WCHAR ManagedTracingAssemblyNameCore[] = _T("Instana.Tracing.Core");
const WCHAR ManagedTracingAssemblyNameCoreCommon[] = _T("Instana.Tracing.Core.Common");
const WCHAR ManagedTracingAssemblyNameCoreTransport[] = _T("Instana.Tracing.Core.Transport");
const WCHAR ManagedTracingAssemblyNameApi[] = _T("Instana.Tracing.Api");
const WCHAR ManagedTracingTracerClassNameCore[] = _T("Instana.Tracing.Tracer");

#ifdef _WINDOWS
std::wstring k_wszEnteredFunctionProbeName(L"OnEnter");
std::wstring k_wszExitedFunctionProbeName(L"OnExit");

std::wstring ManagedTracingAssemblyName(L"Instana.Tracing.Core");
std::wstring ManagedTracingTracerClassName(L"Instana.Tracing.Tracer");
#else
std::u16string k_wszEnteredFunctionProbeName(u"OnEnter");
std::u16string k_wszExitedFunctionProbeName(u"OnExit");

std::u16string ManagedTracingAssemblyName(u"Instana.Tracing.Core");
std::u16string ManagedTracingTracerClassName(u"Instana.Tracing.Tracer");
#endif

COR_SIGNATURE sigFunctionEnterProbe[] = {
    IMAGE_CEE_CS_CALLCONV_DEFAULT,      // default calling convention
    0x05,                               // number of arguments == 2
    ELEMENT_TYPE_OBJECT,                  // return type == object
    ELEMENT_TYPE_OBJECT,
    ELEMENT_TYPE_SZARRAY, ELEMENT_TYPE_OBJECT,
    ELEMENT_TYPE_STRING,				// type name
    ELEMENT_TYPE_STRING,					// method name
    ELEMENT_TYPE_STRING					// custom attribute name
};

COR_SIGNATURE sigFunctionExitProbe[] = {
    IMAGE_CEE_CS_CALLCONV_DEFAULT,      // default calling convention
    0x06,                               // number of arguments == 3
    ELEMENT_TYPE_VOID,                  // return type == void
    ELEMENT_TYPE_OBJECT,				// instrumented object
    ELEMENT_TYPE_OBJECT,				// context obtained by entering
    ELEMENT_TYPE_SZARRAY, ELEMENT_TYPE_OBJECT, // parameters
    ELEMENT_TYPE_STRING,				// type name
    ELEMENT_TYPE_STRING,					// method name
    ELEMENT_TYPE_STRING					// custom attribute name
};

void AssertLogFailure(_In_ const WCHAR* wszError, ...)
{
#ifdef PLATFORM_UNIX
    // Attempting to interpret the var args is very difficult
    // cross-plat without a PAL. We should see about removing
    // the dependency on this in the Common.Lib
    string str;
    HRESULT hr = SystemString::Convert(wszError, str);
    if (!SUCCEEDED(hr))
    {
        str = "Unable to convert wchar string to char string";
    }
    AssertFailed(str.c_str());
#else
    // Since this is a test libary, we will print to standard error and fail
    va_list argptr;
    va_start(argptr, wszError);
    vfwprintf(stderr, wszError, argptr);
#endif
    throw "Assert failed";
}


// Convenience macro for defining strings.
#define InstrStr(_V) CInstrumentationEngineString _V(m_pStringManager)

ILOpcodeInfo ilOpcodeInfo[] =
{
#ifdef PLATFORM_UNIX
#define OPDEF(ord, code, name,  opcodeLen, operandLen, type, alt, flags, pop, push) \
    { name, (DWORD)opcodeLen, (DWORD)operandLen, type, alt, flags, (DWORD)pop, (DWORD)push},
#else
#define OPDEF(ord, code, name,  opcodeLen, operandLen, type, alt, flags, pop, push) \
    { name, (DWORD)opcodeLen, (DWORD)operandLen, ##type, alt, flags, (DWORD)pop, (DWORD)push},
#endif
#include "ILOpcodes.h"
#include "ModuleInfo.h"
#undef OPDEF
};

struct ComInitializer
{
#ifndef PLATFORM_UNIX
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
#endif
};

HRESULT CInstrumentationMethod::Initialize(_In_ IProfilerManager* pProfilerManager)
{
    HRESULT hr;
    CComPtr<IProfilerManagerLogging> spGlobalLogger;
    CComPtr<IProfilerManager4> pProfilerManager4;
    IfFailRet(pProfilerManager->QueryInterface(&pProfilerManager4));
    pProfilerManager4->GetGlobalLoggingInstance(&spGlobalLogger);

    spGlobalLogger->LogDumpMessage(_T("Test module initialize"));


    m_pProfilerManager = pProfilerManager;
    IfFailRet(m_pProfilerManager->QueryInterface(&m_pStringManager));
    spGlobalLogger->LogDumpMessage(_T("Test module QueryInterface"));

#ifdef PLATFORM_UNIX
    DWORD retVal = LoadInstrumentationMethodXml(this);
#else
    // On Windows, xml reading is done in a single-threaded apartment using 
    // COM, so we need to spin up a new thread for it.
    CHandle hConfigThread;
    hConfigThread.Attach(CreateThread(NULL, 0, LoadInstrumentationMethodXml, this, 0, NULL));
    DWORD retVal = WaitForSingleObject(hConfigThread, 15000);
#endif
    spGlobalLogger->LogDumpMessage(_T("Test module Thread is done"));

    if (m_bTestInstrumentationMethodLogging)
    {
        CComPtr<IProfilerManagerLogging> spGlobalLogger;
        CComPtr<IProfilerManager4> pProfilerManager4;
        IfFailRet(pProfilerManager->QueryInterface(&pProfilerManager4));
        pProfilerManager4->GetGlobalLoggingInstance(&spGlobalLogger);
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
DWORD WINAPI CInstrumentationMethod::LoadInstrumentationMethodXml(
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

    tstring testScript;
    IfFailRet(NaglerSystemUtils::GetEnvVar(TestScriptFileEnvName, testScript));

    IfFailRet(NaglerSystemUtils::GetEnvVar(TestOutputPathEnvName, m_strBinaryDir));

    CComPtr<CXmlDocWrapper> pDocument;
    pDocument.Attach(new CXmlDocWrapper());

    hr = pDocument->LoadFile(testScript.c_str());
    if (hr != S_OK)
    {
        AssertFailed("Failed to load test configuration");
        return E_FAIL;
    }

    CComPtr<CXmlNode> pDocumentNode;
    IfFailRet(pDocument->GetRootNode(&pDocumentNode));

    tstring strDocumentNodeName;
    IfFailRet(pDocumentNode->GetName(strDocumentNodeName));

    if (wcscmp(strDocumentNodeName.c_str(), _T("InstrumentationTestScript")) != 0)
    {
        return E_FAIL;
    }


    CComPtr<CXmlNode> pChildNode;
    IfFailRet(pDocumentNode->GetChildNode(&pChildNode));

    while (pChildNode != nullptr)
    {
        tstring strCurrNodeName;
        IfFailRet(pChildNode->GetName(strCurrNodeName));

        if (wcscmp(strCurrNodeName.c_str(), _T("InstrumentMethod")) == 0)
        {
            IfFailRet(ProcessInstrumentMethodNode(pChildNode));
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("ExceptionTracking")) == 0)
        {
            m_bExceptionTrackingEnabled = true;
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("InjectAssembly")) == 0)
        {
#ifndef PLATFORM_UNIX
            shared_ptr<CInjectAssembly> spNewInjectAssembly(new CInjectAssembly());
            ProcessInjectAssembly(pChildNode, spNewInjectAssembly);
            m_spInjectAssembly = spNewInjectAssembly;
#else
            AssertFailed("Inject Assembly tests are currently only supported on Windows platforms.");
#endif
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("MethodLogging")) == 0)
        {
            m_bTestInstrumentationMethodLogging = true;
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("#comment")) == 0)
        {
            // do nothing
        }
        else
        {
            std::string utf8NodeName;
            SystemString::Convert(strCurrNodeName.c_str(), utf8NodeName);
            AssertFailed("Invalid configuration. Element should be InstrumentationMethod");
            AssertFailed(utf8NodeName.c_str());
            return E_FAIL;
        }

        pChildNode = pChildNode->Next();
    }

    return 0;
}

HRESULT CInstrumentationMethod::ProcessInstrumentMethodNode(CXmlNode* pNode)
{
    HRESULT hr = S_OK;

    tstring isRejitEnvValue;
    IfFailRet(NaglerSystemUtils::GetEnvVar(IsRejitEnvName, isRejitEnvValue));
    bool rejitAllInstruMethods = (wcscmp(isRejitEnvValue.c_str(), _T("True")) == 0);

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

        if (wcscmp(strCurrNodeName.c_str(), _T("ModuleName")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            IfFailRet(pChildValue->GetStringValue(moduleName));
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("MethodName")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));
            IfFailRet(pChildValue->GetStringValue(methodName));
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("Instructions")) == 0)
        {
            tstring baselineAttr;
            pChildNode->GetAttribute(_T("Baseline"), baselineAttr);
            isReplacement = !baselineAttr.empty();

            ProcessInstructionNodes(pChildNode, instructions, isReplacement != 0);
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("IsRejit")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            tstring nodeValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));
            IfFailRet(pChildValue->GetStringValue(nodeValue));
            bIsRejit = (wcscmp(nodeValue.c_str(), _T("True")) == 0);
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("AddExceptionHandler")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring nodeValue;
            IfFailRet(pChildValue->GetStringValue(nodeValue));
            isAddExceptionHandler = (wcscmp(nodeValue.c_str(), _T("True")) == 0);
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("CorIlMap")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring varNodeValue;
            IfFailRet(pChildValue->GetStringValue(varNodeValue));

            // This isn't a great way to do things because it is pretty slow, but there doesn't
            // seem to be a good cross-platform way of converting strings to integers
            // that matches the patterns that we are currently using.
            // We may want to update the xml reader to support using whatever encoding
            // is used on the system.
            std::string utf8NodeValue;
            SystemString::Convert(varNodeValue.c_str(), utf8NodeValue);

            stringstream corIlMapString(utf8NodeValue);
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
        else if (wcscmp(strCurrNodeName.c_str(), _T("Locals")) == 0)
        {
            ProcessLocals(pChildNode, locals);
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("PointTo")) == 0)
        {
            std::shared_ptr<CInstrumentMethodPointTo> spPointer(new CInstrumentMethodPointTo());
            spPointTo = spPointer;
            ProcessPointTo(pChildNode, *spPointTo);
        }
        else if (wcscmp(strCurrNodeName.c_str(), _T("MakeSingleRet")) == 0)
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
        else if (wcscmp(strCurrNodeName.c_str(), _T("#comment")) == 0)
        {
            // do nothing. Get next node.
        }
        else
        {
            std::string utf8NodeName;
            SystemString::Convert(strCurrNodeName.c_str(), utf8NodeName);
            AssertFailed("Invalid configuration. Unknown Element");
            AssertFailed(utf8NodeName.c_str());
            return E_FAIL;
        }

        pChildNode = pChildNode->Next();
    }

    if ((moduleName.empty()) ||
        (methodName.empty()))
    {
        AssertFailed("Invalid configuration. Missing child element");
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
        if (wcscmp(nodeName.c_str(), _T("#comment")) == 0)
        {
            // do nothing, get next node.
        }
        else if (wcscmp(nodeName.c_str(), _T("AssemblyName")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring varNodeValue;
            IfFailRet(pChildValue->GetStringValue(pointTo.m_assemblyName));
        }
        else if (wcscmp(nodeName.c_str(), _T("TypeName")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            tstring varNodeValue;
            IfFailRet(pChildValue->GetStringValue(pointTo.m_typeName));
        }
        else if (wcscmp(nodeName.c_str(), _T("MethodName")) == 0)
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
        if (wcscmp(nodeName.c_str(), _T("#comment")) == 0)
        {
            // do nothing. Get next node.
        }
        else if (wcscmp(nodeName.c_str(), _T("Target")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            IfFailRet(pChildValue->GetStringValue(injectAssembly->m_targetAssemblyName));
        }
        else if (wcscmp(nodeName.c_str(), _T("Source")) == 0)
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
        if (wcscmp(nodeName.c_str(), _T("#comment")) == 0)
        {
            // do nothing.
        }
        else
        {
            if (wcscmp(nodeName.c_str(), _T("Local")) != 0)
            {
                AssertFailed("Invalid element");
                return E_UNEXPECTED;
            }

            tstring typeName;
            IfFailRet(pChildNode->GetAttribute(_T("Type"), typeName));

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

        if (wcscmp(currNodeName.c_str(), _T("#comment")) == 0)
        {
            // do nothing
        }
        else if (wcscmp(currNodeName.c_str(), _T("Instruction")) == 0)
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

                if (wcscmp(currInstructionNodeName.c_str(), _T("Opcode")) == 0)
                {
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strOpcode;
                    IfFailRet(pChildValue->GetStringValue(strOpcode));

                    IfFailRet(ConvertOpcode(strOpcode.c_str(), &opcode, &opcodeInfo));
                    bOpcodeSet = true;
                }
                else if (wcscmp(currInstructionNodeName.c_str(), _T("Offset")) == 0)
                {
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strOffset;
                    IfFailRet(pChildValue->GetStringValue(strOffset));
                    dwOffset = _wtoi(strOffset.c_str());
                    bOffsetSet = true;
                }
                else if (wcscmp(currInstructionNodeName.c_str(), _T("Operand")) == 0)
                {
                    if (opcodeInfo.m_operandLength == 0)
                    {
                        AssertFailed("Invalid configuration. Opcode should not have an operand");
                    }
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strOperand;
                    IfFailRet(pChildValue->GetStringValue(strOperand));
                    operand = _wtoi64(strOperand.c_str());

                    // For now, only support integer operands
                    if (!(opcodeInfo.m_type == ILOperandType_None ||
                        opcodeInfo.m_type == ILOperandType_Byte ||
                        opcodeInfo.m_type == ILOperandType_Int ||
                        opcodeInfo.m_type == ILOperandType_UShort ||
                        opcodeInfo.m_type == ILOperandType_Long ||
                        opcodeInfo.m_type == ILOperandType_Token
                        ))
                    {
                        AssertFailed("Unrecognized opcode");
                    }


                }
                else if (wcscmp(currInstructionNodeName.c_str(), _T("InstrumentationType")) == 0)
                {
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strInstrType;
                    IfFailRet(pChildValue->GetStringValue(strInstrType));

                    if (wcscmp(strInstrType.c_str(), _T("InsertBefore")) == 0)
                    {
                        instrType = InsertBefore;
                    }
                    else if (wcscmp(strInstrType.c_str(), _T("InsertAfter")) == 0)
                    {
                        instrType = InsertAfter;
                    }
                    else if (wcscmp(strInstrType.c_str(), _T("Replace")) == 0)
                    {
                        instrType = Replace;
                    }
                    else if (wcscmp(strInstrType.c_str(), _T("Remove")) == 0)
                    {
                        instrType = Remove;
                    }
                    else if (wcscmp(strInstrType.c_str(), _T("RemoveAll")) == 0)
                    {
                        instrType = RemoveAll;
                    }
                }
                else if (wcscmp(currInstructionNodeName.c_str(), _T("RepeatCount")) == 0)
                {
                    CComPtr<CXmlNode> pChildValue;
                    IfFailRet(pInstructionChildNode->GetChildNode(&pChildValue));

                    tstring strRepeatCount;
                    IfFailRet(pChildValue->GetStringValue(strRepeatCount));

                    dwRepeatCount = _wtoi(strRepeatCount.c_str());
                }
                else
                {
                    AssertFailed("Invalid configuration. Unknown Element");
                    return E_FAIL;
                }

                pInstructionChildNode = pInstructionChildNode->Next();
            }

            if ((!isBaseline) && (instrType != Remove) && (instrType != RemoveAll) && (!bOpcodeSet || !bOffsetSet))
            {
                AssertFailed("Invalid configuration. Instruction must have an offset");
                return E_FAIL;
            }

            if (((instrType == Replace) || (instrType == Remove) || (instrType == RemoveAll)) && (dwRepeatCount != 1))
            {
                AssertFailed("Invalid configuration. Incorrect repeat count");
                return E_FAIL;
            }

            shared_ptr<CInstrumentInstruction> pInstrumentInstruction = make_shared<CInstrumentInstruction>(dwOffset, opcode, opcodeInfo, operand, instrType, dwRepeatCount);
            instructions.push_back(pInstrumentInstruction);
        }
        else
        {
            AssertFailed("Invalid configuration. Unknown Element");
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
    _In_ IAppDomainInfo* pAppDomainInfo)
{
    return S_OK;
}

HRESULT CInstrumentationMethod::OnAppDomainShutdown(
    _In_ IAppDomainInfo* pAppDomainInfo)
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
    HRESULT hr = S_OK;

    CComPtr<IProfilerManagerLogging> spLogger;
    CComPtr<IProfilerManager4> pProfilerManager4;
    IfFailRet(m_pProfilerManager->QueryInterface(&pProfilerManager4));
    pProfilerManager4->GetGlobalLoggingInstance(&spLogger);
    try
    {
        ModuleID moduleID;
        pModuleInfo->GetModuleID(&moduleID);

        LPCBYTE pbBaseLoadAddr;
        WCHAR wszName[512];
        ULONG cchNameIn = 512;
        ULONG cchNameOut;
        AssemblyID assemblyID;
        DWORD dwModuleFlags;

        spLogger->LogMessage(_T("Krecemoooo!!!!"));
        ICorProfilerInfo4* m_pProfilerInfo;
        try
        {

            hr = m_pProfilerManager->GetCorProfilerInfo((IUnknown**)&m_pProfilerInfo);
            spLogger->LogMessage(_T("Prosao poziv GetCorProfilerInfo"));
            if (FAILED(hr))
            {
                spLogger->LogMessage(_T("Failed!!!!"));
                return hr;
            }
            spLogger->LogMessage(_T("Krece poziv GetModuleInfo2"));
            hr = m_pProfilerInfo->GetModuleInfo2(
                moduleID,
                &pbBaseLoadAddr,
                cchNameIn,
                &cchNameOut,
                wszName,
                &assemblyID,
                &dwModuleFlags);
            spLogger->LogMessage(_T("Prosao poziv GetModuleInfo2"));
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
            spLogger->LogMessage(_T("GetModuleInfo failed for module"));
            return S_OK;
        }

        // adjust the buffer's size, call again
        if (cchNameIn <= cchNameOut)
        {
            spLogger->LogMessage(_T("The reserved buffer for the module's name was not large enough. Skipping."));
            return S_OK;
        }

        /*std::string logMessage2("Module loaded:  ");
        std::wstring lm2 = std::wstring(logMessage2.begin(), logMessage2.end());
        lm2 = lm2 + wszName;
        spLogger->LogMessage(lm2.c_str());*/

        //if ((dwModuleFlags & COR_PRF_MODULE_WINDOWS_RUNTIME) != 0)
        //{
        //    // Ignore any Windows Runtime modules.  We cannot obtain writeable metadata
        //    // interfaces on them or instrument their IL
        //    spLogger->LogMessage(_T("This is a runtime windows module, skipping"));
        //    return S_OK;
        //}

        AppDomainID appDomainID;
        ModuleID modIDDummy;
        ULONG assemblyNameLength = 0;
        WCHAR assemblyName[255];

        spLogger->LogMessage(_T("Krece poziv InstrStr"));
        //InstrStr(assemblyName);
        spLogger->LogMessage(_T("Prosao poziv InstrStr"));

        spLogger->LogMessage(_T("Krece poziv GetAssemblyInfo"));
        hr = m_pProfilerInfo->GetAssemblyInfo(
            assemblyID,
            255,
            &assemblyNameLength,
            assemblyName,
            &appDomainID,
            &modIDDummy);
        spLogger->LogMessage(_T("Prosao poziv GetAssemblyInfo"));

        if (FAILED(hr))
        {
            spLogger->LogMessage(_T("GetAssemblyInfo failed for module"));
            return S_OK;
        }

        spLogger->LogMessage(assemblyName);

        WCHAR libName[] = _T("AspNetCore3Test");
        if (wcscmp(assemblyName, libName) != 0)
        {
            spLogger->LogMessage(_T("this is not AspNetCore3Test..."));
            return S_OK;
        }

        spLogger->LogMessage(_T("Krece poziv wcscmp"));
        WCHAR mscorlibName[] = _T("mscorlib");
        if (wcscmp(assemblyName, mscorlibName) == 0)
        {
            spLogger->LogMessage(_T("this is mscorlib, noone does anything here..."));
            return S_OK;
        }
        spLogger->LogMessage(_T("Prosao poziv wcscmp"));

        ////bool instrumentationLoaded = false;
        ////// make sure we have the advices loaded from the config-file
        ////if (!instrumentationLoaded)
        ////{
        ////    instrumentationLoaded = true;
        ////}

        ////std::string logMessage("Will instrument "); // initialized elsewhere
        ////std::wstring lm = std::wstring(logMessage.begin(), logMessage.end());
        ////lm = lm + assemblyName;
        ////spLogger->LogMessage(lm.c_str());

        //CComPtr<IMetaDataEmit> pEmit;
        //{
        //    CComPtr<IUnknown> pUnk;
        //    hr = m_pProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataEmit, &pUnk);
        //    if (FAILED(hr))
        //    {
        //        spLogger->LogMessage(_T("GetModuleMetaData failed for module"));
        //        return S_OK;
        //    }

        //    hr = pUnk->QueryInterface(IID_IMetaDataEmit, (LPVOID*)&pEmit);
        //    if (FAILED(hr))
        //    {
        //        spLogger->LogMessage(_T("QueryInterface IID_IMetaDataEmit failed for module"));
        //        return S_OK;
        //    }
        //}

        //CComPtr<IMetaDataImport> pImport;
        //{
        //    CComPtr<IUnknown> pUnk;

        //    hr = m_pProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataImport, &pUnk);
        //    if (FAILED(hr))
        //    {
        //        spLogger->LogMessage(_T("GetModuleMetaData failed for module"));
        //        return S_OK;
        //    }

        //    hr = pUnk->QueryInterface(IID_IMetaDataImport, (LPVOID*)&pImport);
        //    if (FAILED(hr))
        //    {
        //        spLogger->LogMessage(_T("QueryInterface IID_IMetaDataImport failed for module"));
        //        return S_OK;
        //    }
        //}

        //moduleInfo = { 0 };
        //moduleInfo.m_pImport = pImport;
        //moduleInfo.m_pImport->AddRef();
        //////moduleInfo.m_pMethodDefToLatestVersionMap = new MethodDefToLatestVersionMap();

        ////if (wcscpy_s(moduleInfo.m_wszModulePath, _countof(moduleInfo.m_wszModulePath), wszName) != 0)
        ////{
        ////    spLogger->LogMessage(_T("Failed to store module path", wszName));
        ////    return S_OK;
        ////}

        ////if (wcscpy_s(moduleInfo.m_assemblyName, _countof(moduleInfo.m_assemblyName), assemblyName) != 0)
        ////{
        ////    spLogger->LogMessage(_T("Failed to store assemblyName {}", assemblyName));
        ////    return S_OK;
        ////}

        //// Add the references to our helper methods.
        //CComPtr<IMetaDataAssemblyEmit> pAssemblyEmit;
        //{
        //    CComPtr<IUnknown> pUnk;

        //    hr = m_pProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataAssemblyEmit, &pUnk);
        //    if (FAILED(hr))
        //    {
        //        spLogger->LogMessage(_T("GetModuleMetaData IID_IMetaDataAssemblyEmit failed for module"));
        //        return S_OK;
        //    }


        //    hr = pUnk->QueryInterface(IID_IMetaDataAssemblyEmit, (LPVOID*)&pAssemblyEmit);
        //    if (FAILED(hr))
        //    {
        //        //g_logger->error("IID_IMetaDataEmit: QueryInterface failed for ModuleID {}", moduleID);			LogInfo(L"QueryInterface IID_IMetaDataImport failed for module {}", moduleID);
        //        spLogger->LogMessage(_T("QueryInterface IID_IMetaDataAssemblyEmit failed for module"));
        //        return S_OK;
        //    }
        //}

        //CComPtr<IMetaDataAssemblyImport> pAssemblyImport;
        //{
        //    CComPtr<IUnknown> pUnk;

        //    hr = m_pProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyImport, &pUnk);
        //    if (FAILED(hr))
        //    {
        //        spLogger->LogMessage(_T("GetModuleMetaData IID_IMetaDataAssemblyImport failed for module"));
        //        return S_OK;
        //    }

        //    hr = pUnk->QueryInterface(IID_IMetaDataAssemblyImport, (LPVOID*)&pAssemblyImport);
        //    if (FAILED(hr))
        //    {
        //        spLogger->LogMessage(_T("QueryInterface IID_IMetaDataAssemblyImport failed for module"));
        //        return S_OK;
        //    }

        //}
        //bool instrumentThisModule = true;

        //moduleInfo.m_pAssemblyImport = pAssemblyImport;
        //moduleInfo.m_pAssemblyImport->AddRef();
        ////bool relevant = AddMemberRefs(pAssemblyImport, pAssemblyEmit, pEmit, &moduleInfo, instrumentThisModule);
        ////if (relevant)
        ////{
        ////    spLogger->LogMessage(_T("Add member refs is done"));
        ////    //LogInfo(L"Applying instrumentations to {}", moduleInfo.m_assemblyName);
        ////    //m_moduleIDToInfoMap.Update(moduleID, moduleInfo);
        ////    //_instrumentationCoordinator.registerModule(moduleID, moduleInfo);
        ////    //****************ApplyInstrumentations(moduleID, moduleInfo, instrumentThisModule);


        ////    // Append to the list!

        ////    // If we already rejitted functions in other modules with a matching path, then
        ////    // pre-rejit those functions in this module as well.  This takes care of the case
        ////    // where we rejitted functions in a module loaded in one domain, and we just now
        ////    // loaded the same module (unshared) into another domain.  We must explicitly ask to
        ////    // rejit those functions in this domain's copy of the module, since it's identified
        ////    // by a different ModuleID.

        ////    std::vector<ModuleID> rgModuleIDs;
        ////    std::vector<mdToken> rgMethodDefs;


        ////}
        tstring assemblyNameString = tstring(assemblyName);
        spLogger->LogMessage(_T("[PROFILER]\tLoaded module"));

        //// how can we get a path to a file on non-windows OSes (no registry we could hijack... ENV_VARS?)
        /*std::string profilerPath = std::getenv("CORECLR_PROFILER_PATH");
        std::string profilerLibName = "CoreProfiler.so";
        std::string configFileName = "instrumentation.json";*/

        /*std::string cleanPath = profilerPath.substr(0, profilerPath.length() - profilerLibName.length());
        std::string fullPath = cleanPath.append(configFileName);*/

        spLogger->LogMessage(_T("Krece poziv ModuleFlagsAllowInstrumentation"));
        if (ModuleFlagsAllowInstrumentation(dwModuleFlags) != S_OK)
        {
            return S_OK;
        }
        spLogger->LogMessage(_T("Prosao poziv ModuleFlagsAllowInstrumentation"));
        if (IsSystemLib(assemblyNameString))
        {
            return S_OK;
        }
        spLogger->LogMessage(_T("Prosao poziv IsSystemLib"));

        spLogger->LogMessage(_T("Krece poziv GetModuleMetaData"));
        CComPtr<IMetaDataAssemblyImport> metadataAssemblyImport;
        CComPtr<IMetaDataAssemblyEmit> metadataAssemblyEmit;
        m_pProfilerInfo->GetModuleMetaData(moduleID, ofRead | ofWrite, IID_IMetaDataAssemblyImport, reinterpret_cast<IUnknown**>(&metadataAssemblyImport));
        spLogger->LogMessage(_T("Prosao poziv GetModuleMetaData"));

        /*spLogger->LogMessage(_T("Krece poziv GetModuleMetaData"));
        m_pProfilerInfo->GetModuleMetaData(moduleID, ofRead | ofWrite, IID_IMetaDataAssemblyImport, reinterpret_cast<IUnknown**>(&metadataAssemblyImport));
        spLogger->LogMessage(_T("Prosao poziv GetModuleMetaData"));*/

        spLogger->LogMessage(_T("Krece poziv GetModuleMetaData"));
        m_pProfilerInfo->GetModuleMetaData(moduleID, ofRead | ofWrite, IID_IMetaDataAssemblyEmit, reinterpret_cast<IUnknown**>(&metadataAssemblyEmit));
        spLogger->LogMessage(_T("Prosao poziv GetModuleMetaData"));

        CComPtr<IMetaDataEmit> metadataEmit;
        CComPtr<IMetaDataImport> metadataImport;
        spLogger->LogMessage(_T("Krece poziv QueryInterface"));
        if FAILED(metadataAssemblyImport->QueryInterface(IID_IMetaDataEmit, reinterpret_cast<void**>(&metadataEmit)))
        {
            return E_FAIL;
        }
        spLogger->LogMessage(_T("Prosao poziv QueryInterface"));

        spLogger->LogMessage(_T("Krece poziv QueryInterface"));
        if FAILED(metadataAssemblyImport->QueryInterface(IID_IMetaDataImport, reinterpret_cast<void**>(&metadataImport)))
        {
            return E_FAIL;
        }
        spLogger->LogMessage(_T("Prosao poziv QueryInterface"));

        moduleInfo = { 0 };
        moduleInfo.m_pImport = metadataImport;
        moduleInfo.m_pAssemblyImport = metadataAssemblyImport;

        spLogger->LogMessage(_T("Krece poziv copyWideChars"));
        copyWideChars(moduleInfo.m_assemblyName, assemblyName);
        spLogger->LogMessage(_T("Prosao poziv copyWideChars"));

        spLogger->LogMessage(_T("Krece poziv AddTracingReferencesToModule"));
        AddTracingReferencesToModule(metadataAssemblyImport, metadataAssemblyEmit, metadataEmit, &moduleInfo);
        spLogger->LogMessage(_T("Prosao poziv AddTracingReferencesToModule"));
    }
    catch (std::exception& e)
    {
        spLogger->LogMessage(_T("Exception on loading module: {}"));
    }

    return S_OK;
}

WCHAR* CInstrumentationMethod::copyWideChars(WCHAR* dest, const WCHAR* src)
{
    unsigned i;
    for (i = 0; src[i] != '\0'; ++i)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}

bool CInstrumentationMethod::IsSystemLib(tstring assemblyName)
{
    if (assemblyName.compare(_T("Instana.Tracing.Api")) == 0 ||
        assemblyName.compare(_T("Instana.Tracing.Core")) == 0 ||
        assemblyName.compare(_T("Instana.Tracing.Core.Instrumentation")) == 0 ||
        assemblyName.compare(_T("Anonymously Hosted DynamicMethods Assembly")) == 0 ||
        assemblyName.compare(_T("System.Buffers")) == 0 ||
        assemblyName.compare(_T("System.Collections")) == 0 ||
        assemblyName.compare(_T("System.Collections.Concurrent")) == 0 ||
        assemblyName.compare(_T("System.Collections.NonGeneric")) == 0 ||
        assemblyName.compare(_T("System.Collections.Specialized")) == 0 ||
        assemblyName.compare(_T("System.ComponentModel")) == 0 ||
        assemblyName.compare(_T("System.ComponentModel.Annotations")) == 0 ||
        assemblyName.compare(_T("System.ComponentModel.EventBasedAsync")) == 0 ||
        assemblyName.compare(_T("System.ComponentModel.TypeConverter")) == 0 ||
        assemblyName.compare(_T("System.Configuration.ConfigurationManager")) == 0 ||
        assemblyName.compare(_T("System.Diagnostics.Debug")) == 0 ||
        assemblyName.compare(_T("System.Diagnostics.Process")) == 0 ||
        assemblyName.compare(_T("System.Diagnostics.StackTrace")) == 0 ||
        assemblyName.compare(_T("System.Diagnostics.TraceSource")) == 0 ||
        assemblyName.compare(_T("System.Drawing.Primitives")) == 0 ||
        assemblyName.compare(_T("System.IO")) == 0 ||
        assemblyName.compare(_T("System.IO.FileSystem")) == 0 ||
        assemblyName.compare(_T("System.IO.FileSystem.Watcher")) == 0 ||
        assemblyName.compare(_T("System.IO.Pipelines")) == 0 ||
        assemblyName.compare(_T("System.Linq")) == 0 ||
        assemblyName.compare(_T("System.Linq.Expressions")) == 0 ||
        assemblyName.compare(_T("System.Memory")) == 0 ||
        assemblyName.compare(_T("System.ObjectModel")) == 0 ||
        assemblyName.compare(_T("System.Private.Uri")) == 0 ||
        assemblyName.compare(_T("System.Runtime.Numerics")) == 0 ||
        assemblyName.compare(_T("System.Numerics.Vectors")) == 0 ||
        assemblyName.compare(_T("System.Resources.ResourceManager")) == 0 ||
        assemblyName.compare(_T("System.Runtime")) == 0 ||
        assemblyName.compare(_T("System.Runtime.CompilerServices.Unsafe")) == 0 ||
        assemblyName.compare(_T("System.Runtime.InteropServices")) == 0 ||
        assemblyName.compare(_T("System.Runtime.InteropServices.RuntimeInformation")) == 0 ||
        assemblyName.compare(_T("System.Security.Cryptography.Algorithms")) == 0 ||
        assemblyName.compare(_T("System.Security.Cryptography.Encoding")) == 0 ||
        assemblyName.compare(_T("System.Security.Cryptography.OpenSsl")) == 0 ||
        assemblyName.compare(_T("System.Security.Cryptography.Primitives")) == 0 ||
        assemblyName.compare(_T("System.Security.Cryptography.X509Certificates")) == 0 ||
        assemblyName.compare(_T("System.Security.Principal")) == 0 ||
        assemblyName.compare(_T("System.Text.Json")) == 0 ||
        assemblyName.compare(_T("System.Text.Encoding.Extensions")) == 0 ||
        assemblyName.compare(_T("System.Text.Encodings.Web")) == 0 ||
        assemblyName.compare(_T("System.Text.RegularExpressions")) == 0 ||
        assemblyName.compare(_T("System.Threading")) == 0 ||
        assemblyName.compare(_T("System.Threading.Tasks")) == 0 ||
        assemblyName.compare(_T("System.Threading.Thread")) == 0 ||
        assemblyName.compare(_T("System.Threading.Timer")) == 0 ||
        assemblyName.compare(_T("netstandard")) == 0 ||
        assemblyName.compare(_T("Microsoft.Extensions.Configuration.CommandLine")) == 0 ||
        assemblyName.compare(_T("Microsoft.Extensions.FileSystemGlobbing")) == 0 ||
        assemblyName.compare(_T("Microsoft.Extensions.ObjectPool")) == 0 ||
        assemblyName.compare(_T("Microsoft.AspNetCore.Metadata")) == 0 ||
        assemblyName.compare(_T("Microsoft.AspNetCore.Mvc.ViewFeatures")) == 0 ||
        assemblyName.compare(_T("System.Private.Xml")) == 0)
        return true;

    return false;
}

BOOL CInstrumentationMethod::FindMscorlibReference(IMetaDataAssemblyImport* pAssemblyImport, mdAssemblyRef* rgAssemblyRefs, ULONG cAssemblyRefs, mdAssemblyRef* parMscorlib)
{
    HRESULT hr;

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogMessage(_T("Start FindMscorlibReference"));

    for (ULONG i = 0; i < cAssemblyRefs; i++)
    {
        const void* pvPublicKeyOrToken;
        ULONG cbPublicKeyOrToken;
        WCHAR wszName[512];
        ULONG cchNameReturned;
        ASSEMBLYMETADATA asmMetaData;
        memset(&asmMetaData, 0, sizeof(asmMetaData));
        const void* pbHashValue;
        ULONG cbHashValue;
        DWORD asmRefFlags;

        spLogger->LogMessage(_T("Start GetAssemblyRefProps"));
        hr = pAssemblyImport->GetAssemblyRefProps(
            rgAssemblyRefs[i],
            &pvPublicKeyOrToken,
            &cbPublicKeyOrToken,
            wszName,
            512,
            &cchNameReturned,
            &asmMetaData,
            &pbHashValue,
            &cbHashValue,
            &asmRefFlags);
        spLogger->LogMessage(_T("End GetAssemblyRefProps"));

        if (FAILED(hr))
        {
            return FALSE;
        }
        tstring dependencyName = tstring(wszName);
        if (dependencyName.compare(_T("mscorlib")) == 0 ||
            dependencyName.compare(_T("netstandard")) == 0 ||
            dependencyName.compare(_T("System.Runtime")) == 0)
        {
            *parMscorlib = rgAssemblyRefs[i];
            return TRUE;
        }

        return FALSE;
    }

    spLogger->LogMessage(_T("End FindMscorlibReference"));

    return FALSE;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethod::AddTracingReferencesToModule(IMetaDataAssemblyImport* pAssemblyImport, IMetaDataAssemblyEmit* pAssemblyEmit, IMetaDataEmit* pEmit, ModuleInfo* pModuleInfo)
{
    mdAssemblyRef rgMSCAssemblyRefs[20];
    ULONG cMSCAssemblyRefsReturned;
    mdAssemblyRef msCorLibRef = mdAssemblyRefNil;
    HRESULT hr;

    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogMessage(_T("Start AddTracingReferencesToModule"));

    HCORENUM hmscEnum = NULL;
    do
    {
        spLogger->LogMessage(_T("Start EnumAssemblyRefs"));
        hr = pAssemblyImport->EnumAssemblyRefs(
            &hmscEnum,
            rgMSCAssemblyRefs,
            20,
            &cMSCAssemblyRefsReturned);
        spLogger->LogMessage(_T("End EnumAssemblyRefs"));

        if (FAILED(hr))
        {
            return false;
        }

        if (cMSCAssemblyRefsReturned == 0)
        {
            return false;
        }

        printf("%d\n", cMSCAssemblyRefsReturned);

    } while (!FindMscorlibReference(
        pAssemblyImport,
        rgMSCAssemblyRefs,
        cMSCAssemblyRefsReturned,
        &msCorLibRef));
    pAssemblyImport->CloseEnum(hmscEnum);
    hmscEnum = NULL;
#ifdef _WINDOWS
    std::wstring systemObject(L"System.Object");
    std::wstring systemException(L"System.Exception");
    std::wstring systemInt16(L"System.Int16");
    std::wstring systemInt32(L"System.Int32");
    std::wstring systemInt64(L"System.Int64");
    std::wstring systemFloat32(L"System.Single");
    std::wstring systemFloat64(L"System.Double");
    std::wstring systemUInt16(L"System.UInt16");
    std::wstring systemUInt32(L"System.UInt32");
    std::wstring systemUInt64(L"System.UInt64");
    std::wstring systemByte(L"System.Byte");
    std::wstring systemSByte(L"System.SByte");
    std::wstring systemBoolean(L"System.Boolean");
#else
    std::u16string systemObject(u"System.Object");
    std::u16string systemException(u"System.Exception");
    std::u16string systemInt16(u"System.Int16");
    std::u16string systemInt32(u"System.Int32");
    std::u16string systemInt64(u"System.Int64");
    std::u16string systemFloat32(u"System.Single");
    std::u16string systemFloat64(u"System.Double");
    std::u16string systemUInt16(u"System.UInt16");
    std::u16string systemUInt32(u"System.UInt32");
    std::u16string systemUInt64(u"System.UInt64");
    std::u16string systemByte(u"System.Byte");
    std::u16string systemSByte(u"System.SByte");
    std::u16string systemBoolean(u"System.Boolean");

#endif
    hr = GetTypeRef(systemObject.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_objectTypeRef));
    hr = GetTypeRef(systemException.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_exceptionTypeRef));
    hr = GetTypeRef(systemInt16.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_int16TypeRef));
    hr = GetTypeRef(systemInt32.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_int32TypeRef));
    hr = GetTypeRef(systemInt64.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_int64TypeRef));
    hr = GetTypeRef(systemFloat32.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_float32TypeRef));
    hr = GetTypeRef(systemFloat64.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_float64TypeRef));
    hr = GetTypeRef(systemUInt16.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_uint16TypeRef));
    hr = GetTypeRef(systemUInt32.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_uint32TypeRef));
    hr = GetTypeRef(systemUInt64.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_uint64TypeRef));
    hr = GetTypeRef(systemByte.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_byteTypeRef));
    hr = GetTypeRef(systemSByte.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_signedByteTypeRef));
    hr = GetTypeRef(systemBoolean.c_str(), msCorLibRef, pEmit, &(pModuleInfo->m_boolTypeRef));

    mdAssemblyRef assemblyRef = mdTokenNil;
    mdTypeRef typeRef = mdTokenNil;

    // Generate assemblyRef for our managed-tracing assembly
    BYTE rgbPublicKeyToken[] = { 0xa1, 0x6e, 0x91, 0xe2, 0x62, 0x6a, 0xfe, 0x0a };
    WCHAR wszLocale[1] = { L'\0' };

    ASSEMBLYMETADATA assemblyMetaData;
    memset(&assemblyMetaData, 0, sizeof(assemblyMetaData));
    assemblyMetaData.usMajorVersion = 1;
    assemblyMetaData.usMinorVersion = 0;
    assemblyMetaData.usBuildNumber = 0;
    assemblyMetaData.usRevisionNumber = 0;
    assemblyMetaData.szLocale = wszLocale;
#ifdef _WINDOWS
    assemblyMetaData.cbLocale = _countof(wszLocale);
#else
    assemblyMetaData.cbLocale = sizeof(wszLocale);
#endif

    spLogger->LogMessage(_T("Start DefineAssemblyRef"));
    hr = pAssemblyEmit->DefineAssemblyRef(
        (void*)rgbPublicKeyToken,
        sizeof(rgbPublicKeyToken),
        ManagedTracingAssemblyName.c_str(),
        &assemblyMetaData,
        0,                   // hash blob
        0,                   // cb of hash blob
        0,                      // flags
        &assemblyRef);
    // Generate typeRef to our managed tracer
    spLogger->LogMessage(_T("End DefineAssemblyRef"));

    spLogger->LogMessage(_T("Start DefineTypeRefByName"));
    hr = pEmit->DefineTypeRefByName(
        assemblyRef,
        ManagedTracingTracerClassName.c_str(),
        &typeRef);
    spLogger->LogMessage(_T("End DefineTypeRefByName"));

    spLogger->LogMessage(_T("Start DefineMemberRef"));
    hr = pEmit->DefineMemberRef(
        typeRef,
        k_wszEnteredFunctionProbeName.c_str(),
        sigFunctionEnterProbe,
        sizeof(sigFunctionEnterProbe),
        &(pModuleInfo->m_mdEnterProbeRef));
    spLogger->LogMessage(_T("End DefineMemberRef"));

    if (hr != S_OK)
    {
        spLogger->LogMessage(_T("m_mdEnterProbeRef problem"));
    }

    if (pModuleInfo->m_mdEnterProbeRef == mdTokenNil)
    {
        spLogger->LogMessage(_T("m_mdEnterProbeRef is not found"));
    }

    spLogger->LogMessage(_T("Start DefineMemberRef"));
    hr = pEmit->DefineMemberRef(
        typeRef,
        k_wszExitedFunctionProbeName.c_str(),
        sigFunctionExitProbe,
        sizeof(sigFunctionExitProbe),
        &(pModuleInfo->m_mdExitProbeRef));
    spLogger->LogMessage(_T("End DefineMemberRef"));

    spLogger->LogMessage(_T("End AddTracingReferencesToModule"));
    return S_OK;
}

HRESULT CInstrumentationMethod::GetTypeRef(LPCWSTR typeName, mdToken sourceLibraryReference, IMetaDataEmit* pEmit, mdTypeRef* ptr)
{
    HRESULT hr = pEmit->DefineTypeRefByName(sourceLibraryReference, typeName, ptr);
    return hr;
}

HRESULT STDMETHODCALLTYPE CInstrumentationMethod::ModuleFlagsAllowInstrumentation(DWORD dwModuleFlags)
{
    if ((dwModuleFlags & COR_PRF_MODULE_WINDOWS_RUNTIME) != 0 ||
        (dwModuleFlags & COR_PRF_MODULE_RESOURCE) != 0)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CInstrumentationMethod::OnModuleUnloaded(_In_ IModuleInfo* pModuleInfo)
{
    return S_OK;
}

HRESULT CInstrumentationMethod::OnShutdown()
{
    HRESULT hr = S_OK;
    if (m_bExceptionTrackingEnabled)
    {
        CComPtr<IProfilerManagerLogging> spLogger;
        m_pProfilerManager->GetLoggingInstance(&spLogger);
        spLogger->LogDumpMessage(_T("</ExceptionTrace>"));
    }
    else if (m_bTestInstrumentationMethodLogging)
    {
        CComPtr<IProfilerManagerLogging> spLogger;
        CComPtr<IProfilerManager4> pProfilerManager4;
        IfFailRet(m_pProfilerManager->QueryInterface(&pProfilerManager4));
        pProfilerManager4->GetGlobalLoggingInstance(&spLogger);
        spLogger->LogDumpMessage(_T("</InstrumentationMethodLog>"));
    }

    return hr;
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
    m_pProfilerManager->GetLoggingInstance(&spLogger);
    spLogger->LogMessage(_T("ModuleName"));
    spLogger->LogMessage(clrieStrModule);

    InstrStr(clrieStrMethodName);
    pMethodInfo->GetName(&clrieStrMethodName.m_bstr);
    spLogger->LogMessage(_T("MethodName"));
    spLogger->LogMessage(clrieStrMethodName.m_bstr);

    tstring moduleName = clrieStrModule.BStr();

    for (shared_ptr<CInstrumentMethodEntry> pInstrumentMethodEntry : m_instrumentMethodEntries)
    {
        const tstring& enteryModuleName = pInstrumentMethodEntry->GetModuleName();
        spLogger->LogMessage(_T("enteryModuleName"));
        spLogger->LogMessage(enteryModuleName.c_str());

        if (wcscmp(enteryModuleName.c_str(), clrieStrModule) == 0)
        {
            // TODO: Eventually, this will need to use the full name and not the partial name
            InstrStr(clrieStrMethodName);
            pMethodInfo->GetName(&clrieStrMethodName.m_bstr);

            const tstring& entryMethodName = pInstrumentMethodEntry->GetMethodName();
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

    shared_ptr<CInstrumentMethodEntry> pMethodEntry = m_methodInfoToEntryMap[pMethodInfo];

    if (!pMethodEntry)
    {
        AssertFailed("CInstrumentationMethod::InstrumentMethod - why no method entry? It should have been set in ShouldInstrumentMethod");
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
        for (long i = 0; i < baselineSequencePoints.size(); i++)
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
        AssertFailed("CInstrumentationMethod::InstrumentMethod - why no method entry? It should have been set in ShouldInstrumentMethod");
        return E_FAIL;
    }

    /*BOOL bRequireRejit = pMethodEntry->GetIsRejit();
    if (bRequireRejit != isRejit)
    {
        return E_FAIL;
    }*/
    CComPtr<IProfilerManagerLogging> spLogger;
    m_pProfilerManager->GetLoggingInstance(&spLogger);

    CComPtr<IModuleInfo> spModuleInfo;
    IfFailRet(pMethodInfo->GetModuleInfo(&spModuleInfo));

    ModuleID moduleID;
    spModuleInfo->GetModuleID(&moduleID);

    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    m_pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);

    CComPtr<IMetaDataEmit> pEmit;
    {
        CComPtr<IUnknown> pUnk;

        HRESULT hr = pCorProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataEmit, &pUnk);
        hr = pUnk->QueryInterface(IID_IMetaDataEmit, (LPVOID*)&pEmit);
    }


    WCHAR methodName1[] = _T("RequestGet");
    WCHAR* methodName = methodName1;
    ULONG methodNameLength = sizeof(methodName);
    WCHAR typeName1[] = _T("AspNetCore3Test.Program");
    WCHAR* typeName = typeName1;
    ULONG typeLength = sizeof(typeName);
    WCHAR customAttribute1[] = _T("CustomAtt");
    WCHAR* customAttribute = customAttribute1;
    ULONG customAttributeLength = sizeof(customAttribute);

    mdString methodNameStringToken = mdStringNil;
    mdString typeNameStringToken = mdStringNil;
    mdString customAttStringToken = mdStringNil;
    pEmit->DefineUserString(methodName, 10, &methodNameStringToken);
    pEmit->DefineUserString(typeName, 22, &typeNameStringToken);
    pEmit->DefineUserString(customAttribute, 9, &customAttStringToken);

    vector<shared_ptr<CInstrumentInstruction>> instructions = pMethodEntry->GetInstructions();

    CComPtr<IInstructionGraph> pInstructionGraph;
    IfFailRet(pMethodInfo->GetInstructions(&pInstructionGraph));

    /*CComPtr<IUnknown> pUnk;
    HRESULT hr = pCorProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataEmit, &pUnk);
    hr = pUnk->QueryInterface(IID_IMetaDataEmit, (LPVOID*)&pEmit);*/

    BSTR methodName2 = nullptr;
    pMethodInfo->GetFullName(&methodName2);

    spLogger->LogMessage(_T("InstrumentMethod"));
    spLogger->LogMessage(methodName2);

    if (pMethodEntry->IsSingleRetFirst())
    {
        PerformSingleReturnInstrumentation(pMethodInfo, pInstructionGraph);
    }

    if (pMethodEntry->GetPointTo() != nullptr)
    {
        spLogger->LogMessage(_T("point to"));
        std::shared_ptr<CInstrumentMethodPointTo> spPointTo = pMethodEntry->GetPointTo();
        BYTE pSignature[256] = {};
        DWORD cbSignature = 0;
        pMethodInfo->GetCorSignature(extent<decltype(pSignature)>::value, pSignature, &cbSignature);

        /*
        mdToken tkMethodToken = mdTokenNil;

        //CComPtr<IModuleInfo> spModuleInfo;
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

            tstring strName(MAX_PATH, WCHAR());
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
            reinterpret_cast<IUnknown**>(&spIMetaDataImport)));

        mdToken tkTypeToken;

        HRESULT hrFound = spIMetaDataImport->FindTypeRef(tkMscorlibRef, spPointTo->m_typeName.c_str(), &tkTypeToken);
        */

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

        vector<CLocalType> locals;
        locals.push_back(CLocalType(_T("System.Object")));
        locals.push_back(CLocalType(_T("System.Object")));
        pMethodEntry->AddLocals(locals);

        IfFailRet(InstrumentLocals(pMethodInfo, pMethodEntry));

        spLogger->LogMessage(_T("index[0]"));
        printf("%d\n", indexes[0]);
        spLogger->LogMessage(_T("index[1]"));
        printf("%d\n", indexes[1]);

        CComPtr<IInstructionFactory> sptrInstructionFactory;
        IfFailRet(pMethodInfo->GetInstructionFactory(&sptrInstructionFactory));

        CComPtr<IInstructionGraph> sptrInstructionGraph;
        IfFailRet(pMethodInfo->GetInstructions(&sptrInstructionGraph));
        //sptrInstructionGraph->RemoveAll();


        CComPtr<IInstruction> sptrCurrent;
        IfFailRet(sptrInstructionGraph->GetFirstInstruction(&sptrCurrent));

        // store NULL in SpanContext
        //ldNull
        CComPtr<IInstruction> ldNull;
        IfFailRet(sptrInstructionFactory->CreateInstruction(Cee_Ldnull, &ldNull));
        IfFailRet(pInstructionGraph->InsertBefore(sptrCurrent, ldNull));
        //sptrCurrent = ldNull;
        //StLoc
        CComPtr<IInstruction> stLoc;
        IfFailRet(sptrInstructionFactory->CreateStoreLocalInstruction(0, &stLoc));
        IfFailRet(pInstructionGraph->InsertBefore(sptrCurrent, stLoc));
        //sptrCurrent = stLoc;

        // initialize array
        CComPtr<IInstruction> ldcI4;
        IfFailRet(sptrInstructionFactory->CreateInstruction(Cee_Ldc_I4_2, &ldcI4));
        IfFailRet(pInstructionGraph->InsertBefore(sptrCurrent, ldcI4));
        //sptrCurrent = ldcI4;
        CComPtr<IInstruction> newArr;
        IfFailRet(sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Newarr, moduleInfo.m_objectTypeRef, &newArr));
        IfFailRet(pInstructionGraph->InsertBefore(sptrCurrent, newArr));
        //sptrCurrent = newArr;

        CComPtr<IInstruction> stArrLoc;
        IfFailRet(sptrInstructionFactory->CreateStoreLocalInstruction(1, &stArrLoc));
        IfFailRet(pInstructionGraph->InsertBefore(sptrCurrent, stArrLoc));
        //sptrCurrent = stArrLoc;

        /*CComPtr<IInstruction> ldNull3;
        IfFailRet(sptrInstructionFactory->CreateInstruction(Cee_Ldnull, &ldNull3));
        IfFailRet(pInstructionGraph->InsertAfter(sptrCurrent, ldNull3));
        sptrCurrent = ldNull3;
        CComPtr<IInstruction> stArrLoc;
        IfFailRet(sptrInstructionFactory->CreateStoreLocalInstruction(1, &stArrLoc));
        IfFailRet(pInstructionGraph->InsertAfter(sptrCurrent, stArrLoc));
        sptrCurrent = stArrLoc;*/

        //load called object
        CComPtr<IInstruction> sptrLoadArg;
        IfFailRet(sptrInstructionFactory->CreateLoadArgInstruction(0, &sptrLoadArg));
        IfFailRet(sptrInstructionGraph->InsertBefore(sptrCurrent, sptrLoadArg));
        //sptrCurrent = sptrLoadArg;

        /*CComPtr<IInstruction> ldLoc1;
        IfFailRet(sptrInstructionFactory->CreateLoadLocalInstruction(0, &ldLoc1));
        IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, ldLoc1));
        sptrCurrent = ldLoc1;*/

        CComPtr<IInstruction> ldLoc2;
        IfFailRet(sptrInstructionFactory->CreateLoadLocalInstruction(1, &ldLoc2));
        IfFailRet(sptrInstructionGraph->InsertBefore(sptrCurrent, ldLoc2));
        //sptrCurrent = ldLoc2;

        /*CComPtr<IInstruction> ldNull2;
        IfFailRet(sptrInstructionFactory->CreateInstruction(Cee_Ldnull, &ldNull2));
        IfFailRet(pInstructionGraph->InsertAfter(sptrCurrent, ldNull2));
        sptrCurrent = ldNull2;*/

        CComPtr<IInstruction> ldTypeName;
        IfFailRet(sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Ldstr, typeNameStringToken, &ldTypeName));
        IfFailRet(sptrInstructionGraph->InsertBefore(sptrCurrent, ldTypeName));
        //sptrCurrent = ldTypeName;

        CComPtr<IInstruction> ldMethodName;
        IfFailRet(sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Ldstr, methodNameStringToken, &ldMethodName));
        IfFailRet(sptrInstructionGraph->InsertBefore(sptrCurrent, ldMethodName));
        //sptrCurrent = ldMethodName;

        CComPtr<IInstruction> ldCustomAttName;
        IfFailRet(sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Ldstr, customAttStringToken, &ldCustomAttName));
        IfFailRet(sptrInstructionGraph->InsertBefore(sptrCurrent, ldCustomAttName));

        /*for (USHORT i = 0; i < argsCount; i++)
        {
            CComPtr<IInstruction> sptrLoadArg;
            IfFailRet(sptrInstructionFactory->CreateLoadArgInstruction(i, &sptrLoadArg));
            IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrLoadArg));
            sptrCurrent = sptrLoadArg;
        }*/

        if (moduleInfo.m_mdEnterProbeRef == mdTokenNil)
        {
            spLogger->LogMessage(_T("mdToken is nil"));
        }

        CComPtr<IInstruction> sptrCallMethod;

        IfFailRet(sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Call, moduleInfo.m_mdEnterProbeRef, &sptrCallMethod));
        IfFailRet(sptrInstructionGraph->InsertBefore(sptrCurrent, sptrCallMethod));
        //sptrCurrent = sptrCallMethod;

        //CComPtr<IInstruction> sptrReturn;

        //IfFailRet(sptrInstructionFactory->CreateInstruction(Cee_Ret, &sptrReturn));
        //IfFailRet(sptrInstructionGraph->InsertBefore(sptrCurrent, sptrReturn));
        //sptrCurrent = sptrReturn;

        CComPtr<IInstruction> stLoc2;
        IfFailRet(sptrInstructionFactory->CreateStoreLocalInstruction(0, &stLoc2));
        IfFailRet(pInstructionGraph->InsertBefore(sptrCurrent, stLoc2));

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
                        AssertFailed("Switch not implemented yet in profiler host");
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
                                AssertFailed("Switch not implemented yet");
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
                                AssertFailed("Single not implemented yet");
                                return E_FAIL;
                            }
                            else if (opcodeInfo.m_type == ILOperandType_Double)
                            {
                                AssertFailed("Double not implemented yet");
                                return E_FAIL;
                            }
                            else
                            {
                                AssertFailed("Unexpected operand length");
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

    spLogger->LogMessage(_T("Start"));
    printf("%04x", moduleInfo.m_mdEnterProbeRef);
    //IfFailRet(InstrumentLocals(pMethodInfo, pMethodEntry));
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
        AssertFailed("Trying to add exception handler to method with tailcall");
        return E_FAIL;
    }

    if (returnCount > 1) // note it would be valid to have method with zero returns, as we could end in a throw
    {
        AssertFailed("Trying to add exception handler to method with more than one return");
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

    tstring strExceptionTypeName(_T("System.Exception"));

    ULONG chName = MAX_PATH;
    tstring strName(chName, wchar_t());
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
        DWORD locIndex;
        pLocals->GetCount(&locIndex);
        for (const CLocalType& local : locals)
        {
            DWORD indexPtr;
            CComPtr<IType> pType;
            IfFailRet(GetType(pModuleInfo, local, &pType));
            IfFailRet(pLocals->AddLocal(pType, &indexPtr));
            locIndex++;
            indexes.push_back(locIndex);
        }
    }

    return hr;
}

HRESULT CInstrumentationMethod::GetType(IModuleInfo* pModuleInfo, const CLocalType& localType, IType** pType)
{
    HRESULT hr = S_OK;
    const tstring& typeName = localType.GetTypeName();

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
    pCorProfilerInfo->QueryInterface(&pCorProfilerInfo2);

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
    _In_ void* clientData
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
