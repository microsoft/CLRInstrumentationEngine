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
    m_pProfilerManager = pProfilerManager;
    IfFailRet(m_pProfilerManager->QueryInterface(&m_pStringManager));

#ifdef PLATFORM_UNIX
    DWORD retVal = LoadInstrumentationMethodXml(this);
#else
    // On Windows, xml reading is done in a single-threaded apartment using 
    // COM, so we need to spin up a new thread for it.
    CHandle hConfigThread;
    hConfigThread.Attach(CreateThread(NULL, 0, LoadInstrumentationMethodXml, this, 0, NULL));
    DWORD retVal = WaitForSingleObject(hConfigThread, 15000);
#endif

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

            if ( (!isBaseline) && (instrType != Remove) && (instrType != RemoveAll) && (!bOpcodeSet || !bOffsetSet))
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

HRESULT CInstrumentationMethod::OnModuleLoaded(_In_ IModuleInfo* pModuleInfo)
{
    HRESULT hr = S_OK;

    InstrStr(clrieStrModuleName);
    IfFailRet(pModuleInfo->GetModuleName(&clrieStrModuleName.m_bstr));

// Skip importing modules on non-Windows platforms for now.
#ifndef PLATFORM_UNIX
    if ((m_spInjectAssembly != nullptr) && (wcscmp(clrieStrModuleName, m_spInjectAssembly->m_targetAssemblyName.c_str()) == 0))
    {
        CComPtr<IMetaDataDispenserEx> pDisp;
        ComInitializer coInit(COINIT_MULTITHREADED);
        IfFailRet(CoCreateInstance(CLSID_CorMetaDataDispenser, NULL, CLSCTX_INPROC_SERVER,
            IID_IMetaDataDispenserEx, (void **)&pDisp));

        CComPtr<IMetaDataImport2> pSourceImport;
        tstring path(m_strBinaryDir + _T("\\") + m_spInjectAssembly->m_sourceAssemblyName);
        IfFailRet(pDisp->OpenScope(path.c_str(), 0, IID_IMetaDataImport2, (IUnknown **)&pSourceImport));

        // Loads the image with section alignment, but doesn't do any of the other steps to ready an image to run
        // This is sufficient to read IL code from the image without doing RVA -> file offset mapping
        std::shared_ptr<HINSTANCE__> spSourceImage(LoadLibraryEx(path.c_str(), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE), FreeLibrary);
        if (spSourceImage == nullptr)
        {
            return HRESULT_FROM_WIN32(::GetLastError());
        }
        //Not sure if there is a better way to get the base address of a module loaded via LoadLibrary?
        //We are cheating a bit knowing that module handles are actually base addresses with a few bits OR'ed in
        LPCBYTE* pSourceImage = reinterpret_cast<LPCBYTE*>((ULONGLONG)spSourceImage.get() & (~2));

        IfFailRet(pModuleInfo->ImportModule(pSourceImport, pSourceImage));
    }
#endif

    // get the moduleId and method token for instrument method entry
    for (shared_ptr<CInstrumentMethodEntry> pInstrumentMethodEntry : m_instrumentMethodEntries)
    {
        const tstring& instrModuleName = pInstrumentMethodEntry->GetModuleName();

        if (wcscmp(clrieStrModuleName, instrModuleName.c_str()) == 0)
        {
            // search for method name inside all the types of the matching module
            const tstring& methodName = pInstrumentMethodEntry->GetMethodName();

            CComPtr<IMetaDataImport2> pMetadataImport;
            IfFailRet(pModuleInfo->GetMetaDataImport((IUnknown**)&pMetadataImport));

            BOOL bIsRejit = pInstrumentMethodEntry->GetIsRejit();

            // Only request ReJIT if the method will modify code on ReJIT. On .NET Core, the runtime will
            // only callback into the profiler for the ReJIT of the method if ReJIT is requested whereas the
            // appropriate callbacks are made for both JIT and ReJIT on .NET Framework.
            if (bIsRejit)
            {
                MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataImport2> spTypeDefEnum(pMetadataImport, nullptr);
                mdTypeDef typeDef = mdTypeDefNil;
                ULONG cTokens = 0;
                while (S_OK == (hr = pMetadataImport->EnumTypeDefs(spTypeDefEnum.Get(), &typeDef, 1, &cTokens)))
                {
                    MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataImport2> spMethodEnum(pMetadataImport, nullptr);
                    mdToken methodDefs[16];
                    ULONG cMethod = 0;
                    pMetadataImport->EnumMethodsWithName(spMethodEnum.Get(), typeDef, methodName.c_str(), methodDefs, extent<decltype(methodDefs)>::value, &cMethod);

                    if (cMethod > 0)
                    {
                        pModuleInfo->RequestRejit(methodDefs[0]);
                        break;
                    }
                }
            }
        }
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

    tstring moduleName = clrieStrModule.BStr();

    for (shared_ptr<CInstrumentMethodEntry> pInstrumentMethodEntry : m_instrumentMethodEntries)
    {
        const tstring& enteryModuleName = pInstrumentMethodEntry->GetModuleName();

        if (wcscmp(enteryModuleName.c_str(), clrieStrModule) == 0)
        {
            // TODO: Eventually, this will need to use the full name and not the partial name
            InstrStr(clrieStrMethodName);
            pMethodInfo->GetName(&clrieStrMethodName.m_bstr);

            const tstring& entryMethodName= pInstrumentMethodEntry->GetMethodName();

            if (wcscmp(entryMethodName.c_str(), clrieStrMethodName) == 0)
            {
                m_methodInfoToEntryMap[pMethodInfo] = pInstrumentMethodEntry;

                BOOL bRequireRejit = pInstrumentMethodEntry->GetIsRejit();

                *pbInstrument = (bRequireRejit == isRejit);
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

    if (pMethodEntry->IsSingleRetFirst())
    {
        PerformSingleReturnInstrumentation(pMethodInfo, pInstructionGraph);
    }

    if (pMethodEntry->GetPointTo() != nullptr)
    {
        std::shared_ptr<CInstrumentMethodPointTo> spPointTo = pMethodEntry->GetPointTo();
        BYTE pSignature[256] = {};
        DWORD cbSignature = 0;
        pMethodInfo->GetCorSignature(extent<decltype(pSignature)>::value, pSignature, &cbSignature);


        mdToken tkMethodToken = mdTokenNil;

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

        if (hrFound == CLDB_E_RECORD_NOTFOUND)
        {
            CComPtr<IMetaDataEmit> spIMetaDataEmit;
            IfFailRet(spModuleInfo->GetMetaDataEmit(
                reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));

            IfFailRet(spIMetaDataEmit->DefineTypeRefByName(tkMscorlibRef, spPointTo->m_typeName.c_str(), &tkTypeToken));
        }
        else if (FAILED(hrFound))
        {
            IfFailRet(hrFound);
        }

        hrFound = spIMetaDataImport->FindMemberRef(tkTypeToken, spPointTo->m_methodName.c_str(), pSignature, cbSignature, &tkMethodToken);

        if (hrFound == CLDB_E_RECORD_NOTFOUND)
        {
            CComPtr<IMetaDataEmit> spIMetaDataEmit;
            IfFailRet(spModuleInfo->GetMetaDataEmit(
                reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));

            IfFailRet(spIMetaDataEmit->DefineMemberRef(tkTypeToken, spPointTo->m_methodName.c_str(), pSignature, cbSignature, &tkMethodToken));
        }
        else if (FAILED(hrFound))
        {
            IfFailRet(hrFound);
        }


        // Get arguments count
        if (cbSignature < 2)
            return S_FALSE;
        USHORT argsCount = pSignature[1];

        CComPtr<IInstructionFactory> sptrInstructionFactory;
        IfFailRet(pMethodInfo->GetInstructionFactory(&sptrInstructionFactory));

        CComPtr<IInstructionGraph> sptrInstructionGraph;
        IfFailRet(pMethodInfo->GetInstructions(&sptrInstructionGraph));
        sptrInstructionGraph->RemoveAll();

        CComPtr<IInstruction> sptrCurrent;
        IfFailRet(sptrInstructionGraph->GetFirstInstruction(&sptrCurrent));

        for (USHORT i = 0; i < argsCount; i++)
        {
            CComPtr<IInstruction> sptrLoadArg;

            IfFailRet(sptrInstructionFactory->CreateLoadArgInstruction(i, &sptrLoadArg));
            IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrLoadArg));

            sptrCurrent = sptrLoadArg;
        }

        CComPtr<IInstruction> sptrCallMethod;

        IfFailRet(sptrInstructionFactory->CreateTokenOperandInstruction(Cee_Call, tkMethodToken, &sptrCallMethod));
        IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrCallMethod));
        sptrCurrent = sptrCallMethod;

        CComPtr<IInstruction> sptrReturn;

        IfFailRet(sptrInstructionFactory->CreateInstruction(Cee_Ret, &sptrReturn));
        IfFailRet(sptrInstructionGraph->InsertAfter(sptrCurrent, sptrReturn));
        sptrCurrent = sptrReturn;

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

    IfFailRet(InstrumentLocals(pMethodInfo, pMethodEntry));

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
