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
//extern "C"
//{
//#include "dlfcn.h"
//}


MicrosoftInstrumentationEngine::CInstrumentationMethod::CInstrumentationMethod(
    _In_ LPCWSTR zwModuleFolder,
    _In_ LPCWSTR zwName,
    _In_ LPCWSTR zwDescription,
    _In_ LPCWSTR zwModule,
    _In_ GUID& guidClassId,
    _In_ DWORD dwPriority
) : m_hmod(NULL), m_bstrName(zwName), m_bstrModuleFolder(zwModuleFolder), m_bstrModule(zwModule), m_bstrDescription(zwDescription), m_guidClassId(guidClassId), m_dwPriority(dwPriority)
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
    CLogging::LogMessage(_T("CInstrumentationMethod::InitializeCore - validateCodeSignature, PID: %d"), validateCodeSignature);
    CLogging::LogError(_T("CInstrumentationMethod::InitializeCore - validateCodeSignature, PID: %d"), validateCodeSignature);

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

    hr = StringUtils::SafePathAppend(wszModuleFullPath, m_bstrModule, MAX_PATH);
    if (FAILED(hr))
    {
        CLogging::LogError(
            _T("CInstrumentationMethod::Initialize - failed to append method dll to method configuration folder, PID: %u, hr: %x, target: '%s' + '%s'"),
            GetCurrentProcessId(), hr, m_bstrModuleFolder.m_str, m_bstrModule.m_str);
        return hr;
    }

    // open the library
    /*CLogging::LogError(
        _T("Opening libInstrumentationEngine.Method.so"));
    void* handle = dlopen("/home/azureuser/AspNetCore3Test/base/libInstrumentationEngine.Method.so", RTLD_GLOBAL);

    char* error;
    error = dlerror();
    if (!error)
    {
        CLogging::LogError(
            _T("ERROR\tNo error from dlopen for non-library file\n"));
    }
    else
        printf("SUCCESS\tCould not open file with too long file name: %s\n", error);

    if (!handle) {
        CLogging::LogError(
            _T("Cannot open library... "));
        return 1;
    }*/

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

//HRESULT MicrosoftInstrumentationEngine::CInstrumentationMethod::GetLastErrorAsString()
//{
//    HRESULT hr = S_OK;
//    //Get the error message ID, if any.
//    DWORD errorMessageID = ::GetLastError();
//    if (errorMessageID == 0) {
//        return ((HRESULT)1L); //No error message has been recorded
//    }
//
//    LPSTR messageBuffer = nullptr;
//
//    //Ask Win32 to give us the string version of that message ID.
//    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
//    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
//
//    ////Copy the error message into a std::string.
//    //std::wstring message(messageBuffer, size);
//
//
//    CLogging::LogError(
//        _T("CInstrumentationMethod::GetLastErrorAsString - LastError: %s"),
//        messageBuffer);
//
//    //Free the Win32's string's buffer.
//    LocalFree(messageBuffer);
//
//    return hr;
//}

REFGUID MicrosoftInstrumentationEngine::CInstrumentationMethod::GetClassId()
{
    return m_guidClassId;
}

DWORD MicrosoftInstrumentationEngine::CInstrumentationMethod::GetPriority()
{
    return m_dwPriority;
}