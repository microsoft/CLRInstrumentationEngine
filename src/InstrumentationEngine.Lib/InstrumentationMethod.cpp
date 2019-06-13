// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "../InstrumentationEngine.Api/InstrumentationEngine.h"
#include "InstrumentationMethod.h"
#ifndef PLATFORM_UNIX
#include "SignatureValidator.h"
#endif


MicrosoftInstrumentationEngine::CInstrumentationMethod::CInstrumentationMethod(
    _In_ BSTR bstrModuleFolder,
    _In_ BSTR bstrName,
    _In_ BSTR bstrDescription,
    _In_ BSTR bstrModule,
    _In_ GUID& guidClassId,
    _In_ DWORD dwPriority
    ) : m_hmod(NULL), m_bstrName(bstrName), m_bstrModuleFolder(bstrModuleFolder), m_bstrModule(bstrModule), m_bstrDescription(bstrDescription), m_guidClassId(guidClassId), m_dwPriority(dwPriority)
{

}


HRESULT MicrosoftInstrumentationEngine::CInstrumentationMethod::Initialize(_In_ IProfilerManager* pProfilerManager, bool validateCodeSignature)
{
    HRESULT hr = S_OK;

    if ((m_bstrName.Length() == 0) ||
        (m_bstrDescription.Length() == 0) ||
        (m_bstrModuleFolder.Length() == 0) ||
        (m_bstrModule.Length() == 0))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - Missing configuration param"));
        return E_INVALIDARG;
    }

    WCHAR wszModuleFullPath[MAX_PATH];
    memset(wszModuleFullPath, 0, MAX_PATH);
    wcscpy_s(wszModuleFullPath, MAX_PATH, m_bstrModuleFolder);
    PathCchAppend(wszModuleFullPath, MAX_PATH, m_bstrModule);

    m_hmod = ::LoadLibrary(wszModuleFullPath);

    if (m_hmod == NULL)
    {
        auto error = ::GetLastError();
        // Failed to load the module.
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - failed to load instrumentation method module (path=%s, error=%d)"), wszModuleFullPath, error);
        return HRESULT_FROM_WIN32(error);
    }

    #ifndef PLATFORM_UNIX
    if (validateCodeSignature)
    {
        CSignatureValidator validator;
        if (FALSE == validator.VerifyEmbeddedSignature(wszModuleFullPath))
        {
            CLogging::LogError(_T("CInstrumentationMethod::Initialize - Instrumentaiton Method should be code signed"));
            FreeLibrary(m_hmod);
            return E_FAIL;
        }
    }
    #endif

    typedef HRESULT(__stdcall* DLLGETCLASSOBJECT)(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ void** ppvObject);

    DLLGETCLASSOBJECT pfnDllGetClassObject = (DLLGETCLASSOBJECT)GetProcAddress(m_hmod, "DllGetClassObject");
    if (!pfnDllGetClassObject)
    {
        auto error = ::GetLastError();
        FreeLibrary(m_hmod);
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - failed to find DllGetClassObject"));
        return HRESULT_FROM_WIN32(error);
    }

    CComPtr<IClassFactory> pFactory;
    hr = pfnDllGetClassObject(m_guidClassId, __uuidof(IClassFactory), (LPVOID*)&pFactory);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - failed to get class factory from instrumentation method dll"));
        return hr;
    }


    hr = pFactory->CreateInstance(NULL, __uuidof(IInstrumentationMethod), (LPVOID*)&m_pInstrumentationMethod);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - failed to get class factory from instrumentation method dll"));
        return hr;
    }

    hr = m_pInstrumentationMethod->Initialize(pProfilerManager);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - failed to initialize instrumentation method PID: %u hr: %x name: %s"), GetCurrentProcessId(), hr, m_bstrName.m_str);
        return hr;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstrumentationMethod::GetRawInstrumentationMethod(_Out_ IInstrumentationMethod** ppInstrumentationMethod)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(ppInstrumentationMethod);

    hr = m_pInstrumentationMethod.CopyTo(ppInstrumentationMethod);

    return hr;
}

REFGUID MicrosoftInstrumentationEngine::CInstrumentationMethod::GetClassId()
{
    return m_guidClassId;
}

DWORD MicrosoftInstrumentationEngine::CInstrumentationMethod::GetPriority()
{
    return m_dwPriority;
}