// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "InstrumentationMethod.h"
#include <sstream>
#include "ProfilerManagerHost.h"
#include "Util.h"

const WCHAR CInstrumentationMethod::TestOutputPathEnvName[] = L"Nagler_TestOutputPath";
const WCHAR CInstrumentationMethod::TestScriptFileEnvName[] = L"Nagler_TestScript";
const WCHAR CInstrumentationMethod::TestScriptFolder[] = L"TestScripts";
const WCHAR CInstrumentationMethod::IsRejitEnvName[] = L"Nagler_IsRejit";

HRESULT CInstrumentationMethod::Initialize(_In_ IProfilerManager* pProfilerManager)
{
    m_pProfilerManager = pProfilerManager;

    CHandle hConfigThread;
    hConfigThread.Attach(CreateThread(NULL, 0, InstrumentationMethodThreadProc, this, 0, NULL));

    DWORD retVal = WaitForSingleObject(hConfigThread, 15000);

    if (m_bExceptionTrackingEnabled)
    {
        CComPtr<ICorProfilerInfo> pCorProfilerInfo;
        pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);
        pCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_ENABLE_STACK_SNAPSHOT);

        CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();
        pProfilerManagerHost->LogDumpMessage(_T("<ExceptionTrace>\r\n"));
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
    CoInitialize(NULL);

    CInstrumentationMethod* pThis = (CInstrumentationMethod*)lpParameter;
    HRESULT hr = pThis->LoadTestScript();

    CoUninitialize();

    if (FAILED(hr))
    {
        return (DWORD)-1;
    }
    else
    {
        return 0;
    }
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
	CComPtr<IXMLDOMDocument3> pDocument;
    IfFailRet(CoCreateInstance(CLSID_FreeThreadedDOMDocument60, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument3, (void**)&pDocument));

    VARIANT_BOOL vbResult = VARIANT_TRUE;
    hr = pDocument->load(CComVariant(testScript), &vbResult);
    if (FAILED(hr) || vbResult != VARIANT_TRUE)
    {
        ATLASSERT(!L"Failed to load test configuration");
        return -1;
    }

    CComPtr<IXMLDOMElement> pDocumentNode;
    IfFailRet(pDocument->get_documentElement(&pDocumentNode));

    CComBSTR bstrDocumentNodeName;
    IfFailRet(pDocumentNode->get_nodeName(&bstrDocumentNodeName));

    if (wcscmp(bstrDocumentNodeName, L"InstrumentationTestScript") != 0)
    {
        return -1;
    }

    CComPtr<IXMLDOMNodeList> pChildNodes;
    IfFailRet(pDocumentNode->get_childNodes(&pChildNodes));

    long cChildren = 0;
    pChildNodes->get_length(&cChildren);

    for (long i = 0; i < cChildren; i++)
    {
        CComPtr<IXMLDOMNode> pCurrChildNode;
        IfFailRet(pChildNodes->get_item(i, &pCurrChildNode));

        CComBSTR bstrCurrNodeName;
        IfFailRet(pCurrChildNode->get_nodeName(&bstrCurrNodeName));

        if (wcscmp(bstrCurrNodeName, L"InstrumentMethod") == 0)
        {
            IfFailRet(ProcessInstrumentMethodNode(pCurrChildNode));
        }
        else if (wcscmp(bstrCurrNodeName, L"ExceptionTracking") == 0)
        {
            m_bExceptionTrackingEnabled = true;
        }
        else if (wcscmp(bstrCurrNodeName, L"InjectAssembly") == 0)
        {
            shared_ptr<CInjectAssembly> spNewInjectAssembly(new CInjectAssembly());
            ProcessInjectAssembly(pCurrChildNode, spNewInjectAssembly);
            m_spInjectAssembly = spNewInjectAssembly;
        }
        else
        {
            ATLASSERT(!L"Invalid configuration. Element should be InstrumentationMethod");
            return -1;
        }
    }

    return 0;
}

