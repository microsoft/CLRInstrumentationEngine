// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ManagedRedirectCodeInjector.h"
#include "WellKnownInstrumentationDefs.h"

CManagedRedirectCodeInjector::CManagedRedirectCodeInjector()
    : m_namesMapping({
            { L"Add", L"ApplicationInsights_AddCallbacks" },
            { L"Remove", L"ApplicationInsights_RemoveCallbacks" }
        })
{
    Create(m_spReflectionHelper);
}

HRESULT CManagedRedirectCodeInjector::BuildUpMembers()
{
    IfFailRet(BuildUpObjPtr(m_spReflectionHelper));
    return S_OK;
}

HRESULT CManagedRedirectCodeInjector::EmitModule(
    _In_ const IModuleInfoSptr&)
{
    HRESULT hr = S_OK;

    //auto itEnd = m_redirects.end();
    //for (auto it = m_redirects.begin(); it != m_redirects.end(); ++it)
    //{
    //    IfFailRet(hr = m_spReflectionHelper->IsTypeBelongsToModule(spModuleInfo, it->first->GetType()));
    //    if (hr == S_OK)
    //    {
    //        mdToken mdMethodRef;
    //        m_spReflectionHelper->DefineMethodToken(spModuleInfo, it->second, mdMethodRef);
    //    }
    //}

    return hr;
}

HRESULT CManagedRedirectCodeInjector::Inject(
    _In_ const IMethodInfoSptr& spMethodInfo,
    _In_ const CMethodRecordSptr& spMethodRecord)
{
    HRESULT hr = S_OK;

    BYTE argsCount = 0;

    // Get signature
    BYTE pSignature[256] = {}; // magical number.
    DWORD cbSignature = 0;
    IfFailRet(spMethodInfo->GetCorSignature(_countof(pSignature), pSignature, &cbSignature));

    // Get arguments count
    IfFalseRet(cbSignature > 2, S_FALSE);
    argsCount = pSignature[1];

    // Get signature token
    IModuleInfoSptr spModuleInfo;
    IfFailRet(spMethodInfo->GetModuleInfo(&spModuleInfo));

    ATL::CComBSTR bstrMethodName;
    IfFailRet(spMethodInfo->GetName(&bstrMethodName));
    std::wstring methodName((LPWSTR)bstrMethodName);

    std::shared_ptr<Agent::Reflection::CWellKnownMethodInfo> spMethodToRedirect(
        new Agent::Reflection::CWellKnownMethodInfo(ExtensionsBaseAssemblyName, ExtensionsBaseModuleName, ExtensionsBasePublicContractType, m_namesMapping.find(methodName)->second, pSignature, cbSignature));
    mdToken tkMethodRef;
    IfFailRet(m_spReflectionHelper->DefineMethodToken(spModuleInfo, spMethodToRedirect, tkMethodRef));


    IMetaDataEmitSptr spIMetaDataEmit;
    IfFailRet(spModuleInfo->GetMetaDataEmit(reinterpret_cast<IUnknown**>(&spIMetaDataEmit)));

    //Create instructions
    IInstructionFactorySptr spInstructionFactory;
    IfFailRet(spMethodInfo->GetInstructionFactory(&spInstructionFactory));

    IInstructionGraphSptr spInstructionGraph;
    IfFailRet(spMethodInfo->GetInstructions(&spInstructionGraph));
    spInstructionGraph->RemoveAll();

    IInstructionSptr spCurrent;
    IfFailRet(spInstructionGraph->GetFirstInstruction(&spCurrent));

	for (USHORT i = 0; i < argsCount; i++)
    {
        IInstructionSptr spLoadArg;

        IfFailRet(spInstructionFactory->CreateLoadArgInstruction(i, &spLoadArg));
        IfFailRet(spInstructionGraph->InsertAfter(spCurrent, spLoadArg));

        spCurrent = spLoadArg;
    }

    IInstructionSptr spCallMethod;

    IfFailRet(spInstructionFactory->CreateTokenOperandInstruction(Cee_Call, tkMethodRef, &spCallMethod));
    IfFailRet(spInstructionGraph->InsertAfter(spCurrent, spCallMethod));
    spCurrent = spCallMethod;

    IInstructionSptr spReturn;

    IfFailRet(spInstructionFactory->CreateInstruction(Cee_Ret, &spReturn));
    IfFailRet(spInstructionGraph->InsertAfter(spCurrent, spReturn));
    spCurrent = spReturn;

    return hr;
}
