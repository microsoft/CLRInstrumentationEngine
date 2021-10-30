#include "stdafx.h"
#include "InstrumentationEngineApi.h"
#include "../Common.Headers/InitOnce.h"

#include <Windows.h>

#if defined(X86)
const WCHAR InstrumentationEngineModule[] = _T("MicrosoftInstrumentationEngine_x86.dll");
#elif defined(X64)
const WCHAR InstrumentationEngineModule[] = _T("MicrosoftInstrumentationEngine_x64.dll");
#else
#error "Platform Not Supported"
#endif

using namespace std;

class ApiFunctions
{
public:
    using TFreeString = HRESULT(STDMETHODCALLTYPE*)(BSTR bstr);
    TFreeString _InstrumentationEngineFreeString = nullptr;
    HRESULT Initialize(LPCWSTR szModuleName);
};

static ApiFunctions g_apiFunctions;
static CInitOnce g_initializer([] { return g_apiFunctions.Initialize(InstrumentationEngineModule); });


HRESULT InstrumentationEngineApi::FreeString(BSTR bstr)
{
    HRESULT hr;
    IfFailRet(g_initializer.Get());
    return g_apiFunctions._InstrumentationEngineFreeString(bstr);
}

HRESULT ApiFunctions::Initialize(LPCWSTR szModuleName)
{
    // Todo: this will have to be done differently for non Windows platforms.
    HMODULE mod = GetModuleHandle(szModuleName);
    if (mod == nullptr)
    {
        return E_NOTIMPL;
    }

    _InstrumentationEngineFreeString = (TFreeString)GetProcAddress(mod, "InstrumentationEngineFreeString");
    if (_InstrumentationEngineFreeString == nullptr)
    {
        return E_NOTIMPL;
    }
}