HRESULT CInstrumentationMethod::ProcessInstrumentMethodNode(IXMLDOMNode* pNode)
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

    CComPtr<IXMLDOMNodeList> pChildNodes;
    IfFailRet(pNode->get_childNodes(&pChildNodes));

    long cChildren = 0;
    pChildNodes->get_length(&cChildren);

    CComBSTR bstrModuleName;
    CComBSTR bstrMethodName;
    BOOL bIsRejit = rejitAllInstruMethods;
    vector<shared_ptr<CInstrumentInstruction>> instructions;
    vector<CLocalType> locals;
    vector<COR_IL_MAP> corIlMap;
    BOOL isReplacement = FALSE;
    shared_ptr<CInstrumentMethodPointTo> spPointTo(nullptr);

    for (long i = 0; i < cChildren; i++)
    {
        CComPtr<IXMLDOMNode> pChildNode;
        IfFailRet(pChildNodes->get_item(i, &pChildNode));

        CComBSTR bstrCurrNodeName;
        IfFailRet(pChildNode->get_nodeName(&bstrCurrNodeName));

        if (wcscmp(bstrCurrNodeName, L"ModuleName") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);
            bstrModuleName = varNodeValue.bstrVal;
        }
        else if (wcscmp(bstrCurrNodeName, L"MethodName") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);
            bstrMethodName = varNodeValue.bstrVal;
        }
        else if (wcscmp(bstrCurrNodeName, L"Instructions") == 0)
        {
            CComPtr<IXMLDOMNode> pAttrValue;
            CComPtr<IXMLDOMNamedNodeMap> attributes;
            pChildNode->get_attributes(&attributes);
            CComPtr<IXMLDOMNode> pBaseLine;
            attributes->getNamedItem(L"Baseline", &pBaseLine);

            isReplacement = pBaseLine != nullptr;

            ProcessInstructionNodes(pChildNode, instructions, isReplacement != 0);
        }
        else if (wcscmp(bstrCurrNodeName, L"IsRejit") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);
            bIsRejit = (wcscmp(varNodeValue.bstrVal, L"True") == 0);
        }
        else if (wcscmp(bstrCurrNodeName, L"CorIlMap") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);

            wstringstream corIlMapString(varNodeValue.bstrVal);
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
        else if (wcscmp(bstrCurrNodeName, L"Locals") == 0)
        {
            ProcessLocals(pChildNode, locals);
        }
        else if (wcscmp(bstrCurrNodeName, L"PointTo") == 0)
        {
            std::shared_ptr<CInstrumentMethodPointTo> spPointer(new CInstrumentMethodPointTo());
            spPointTo = spPointer;
            ProcessPointTo(pChildNode, *spPointTo);
        }
        else
        {
            ATLASSERT(!L"Invalid configuration. Unknown Element");
            return E_FAIL;
        }
    }

    if ((bstrModuleName.Length() == 0) ||
        (bstrMethodName.Length() == 0))
    {
        ATLASSERT(!L"Invalid configuration. Missing child element");
        return E_FAIL;
    }

    shared_ptr<CInstrumentMethodEntry> pMethod = make_shared<CInstrumentMethodEntry>(bstrModuleName, bstrMethodName, bIsRejit);
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

HRESULT CInstrumentationMethod::ProcessPointTo(IXMLDOMNode* pNode, CInstrumentMethodPointTo& pointTo)
{
    HRESULT hr = S_OK;

    CComPtr<IXMLDOMNodeList> pChildNodes;
    IfFailRet(pNode->get_childNodes(&pChildNodes));
    long count = 0;
    IfFailRet(pChildNodes->get_length(&count));

    for (long i = 0; i < count; i++)
    {
        CComPtr<IXMLDOMNode> pChildNode;
        IfFailRet(pChildNodes->get_item(i, &pChildNode));

        //<AssemblyName>InjectToMscorlibTest_Release</AssemblyName>
        //<TypeName>InstruOperationsTests.Program</TypeName>
        //<MethodName>MethodToCallInstead</MethodName>
        CComBSTR nodeName;
        IfFailRet(pChildNode->get_nodeName(&nodeName));
        if (wcscmp(nodeName, L"AssemblyName") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);
            CComBSTR bstrVal = varNodeValue.bstrVal;
            pointTo.m_assemblyName = bstrVal;
        }
        else if (wcscmp(nodeName, L"TypeName") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);
            CComBSTR bstrVal = varNodeValue.bstrVal;
            pointTo.m_typeName = bstrVal;
        }
        else if (wcscmp(nodeName, L"MethodName") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);
            CComBSTR bstrVal = varNodeValue.bstrVal;
            pointTo.m_methodName = bstrVal;
        }

    }
    return hr;
}

