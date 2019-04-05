// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "InstrumentationMethod.h"
#include <sstream>
#include "ProfilerManagerHost.h"
#include "Util.h"
#include "ILDisassembler.h"

using namespace vanguard::instrumentation::managed;

static vector<wstring> globalMethodCol;

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
    CoUninitialize();

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

    CComBSTR bstrModulePath;
    IfFailRet(pModuleInfo->GetFullPath(&bstrModulePath));

    std::wstring path(bstrModulePath, SysStringLen(bstrModulePath));
    wstring pdbFile = path.substr(0, path.find_last_of(L'.')) + L".pdb";

    FILE *file;
    _wfopen_s(&file, pdbFile.c_str(), L"r");

    if (file == NULL)
    {
        return S_OK;
    }
    else
    {
        fclose(file);
    }

    if (m_hmod == NULL)
    {
        m_hmod = ::LoadLibrary(L"E:/xplat/CLRInstrumentationEngine/x64/Debug/BJSBReader.dll");
    }

    if (m_hmod != NULL)
    {
        ReadPdb pfnReadPdb = (ReadPdb)GetProcAddress(m_hmod, "ReadPdb");
        if (!pfnReadPdb)
        {
            auto error = ::GetLastError();
            FreeLibrary(m_hmod);
            return HRESULT_FROM_WIN32(error);
        }

        int methodCount = pfnReadPdb(pdbFile);

        const mdMethodDef baseToken = 0x06 << 24;
        mdMethodDef methodDef = 1;
        HRESULT methodInfoResult = S_OK;
        vector<IMethodInfo*> methodInfoCollection;

        il_disassembler disassembler(pModuleInfo);

        while (methodCount)
        {
            IMethodInfo* methodInfo = NULL;
            CComBSTR bstrMethodName;

            mdMethodDef token = baseToken | methodDef;
            methodInfoResult = pModuleInfo->GetMethodInfoByToken(token, &methodInfo);
            methodInfoCollection.push_back(methodInfo);
            methodDef++;
            methodCount--;
            methodInfo->GetFullName(&bstrMethodName);
            printf("Method Name: %S \n ", bstrMethodName);

            vanguard::instrumentation::managed::function* func = new vanguard::instrumentation::managed::function();
            func->set_token(token);

            disassembler.initialize_function(methodInfo, func);
            disassembler.disassemble_function();

            struct function_releaser
            {
                function_releaser(il_disassembler& d) :
                    _disassembler(d)
                {
                }

                ~function_releaser() { _disassembler.cleanup_function(); }

            private:
                il_disassembler& _disassembler;
            } releaser(disassembler);

            func->calculate_blocks(disassembler);

            if (func->get_block_count() > 0)
            {
                std::wstring methodNameStr(bstrMethodName, SysStringLen(bstrMethodName));
                globalMethodCol.push_back(methodNameStr);

                disassembler.instrument_function();
            }
        }

        ModuleID modId;
        pModuleInfo->GetModuleID(&modId);

        module_info* info = disassembler.get_module_info();
        info->set_block_count(disassembler.get_global_block_count());

        _instrumented_modules.insert(make_pair(modId, info));

        /*for each (IMethodInfo* methodInfo in methodInfoCollection)
        {
            IInstructionGraph* instructionsGraph;
            HRESULT hr = methodInfo->GetInstructions(&instructionsGraph);

            if (hr == S_OK)
            {
                std::vector<IInstruction*> _il_instructions;
                IInstruction* instruction = NULL;

                HRESULT instructionData = instructionsGraph->GetFirstInstruction(&instruction);
                _il_instructions.push_back(instruction);

                while (instructionData == S_OK)
                {
                    IInstruction* nextInstruction = NULL;
                    instructionData = instruction->GetNextInstruction(&nextInstruction);

                    _il_instructions.push_back(nextInstruction);
                    instruction = nextInstruction;
                }
            }
        }*/
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
    /*CComBSTR bstrMethodName;
    pMethodInfo->GetFullName(&bstrMethodName);

    std::wstring methodNameStr(bstrMethodName, SysStringLen(bstrMethodName));

    for (size_t i = 0; i < globalMethodCol.size(); i++)
    {
        if (methodNameStr.compare(globalMethodCol[i]) == 0)
        {
            *pbInstrument = TRUE;
            break;
        }
    }*/

    CComPtr<IModuleInfo> pModuleInfo;
    pMethodInfo->GetModuleInfo(&pModuleInfo);

    ModuleID modId;
    pModuleInfo->GetModuleID(&modId);

    unordered_map<ModuleID, module_info*>::iterator it = _instrumented_modules.find(modId);
    if (it != _instrumented_modules.end())
    {
        mdToken token;
        pMethodInfo->GetMethodToken(&token);
        module_info *info = it->second;
        *pbInstrument = info->contains_instrumented_method(token);
    }
    
    return S_OK;
}

