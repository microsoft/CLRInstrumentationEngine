#include "stdafx.h"
#include <unistd.h>
#include "CExtensionsHost.h"

using namespace vanguard::instrumentation::managed;

static vector<string> globalMethodCol;

HRESULT ExtensionsHostCrossPlat::CExtensionHost::Initialize(
    _In_ IProfilerManager* pProfilerManager
)
{
    usleep(5*1000);
    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);
    pCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_ENABLE_STACK_SNAPSHOT);
    return S_OK;
}

HRESULT ExtensionsHostCrossPlat::CExtensionHost::InstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit)
{
    /*CComBSTR bstrMethodName;
    pMethodInfo->GetFullName(&bstrMethodName);

    tstringstream methodName;
    methodName << (LPWSTR)bstrMethodName;

    tstring methodNameWStr = methodName.str();
    string methodNameStr(methodNameWStr.begin(), methodNameWStr.end());

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

    if (result)
    {
        il_disassembler disassembler(pModuleInfo);
        auto block_indices = info.get_block_indexes();
        auto start_index = info.get_start_index();
        void const *coverage_buffer = instrumented_module->get_coverage_buffer();
    }

    CComPtr<IInstructionFactory> sptrInstructionFactory;
    HRESULT hr = (pMethodInfo->GetInstructionFactory(&sptrInstructionFactory));
    IfFailRet(hr);

    CComPtr<IInstructionGraph> sptrInstructionGraph;
    hr = (pMethodInfo->GetInstructions(&sptrInstructionGraph));
    IfFailRet(hr);

    CComPtr<IInstruction> sptrCurrent;
    hr = (sptrInstructionGraph->GetFirstInstruction(&sptrCurrent));
    IfFailRet(hr);

    vector<__int64> vec(1);
    auto data = reinterpret_cast<__int64>(std::addressof(vec[0]));

    CComPtr<IInstruction> operandInstr;
    hr = (sptrInstructionFactory->CreateLongOperandInstruction(Cee_Ldc_I8, data, &operandInstr));
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
    IfFailRet(hr);

    return hr;
}

HRESULT ExtensionsHostCrossPlat::CExtensionHost::OnShutdown()
{
    return S_OK;
}

HRESULT ExtensionsHostCrossPlat::CExtensionHost::ShouldInstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit, _Out_ BOOL* pbInstrument)
{
    /*CComBSTR bstrMethodName;
    pMethodInfo->GetFullName(&bstrMethodName);

    tstringstream methodName;
    methodName << (LPWSTR)bstrMethodName;

    tstring methodNameWStr = methodName.str();
    string methodNameStr(methodNameWStr.begin(), methodNameWStr.end());


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

HRESULT ExtensionsHostCrossPlat::CExtensionHost::OnModuleLoaded(IModuleInfo* pModuleInfo)
{
    HRESULT hr = S_OK;

    CComBSTR bstrModuleName;
    IfFailRet(pModuleInfo->GetModuleName(&bstrModuleName));

    CComBSTR bstrModulePath;
    IfFailRet(pModuleInfo->GetFullPath(&bstrModulePath));

    tstringstream dllPath;
    dllPath << (LPWSTR)bstrModulePath;

    tstring pdbPath = dllPath.str();
    pdbPath = pdbPath.substr(0, pdbPath.find_last_of(_T('.'))) + _T(".pdb");

    string filePathChar(pdbPath.begin(), pdbPath.end());

    tstringstream pathBuilder;
    pathBuilder <<_T("/home/maban/projects/XPlatPdbReader/bin/x64/Debug/libXPlatPdbReader.so");

    if (m_hmod == NULL)
    {
        m_hmod = ::LoadLibrary(pathBuilder.str().c_str());
    }

    if (m_hmod == NULL)
    {
        auto error = ::GetLastError();
        // Failed to load the module.
        return HRESULT_FROM_WIN32(error);
    }

    typedef int(__stdcall* ReadPDB)(const char* path);

    ReadPDB pfnReadPdb = (ReadPDB)GetProcAddress(m_hmod, "ReadPDB");
    if (!pfnReadPdb)
    {
        auto error = ::GetLastError();
        FreeLibrary(m_hmod);
        return HRESULT_FROM_WIN32(error);
    }

    int methodCount = 0;
    methodCount = pfnReadPdb(filePathChar.c_str());

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

        //printf("Method Name: %ls \n ", bstrMethodName);
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
            tstringstream methodName;
            methodName << (LPWSTR)bstrMethodName;

            tstring methodNameWStr = methodName.str();
            string methodNameStr(methodNameWStr.begin(), methodNameWStr.end());
            globalMethodCol.push_back(methodNameStr);
            disassembler.instrument_function();
        }
    }

    ModuleID modId;
    pModuleInfo->GetModuleID(&modId);

    module_info* info = disassembler.get_module_info();
    info->set_block_count(disassembler.get_global_block_count());

    _instrumented_modules.insert(make_pair(modId, info));

    return hr;
}
