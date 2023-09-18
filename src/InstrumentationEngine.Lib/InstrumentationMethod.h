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
            _In_ LPCWSTR zwModuleFolder,
            _In_ LPCWSTR zwName,
            _In_ LPCWSTR zwDescription,
            _In_ LPCWSTR zwModule,
            _In_ GUID& guidClassId,
            _In_ DWORD dwPriority
        );

        HRESULT Initialize(
            _In_ IProfilerManager* pProfilerManager,
            _In_ bool validateCodeSignature
        );

        HRESULT InitializeForAttach(
            _In_ IProfilerManager* pProfilerManager,
            _In_ IEnumInstrumentationMethodSettings* pSettingsEnum,
            _In_ bool validateCodeSignature
        );

        HRESULT AttachComplete();

        HRESULT GetRawInstrumentationMethod(_Out_ IInstrumentationMethod** ppInstrumentationMethod);
        HRESULT GetLastErrorAsString();
        REFGUID GetClassId();

        DWORD GetPriority();

    private:
        HRESULT InitializeCore(
            _In_ bool validateCodeSignature
        );
    };
}