HRESULT CInstrumentationMethod::InstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit)
{
   /* CComBSTR bstrMethodName;
    pMethodInfo->GetFullName(&bstrMethodName);

    std::wstring methodNameStr(bstrMethodName, SysStringLen(bstrMethodName));

    bool instrument = false;

    for (size_t i = 0; i < globalMethodCol.size(); i++)
    {
        if (methodNameStr.compare(globalMethodCol[i]) == 0)
        {
            instrument = true;
            break;
        }
    }

    if (!instrument)
    {
        return S_OK;
    }

    BYTE pSignature[256] = {};
    DWORD cbSignature = 0;
    pMethodInfo->GetCorSignature(_countof(pSignature), pSignature, &cbSignature);

    if (cbSignature < 2)
        return S_FALSE;
    USHORT argsCount = pSignature[1];*/

    CComPtr<IModuleInfo> pModuleInfo;
    pMethodInfo->GetModuleInfo(&pModuleInfo);

    ModuleID modId;
    pModuleInfo->GetModuleID(&modId);

    module_info *instrumented_module = NULL;

    unordered_map<ModuleID, module_info*>::iterator it = _instrumented_modules.find(modId);
    if (it != _instrumented_modules.end())
    {
        mdToken token;
        pMethodInfo->GetMethodToken(&token);
        instrumented_module = it->second;
    }

    mdMethodDef token;
    pMethodInfo->GetMethodToken(&token);

    method_info info;
    bool result = instrumented_module->get_method_info(token, info);

    if (!result)
    {
        return S_OK;
    }

    auto block_indices = info.get_block_indexes();
    auto start_index = info.get_start_index();
    void const *coverage_buffer = instrumented_module->get_coverage_buffer();

    auto index = reinterpret_cast<__int64>(coverage_buffer);
    index = index + info.get_start_index();

    CComPtr<IInstructionGraph> sptrInstructionGraph;
    HRESULT hr = (pMethodInfo->GetInstructions(&sptrInstructionGraph));
    IfFailRet(hr);

    std::vector<IInstruction*> il_instructions;
    CComPtr<IInstruction> il_inst;

    hr = sptrInstructionGraph->GetFirstInstruction(&il_inst);
    il_instructions.push_back(il_inst);

    while (hr == S_OK)
    {
        CComPtr<IInstruction> nextInstruction;
        hr = il_inst->GetNextInstruction(&nextInstruction);

        if (hr != S_OK)
        {
            break;
        }

        il_instructions.push_back(nextInstruction);
        il_inst = nextInstruction;
    }

    CComPtr<IInstructionFactory> sptrInstructionFactory;
    hr = (pMethodInfo->GetInstructionFactory(&sptrInstructionFactory));
    IfFailRet(hr);

    for (auto loc = block_indices.begin(); loc < block_indices.end(); ++loc)
    {
        CComPtr<IInstruction> il_inst = il_instructions[*loc];

        CComPtr<IInstruction> operandInstr;
        hr = (sptrInstructionFactory->CreateLongOperandInstruction(Cee_Ldc_I8, index, &operandInstr));
        IfFailRet(hr);
        hr = (sptrInstructionGraph->InsertBeforeAndRetargetOffsets(il_inst, operandInstr));
        IfFailRet(hr);

        CComPtr<IInstruction> pushInstr;
        hr = (sptrInstructionFactory->CreateInstruction(Cee_Ldc_I4_1, &pushInstr));
        IfFailRet(hr);
        hr = (sptrInstructionGraph->InsertBefore(il_inst, pushInstr));
        IfFailRet(hr);

        CComPtr<IInstruction> commitInstr;
        hr = (sptrInstructionFactory->CreateInstruction(Cee_Stind_I1, &commitInstr));
        IfFailRet(hr);
        hr = (sptrInstructionGraph->InsertBefore(il_inst, commitInstr));
        IfFailRet(hr);

        ++index;
    }

    
    /*CComPtr<IInstruction> sptrCurrent;
    hr = (sptrInstructionGraph->GetFirstInstruction(&sptrCurrent));
    IfFailRet(hr);

    CComPtr<IInstruction> operandInstr;
    hr = (sptrInstructionFactory->CreateLongOperandInstruction(Cee_Ldc_I8, index, &operandInstr));
    IfFailRet(hr);
    hr = (sptrInstructionGraph->InsertBeforeAndRetargetOffsets(sptrCurrent, operandInstr));
    IfFailRet(hr);

    CComPtr<IInstruction> pushInstr;
    hr = (sptrInstructionFactory->CreateInstruction(Cee_Ldc_I4_1, &pushInstr));
    IfFailRet(hr);
    hr = (sptrInstructionGraph->InsertBefore(sptrCurrent, pushInstr));
    IfFailRet(hr);

    CComPtr<IInstruction> commitInstr;
    hr = (sptrInstructionFactory->CreateInstruction(Cee_Stind_I1, &commitInstr));
    IfFailRet(hr);
    hr = (sptrInstructionGraph->InsertBefore(sptrCurrent, commitInstr));
    IfFailRet(hr);*/
    
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
