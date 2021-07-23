// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "../InstrumentationEngine.Api/InstrumentationEngine.h"
#include "InstrumentationMethod.h"
#include "InstrumentationMethodAttachContext.h"
#ifndef PLATFORM_UNIX
#include "SignatureValidator.h"
#endif
#include "../Common.Headers/StringUtils.h"


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

HRESULT MicrosoftInstrumentationEngine::CInstrumentationMethod::Initialize(
    _In_ IProfilerManager* pProfilerManager,
    _In_ bool validateCodeSignature
    )
{
    HRESULT hr = S_OK;

    IfFailRet(InitializeCore(validateCodeSignature));

    hr = m_pInstrumentationMethod->Initialize(pProfilerManager);
    if (FAILED(hr))
    {
        CLogging::LogError(
            _T("CInstrumentationMethod::Initialize - failed to initialize instrumentation method PID: %u, hr: %x, name: %s"),
            GetCurrentProcessId(), hr, m_bstrName.m_str);
        return hr;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstrumentationMethod::InitializeForAttach(
    _In_ IProfilerManager* pProfilerManager,
    _In_ IEnumInstrumentationMethodSettings* pSettingsEnum,
    _In_ bool validateCodeSignature
    )
{
    HRESULT hr = S_OK;

    IfFailRet(InitializeCore(validateCodeSignature));

    CComQIPtr<IInstrumentationMethodAttach> pInstrumentationMethodAttach(m_pInstrumentationMethod);
    IfNullRet(pInstrumentationMethodAttach);

    CComPtr<IInstrumentationMethodAttachContext> pContext;
    pContext.Attach(new (nothrow) CInstrumentationMethodAttachContext(pSettingsEnum));
    IfFalseRet(nullptr != pContext, E_OUTOFMEMORY);

    IfFailRet(pInstrumentationMethodAttach->InitializeForAttach(pProfilerManager, pContext));

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstrumentationMethod::InitializeCore(
    _In_ bool validateCodeSignature
    )
{
    HRESULT hr = S_OK;

    if ((m_bstrName.Length() == 0) ||
        (m_bstrDescription.Length() == 0) ||
        (m_bstrModuleFolder.Length() == 0) ||
        (m_bstrModule.Length() == 0))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - Missing configuration param, PID: %u"), GetCurrentProcessId());
        return E_INVALIDARG;
    }

    if ((m_bstrModuleFolder.Length() >= MAX_PATH))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - module folder path is too long, PID: %u"), GetCurrentProcessId());
        return E_BOUNDS;
    }

    WCHAR wszModuleFullPath[MAX_PATH];
    ZeroMemory(wszModuleFullPath, MAX_PATH * sizeof(WCHAR));
    wcscpy_s(wszModuleFullPath, MAX_PATH, m_bstrModuleFolder);

#ifndef PLATFORM_UNIX
    if (FAILED(StringUtils::SafePathAppend(wszModuleFullPath, m_bstrModule, MAX_PATH)))
#else
    if (FAILED(PathCchAppend(wszModuleFullPath, MAX_PATH, m_bstrModule)))
#endif
    {
        CLogging::LogError(
            _T("CInstrumentationMethod::Initialize - failed to append method dll to method configuration folder, PID: %u, hr: %x, target: '%s' + '%s'"),
            GetCurrentProcessId(), hr, m_bstrModuleFolder.m_str, m_bstrModule.m_str);
        return hr;
    }

    m_hmod = ::LoadLibrary(wszModuleFullPath);

    if (m_hmod == NULL)
    {
        auto error = ::GetLastError();
        // Failed to load the module.
        CLogging::LogError(
            _T("CInstrumentationMethod::Initialize - failed to load instrumentation method module, path=%s, PID: %u, error: %d, name: %s"),
            wszModuleFullPath, GetCurrentProcessId(), error, m_bstrName.m_str);
        return HRESULT_FROM_WIN32(error);
    }

    #ifndef PLATFORM_UNIX
    if (validateCodeSignature)
    {
        CSignatureValidator validator;
        if (FALSE == validator.VerifyEmbeddedSignature(wszModuleFullPath))
        {
            CLogging::LogError(
                _T("CInstrumentationMethod::Initialize - Instrumentation Method should be code signed, PID: %u, name: %s"),
                GetCurrentProcessId(), m_bstrName.m_str);
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
        CLogging::LogError(
            _T("CInstrumentationMethod::Initialize - failed to find DllGetClassObject, PID: %u, error: %d, name: %s"),
            GetCurrentProcessId(), error, m_bstrName.m_str);
        return HRESULT_FROM_WIN32(error);
    }

    CComPtr<IClassFactory> pFactory;
    hr = pfnDllGetClassObject(m_guidClassId, __uuidof(IClassFactory), (LPVOID*)&pFactory);
    if (FAILED(hr))
    {
        CLogging::LogError(
            _T("CInstrumentationMethod::Initialize - failed to get class factory from instrumentation method dll, PID: %u, hr: %x, name: %s"),
            GetCurrentProcessId(), hr, m_bstrName.m_str);
        return hr;
    }

    hr = pFactory->CreateInstance(NULL, __uuidof(IInstrumentationMethod), (LPVOID*)&m_pInstrumentationMethod);
    if (FAILED(hr))
    {
        CLogging::LogError(
            _T("CInstrumentationMethod::Initialize - failed to create instance from instrumentation method dll, PID: %u, hr: %x, name: %s"),
            GetCurrentProcessId(), hr, m_bstrName.m_str);
        return hr;
    }

    return S_OK;
}

HRESULT MicrosoftInstrumentationEngine::CInstrumentationMethod::AttachComplete()
{
    HRESULT hr = S_OK;

    CComQIPtr<IInstrumentationMethodAttach> pInstrumentationMethodAttach(m_pInstrumentationMethod);
    IfNullRet(pInstrumentationMethodAttach);

    IfFailRet(pInstrumentationMethodAttach->AttachComplete());

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