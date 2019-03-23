#include "stdafx.h"
#include <unistd.h>
#include "CExtensionsHost.h"

using namespace vanguard::instrumentation::managed;

vector<string> globalMethodCol;

HRESULT ExtensionsHostCrossPlat::CExtensionHost::Initialize(
    _In_ IProfilerManager* pProfilerManager
)
{
    printf("Loaded Custom Profiler");
    usleep(5*1000);
    CComPtr<ICorProfilerInfo> pCorProfilerInfo;
    pProfilerManager->GetCorProfilerInfo((IUnknown**)&pCorProfilerInfo);
    pCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_EXCEPTIONS | COR_PRF_ENABLE_STACK_SNAPSHOT);
    return S_OK;
}

HRESULT ExtensionsHostCrossPlat::InstrumentMethod(_In_ IMethodInfo* pMethodInfo, _In_ BOOL isRejit)
{
    CComBSTR bstrMethodName;
    pMethodInfo->GetFullName(&bstrMethodName);

    tstringstream methodName;
    methodName << (LPWSTR)bstrMethodName;

    tstring methodNameWStr = methodName.str();
    string methodNameStr(methodNameWStr.begin(), methodNameWStr.end());

    bool instrument = false;

    for (size_t a = 0; a < globalMethodCol.size(); a++)
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
    USHORT argsCount = pSignature[1];

    CComPtr<IInstructionFactory> sptrInstructionFactory;
    HRESULT hr = (pMethodInfo->GetInstructionFactory(&sptrInstructionFactory));
    IfFailRet(hr);

    CComPtr<IInstructionGraph> sptrInstructionGraph;
    hr = (pMethodInfo->GetInstructions(&sptrInstructionGraph));
    IfFailRet(hr);

    sptrInstructionGraph->RemoveAll();

    CComPtr<IInstruction> sptrCurrent;
    hr = (sptrInstructionGraph->GetFirstInstruction(&sptrCurrent));
    IfFailRet(hr);

    for (USHORT i = 0; i < argsCount; i++)
    {
        CComPtr<IInstruction> sptrLoadArg;

        hr = (sptrInstructionFactory->CreateLoadArgInstruction(i, &sptrLoadArg));
        IfFailRet(hr);

        hr = (sptrInstructionGraph->InsertAfter(sptrCurrent, sptrLoadArg));
        IfFailRet(hr);

        sptrCurrent = sptrLoadArg;
    }

    CComPtr<IInstruction> sptrReturn;

    hr = (sptrInstructionFactory->CreateInstruction(Cee_Ret, &sptrReturn));
    IfFailRet(hr);

    hr = (sptrInstructionGraph->InsertAfter(sptrCurrent, sptrReturn));
    IfFailRet(hr);

    sptrCurrent = sptrReturn;
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

        methodInfoResult = pModuleInfo->GetMethodInfoByToken(baseToken | methodDef, &methodInfo);
        methodInfoCollection.push_back(methodInfo);
        methodDef++;
        methodCount--;
        methodInfo->GetFullName(&bstrMethodName);

        //printf("Method Name: %ls \n ", bstrMethodName);
        vanguard::instrumentation::managed::function* func = new vanguard::instrumentation::managed::function();
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
            globalMethodCol.push_back(methodNameWStr);
        }

        disassembler.instrument_function();
    }

    return hr;
}