HRESULT CInstrumentationMethod::ProcessInjectAssembly(IXMLDOMNode* pNode, shared_ptr<CInjectAssembly>& injectAssembly)
{
    HRESULT hr = S_OK;

    CComPtr<IXMLDOMNodeList> pChildNodes;
    IfFailRet(pNode->get_childNodes(&pChildNodes));
    long count = 0;
    IfFailRet(pChildNodes->get_length(&count));

    for (long i = 0; i < count; i++)
    {
        CComPtr<IXMLDOMNode> pChildNode;
        IfFailRet(pChildNodes->get_item(i, &pChildNode));

        //<Source>C:\tfs\pioneer2\VSClient1\src\edev\diagnostics\intellitrace\InstrumentationEngine\Tests\InstrEngineTests\Binary\InjectToMscorlibTest_Debug.exe</Source>
        //<Target>mscorlib</Target>
        CComBSTR nodeName;
        IfFailRet(pChildNode->get_nodeName(&nodeName));
        if (wcscmp(nodeName, L"Target") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);
            CComBSTR bstrVal = varNodeValue.bstrVal;
            injectAssembly->m_targetAssemblyName = bstrVal;
        }
        else if (wcscmp(nodeName, L"Source") == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            CComVariant varNodeValue;
            pChildValue->get_nodeValue(&varNodeValue);
            CComBSTR bstrVal = varNodeValue.bstrVal;
            injectAssembly->m_sourceAssemblyName = bstrVal;
        }
    }
    return hr;
}

HRESULT CInstrumentationMethod::ProcessLocals(IXMLDOMNode* pNode, vector<CLocalType>& locals)
{
    HRESULT hr = S_OK;

    CComPtr<IXMLDOMNodeList> pChildNodes;
    IfFailRet(pNode->get_childNodes(&pChildNodes));
    long count = 0;
    IfFailRet(pChildNodes->get_length(&count));

    for (long i = 0; i < count; i++)
    {
        CComPtr<IXMLDOMNode> pChildNode;
        IfFailRet(pChildNodes->get_item(i, &pChildNode));

        CComBSTR nodeName;
        IfFailRet(pChildNode->get_nodeName(&nodeName));
        if (wcscmp(nodeName, L"Local") != 0)
        {
            ATLASSERT(!L"Invalid element");
            return E_UNEXPECTED;
        }

        CComPtr<IXMLDOMNamedNodeMap> attributes;
        IfFailRet(pChildNode->get_attributes(&attributes));
        CComPtr<IXMLDOMNode> pTypeName;
        IfFailRet(attributes->getNamedItem(L"Type", &pTypeName));

        CComVariant varTypeName;
        pTypeName->get_nodeValue(&varTypeName);
        CComBSTR bstrTypeName = varTypeName.bstrVal;

        locals.push_back(CLocalType((LPWSTR)bstrTypeName));
    }
    return hr;
}

