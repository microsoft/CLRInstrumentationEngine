// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// This file represents an instrumentation method to the profiler manager.

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    class CInstrumentationMethod
    {
    private:
        HMODULE m_hmod;

        CComBSTR m_bstrName;
        CComBSTR m_bstrDescription;
        CComBSTR m_bstrModuleFolder;
        CComBSTR m_bstrModule;
        DWORD m_dwPriority;

        GUID m_guidClassId;

        // pointer to the actual instrumentation method implementation;
        CComPtr<IInstrumentationMethod> m_pInstrumentationMethod;

    public:
        CInstrumentationMethod(
            _In_ BSTR bstrModuleFolder,
            _In_ BSTR bstrName,
            _In_ BSTR bstrDescription,
            _In_ BSTR bstrModule,
            _In_ GUID& guidClassId,
            _In_ DWORD dwPriority
            );

        HRESULT Initialize(_In_ IProfilerManager* pProfilerManager, bool validateCodeSignature);

        HRESULT GetRawInstrumentationMethod(_Out_ IInstrumentationMethod** ppInstrumentationMethod);

        REFGUID GetClassId();

        DWORD GetPriority();
    };
}