HRESULT CInstrumentationMethod::ProcessInstructionNodes(IXMLDOMNode* pNode, vector<shared_ptr<CInstrumentInstruction>>& instructions, bool isBaseline)
{
    HRESULT hr = S_OK;

    CComPtr<IXMLDOMNodeList> pChildNodes;
    IfFailRet(pNode->get_childNodes(&pChildNodes));

    long cChildren = 0;
    pChildNodes->get_length(&cChildren);

    for (long i = 0; i < cChildren; i++)
    {
        CComPtr<IXMLDOMNode> pChildNode;
        IfFailRet(pChildNodes->get_item(i, &pChildNode));

        CComBSTR bstrCurrNodeName;
        IfFailRet(pChildNode->get_nodeName(&bstrCurrNodeName));

        if (wcscmp(bstrCurrNodeName, L"Instruction") == 0)
        {
            CComPtr<IXMLDOMNodeList> pInstructionChildNodes;
            IfFailRet(pChildNode->get_childNodes(&pInstructionChildNodes));

            long cInstructionChildren = 0;
            pInstructionChildNodes->get_length(&cInstructionChildren);

            bool bOpcodeSet = false;
            bool bOffsetSet = false;
            DWORD dwOffset = 0;
            ILOrdinalOpcode opcode;
            ILOpcodeInfo opcodeInfo;
            InstrumentationType instrType = InsertBefore;
            DWORD dwRepeatCount = 1;

            // NOTE: only support integer operands for the time being.
            INT64 operand = 0;
            for (long j = 0; j < cInstructionChildren; j++)
            {
                CComPtr<IXMLDOMNode> pInstructionChildNode;
                IfFailRet(pInstructionChildNodes->get_item(j, &pInstructionChildNode));

                CComBSTR bstrCurrInstructionNodeName;
                IfFailRet(pInstructionChildNode->get_nodeName(&bstrCurrInstructionNodeName));

                if (wcscmp(bstrCurrInstructionNodeName, L"Opcode") == 0)
                {
                    CComPtr<IXMLDOMNode> pChildValue;
                    pInstructionChildNode->get_firstChild(&pChildValue);

                    CComVariant varNodeValue;
                    pChildValue->get_nodeValue(&varNodeValue);
                    CComBSTR bstrOpcode = varNodeValue.bstrVal;

                    IfFailRet(ConvertOpcode(bstrOpcode, &opcode, &opcodeInfo));
                    bOpcodeSet = true;
                }
                else if (wcscmp(bstrCurrInstructionNodeName, L"Offset") == 0)
                {
                    CComPtr<IXMLDOMNode> pChildValue;
                    pInstructionChildNode->get_firstChild(&pChildValue);

                    CComVariant varNodeValue;
                    pChildValue->get_nodeValue(&varNodeValue);
                    CComBSTR bstrOffset = varNodeValue.bstrVal;

                    dwOffset = _wtoi(bstrOffset);
                    bOffsetSet = true;
                }
                else if (wcscmp(bstrCurrInstructionNodeName, L"Operand") == 0)
                {
                    if (opcodeInfo.m_operandLength == 0)
                    {
                        ATLASSERT(!L"Invalid configuration. Opcode should not have an operand");
                    }
                    CComPtr<IXMLDOMNode> pChildValue;
                    pInstructionChildNode->get_firstChild(&pChildValue);

                    CComVariant varNodeValue;
                    pChildValue->get_nodeValue(&varNodeValue);
                    CComBSTR bstrOperand = varNodeValue.bstrVal;

                    operand = _wtoi64(bstrOperand);

                    // For now, only support integer operands
                    ATLASSERT(opcodeInfo.m_type == ILOperandType_None ||
                        opcodeInfo.m_type == ILOperandType_Byte ||
                        opcodeInfo.m_type == ILOperandType_Int ||
                        opcodeInfo.m_type == ILOperandType_UShort ||
                        opcodeInfo.m_type == ILOperandType_Long ||
                        opcodeInfo.m_type == ILOperandType_Token
                        );


                }
                else if (wcscmp(bstrCurrInstructionNodeName, L"InstrumentationType") == 0)
                {
                    CComPtr<IXMLDOMNode> pChildValue;
                    pInstructionChildNode->get_firstChild(&pChildValue);

                    CComVariant varNodeValue;
                    pChildValue->get_nodeValue(&varNodeValue);
                    CComBSTR bstrInstrType = varNodeValue.bstrVal;

                    if (wcscmp(bstrInstrType, L"InsertBefore") == 0)
                    {
                        instrType = InsertBefore;
                    }
                    else if (wcscmp(bstrInstrType, L"InsertAfter") == 0)
                    {
                        instrType = InsertAfter;
                    }
                    else if (wcscmp(bstrInstrType, L"Replace") == 0)
                    {
                        instrType = Replace;
                    }
                    else if (wcscmp(bstrInstrType, L"Remove") == 0)
                    {
                        instrType = Remove;
                    }
                    else if (wcscmp(bstrInstrType, L"RemoveAll") == 0)
                    {
                        instrType = RemoveAll;
                    }
                }
                else if (wcscmp(bstrCurrInstructionNodeName, L"RepeatCount") == 0)
                {
                    CComPtr<IXMLDOMNode> pChildValue;
                    pInstructionChildNode->get_firstChild(&pChildValue);

                    CComVariant varNodeValue;
                    pChildValue->get_nodeValue(&varNodeValue);
                    CComBSTR bstrRepeatCount = varNodeValue.bstrVal;

                    dwRepeatCount = _wtoi(bstrRepeatCount);
                }
                else
                {
                    ATLASSERT(!L"Invalid configuration. Unknown Element");
                    return E_FAIL;
                }
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
    }
    return S_OK;
}

HRESULT CInstrumentationMethod::ConvertOpcode(BSTR bstrOpcode, ILOrdinalOpcode* pOpcode, ILOpcodeInfo* pOpcodeInfo)
{
    // Horribly slow solution to mapping opcode name to opcode.
    // This should be okay though since the tests should only have a few instructions

    const DWORD cOpcodes = 0x0124;
    for (DWORD i = 0; i < 0x0124; i++)
    {
        if (wcscmp(ilOpcodeInfo[i].m_name, bstrOpcode) == 0)
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

    CComBSTR bstrModuleName;
    IfFailRet(pModuleInfo->GetModuleName(&bstrModuleName));

    if ((m_spInjectAssembly != nullptr) && (wcscmp(bstrModuleName, m_spInjectAssembly->m_targetAssemblyName.c_str()) == 0))
    {
        CComPtr<IMetaDataDispenserEx> pDisp;
        CoInitializeEx(0, COINIT_MULTITHREADED);
        IfFailRet(CoCreateInstance(CLSID_CorMetaDataDispenser, NULL, CLSCTX_INPROC_SERVER,
            IID_IMetaDataDispenserEx, (void **)&pDisp));

        CComPtr<IMetaDataImport2> pSourceImport;
        std::wstring path(m_strBinaryDir + L"\\" + m_spInjectAssembly->m_sourceAssemblyName);
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


    // get the moduleId and method token for instrument method entry
    for (shared_ptr<CInstrumentMethodEntry> pInstrumentMethodEntry : m_instrumentMethodEntries)
    {
        CComBSTR bstrInstruModuleName;
        IfFailRet(pInstrumentMethodEntry->GetModuleName(&bstrInstruModuleName));

        if (wcscmp(bstrModuleName, bstrInstruModuleName) == 0)
        {
            // search for method name inside all the types of the matching module
            CComBSTR bstrMethodName;
            IfFailRet(pInstrumentMethodEntry->GetMethodName(&bstrMethodName));

            CComPtr<IMetaDataImport2> pMetadataImport;
            IfFailRet(pModuleInfo->GetMetaDataImport((IUnknown**)&pMetadataImport));


			MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataImport2> spTypeDefEnum(pMetadataImport, nullptr);
			mdTypeDef typeDef = mdTypeDefNil;
			ULONG cTokens = 0;
			while (S_OK == (hr = pMetadataImport->EnumTypeDefs(spTypeDefEnum.Get(), &typeDef, 1, &cTokens)))
			{
				MicrosoftInstrumentationEngine::CMetadataEnumCloser<IMetaDataImport2> spMethodEnum(pMetadataImport, nullptr);
				mdToken methodDefs[16];
				ULONG cMethod = 0;
				pMetadataImport->EnumMethodsWithName(spMethodEnum.Get(), typeDef, bstrMethodName, methodDefs, _countof(methodDefs), &cMethod);

				if (cMethod > 0)
				{
					pModuleInfo->RequestRejit(methodDefs[0]);
					break;
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
    if (m_bExceptionTrackingEnabled)
    {
        CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();
        pProfilerManagerHost->LogDumpMessage(_T("</ExceptionTrace>\r\n"));
    }
    return S_OK;
}

HRESULT CInstrumentationMethod::ShouldInstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit, _Out_ BOOL* pbInstrument)
{
    *pbInstrument = FALSE;

    CComPtr<IModuleInfo> pModuleInfo;
    pMethodInfo->GetModuleInfo(&pModuleInfo);

    CComBSTR bstrModule;
    pModuleInfo->GetModuleName(&bstrModule);

    wstring moduleName = bstrModule;

    for (shared_ptr<CInstrumentMethodEntry> pInstrumentMethodEntry : m_instrumentMethodEntries)
    {
        CComBSTR bstrEntryModuleName;
        pInstrumentMethodEntry->GetModuleName(&bstrEntryModuleName);

        if (wcscmp(bstrEntryModuleName, bstrModule) == 0)
        {
            // TODO: Eventually, this will need to use the full name and not the partial name
            CComBSTR bstrMethodName;
            pMethodInfo->GetName(&bstrMethodName);

            CComBSTR bstrEntryMethodName;
            pInstrumentMethodEntry->GetMethodName(&bstrEntryMethodName);

            if (wcscmp(bstrEntryMethodName, bstrMethodName) == 0)
            {
                m_methodInfoToEntryMap[pMethodInfo] = pInstrumentMethodEntry;

                BOOL bRequireRejit = FALSE;
                pInstrumentMethodEntry->GetIsRejit(&bRequireRejit);

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
        ATLASSERT(!L"CInstrumentationMethod::InstrumentMethod - why no method entry? It should have been set in ShouldInstrumentMethod");
        return E_FAIL;
    }

    BOOL bRequireRejit = FALSE;
    IfFailRet(pMethodEntry->GetIsRejit(&bRequireRejit));
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
        for (DWORD i = 0; i < baselineSequencePoints.size(); i++)
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

    BOOL bRequireRejit = FALSE;
    IfFailRet(pMethodEntry->GetIsRejit(&bRequireRejit));
    if (bRequireRejit != isRejit)
    {
        return E_FAIL;
    }

    vector<shared_ptr<CInstrumentInstruction>> instructions = pMethodEntry->GetInstructions();

    CComPtr<IInstructionGraph> pInstructionGraph;
    IfFailRet(pMethodInfo->GetInstructions(&pInstructionGraph));

    if (pMethodEntry->GetPointTo() != nullptr)
    {
        std::shared_ptr<CInstrumentMethodPointTo> spPointTo = pMethodEntry->GetPointTo();
        BYTE pSignature[256] = {};
        DWORD cbSignature = 0;
        pMethodInfo->GetCorSignature(_countof(pSignature), pSignature, &cbSignature);


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
            LPCVOID		pbPublicKey = nullptr;
            ULONG		cbPublicKey = 0;
            LPCVOID		pbHashValue = nullptr;
            ULONG		cbHashValue = 0;
            ULONG		chName = 0;

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
                        ATLASSERT(!L"Switch not implemented yet in nagler");
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

    IfFailRet(InstrumentLocals(pMethodInfo, pMethodEntry));

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
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    CComBSTR bstrFullName;
    pMethodInfo->GetFullName(&bstrFullName);

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionCatcherEnter>"));
        pProfilerManagerHost->LogDumpMessage(bstrFullName);
    pProfilerManagerHost->LogDumpMessage(_T("</ExceptionCatcherEnter>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionCatcherLeave()
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionCatcherLeave/>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchCatcherFound(
    _In_ IMethodInfo* pMethodInfo
    )
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    CComBSTR bstrFullName;
    pMethodInfo->GetFullName(&bstrFullName);

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionSearchCatcherFound>"));
    pProfilerManagerHost->LogDumpMessage(bstrFullName);
    pProfilerManagerHost->LogDumpMessage(_T("</ExceptionSearchCatcherFound>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchFilterEnter(
    _In_ IMethodInfo* pMethodInfo
    )
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    CComBSTR bstrFullName;
    pMethodInfo->GetFullName(&bstrFullName);

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionSearchFilterEnter>"));
    pProfilerManagerHost->LogDumpMessage(bstrFullName);
    pProfilerManagerHost->LogDumpMessage(_T("</ExceptionSearchFilterEnter>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchFilterLeave()
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionCatcherLeave/>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchFunctionEnter(
    _In_ IMethodInfo* pMethodInfo
    )
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    CComBSTR bstrFullName;
    pMethodInfo->GetFullName(&bstrFullName);

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionSearchFunctionEnter>"));
        pProfilerManagerHost->LogDumpMessage(bstrFullName);
    pProfilerManagerHost->LogDumpMessage(_T("</ExceptionSearchFunctionEnter>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionSearchFunctionLeave()
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionSearchFunctionLeave/>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionThrown(
    _In_ UINT_PTR thrownObjectId
    )
{
    HRESULT hr = S_OK;

    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionThrown>\r\n"));

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


    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("</ExceptionThrown>\r\n"));

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

    CComBSTR bstrMethodName;
    IfFailRet(pMethodInfo->GetName(&bstrMethodName));

    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    pProfilerManagerHost->LogDumpMessage(_T("        "));
    pProfilerManagerHost->LogDumpMessage(_T("<MethodName>"));
        pProfilerManagerHost->LogDumpMessage(bstrMethodName);
    pProfilerManagerHost->LogDumpMessage(_T("</MethodName>\r\n"));


    // verify other code paths that don't contribute to actual baseline
    // This is just to get coverage on althernative code paths to obtain method infos
    CComPtr<IModuleInfo> pModuleInfo;
    IfFailRet(pMethodInfo->GetModuleInfo(&pModuleInfo));
    mdToken methodToken;
    IfFailRet(pMethodInfo->GetMethodToken(&methodToken));

    CComPtr<IMethodInfo> pMethodInfo2;
    IfFailRet(pModuleInfo->GetMethodInfoByToken(methodToken, &pMethodInfo2));

    CComBSTR bstrMethodName2;
    IfFailRet(pMethodInfo2->GetName(&bstrMethodName2));

    if (bstrMethodName != bstrMethodName2)
    {
        pProfilerManagerHost->LogDumpMessage(_T("pModuleInfo->GetMethodInfoByToken did not return same method as pAppDomainCollection->GetMethodInfoById"));
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
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    CComBSTR bstrFullName;
    pMethodInfo->GetFullName(&bstrFullName);

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionSearchFunctionEnter>"));
    pProfilerManagerHost->LogDumpMessage(bstrFullName);
    pProfilerManagerHost->LogDumpMessage(_T("</ExceptionSearchFunctionEnter>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionUnwindFinallyLeave()
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionUnwindFinallyLeave/>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionUnwindFunctionEnter(
    _In_ IMethodInfo* pMethodInfo
    )
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    CComBSTR bstrFullName;
    pMethodInfo->GetFullName(&bstrFullName);

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionUnwindFunctionEnter>"));
    pProfilerManagerHost->LogDumpMessage(bstrFullName);
    pProfilerManagerHost->LogDumpMessage(_T("</ExceptionUnwindFunctionEnter>\r\n"));

    return S_OK;
}

HRESULT CInstrumentationMethod::ExceptionUnwindFunctionLeave()
{
    CProfilerManagerHost* pProfilerManagerHost = CProfilerManagerHost::GetInstance();

    pProfilerManagerHost->LogDumpMessage(_T("    "));
    pProfilerManagerHost->LogDumpMessage(_T("<ExceptionUnwindFunctionLeave/>\r\n"));

    return S_OK;
}
