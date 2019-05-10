// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>
#include <combaseapi.h>
#include <string>
#include <vector>
#include <tchar.h>
#include <regex>

// Handle to profiler profiler module.
HMODULE hProfiler = nullptr;

// Critical section object which provides synchronization when accessing hProfiler.
CRITICAL_SECTION criticalSection;

BOOL APIENTRY DllMain(
    HMODULE hModule,   // handle to DLL module
    DWORD fdwReason,   // reason for calling function
    LPVOID lpReserved) // reserved
{
    /*
        If fdwReason is DLL_PROCESS_ATTACH, lpReserved is NULL for dynamic loads, non-NULL for static loads.
        If fdwReason is DLL_PROCESS_DETACH, lpReserved is NULL if FreeLibrary called or DLL load failed, non-NULL for process termination.
    */

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        InitializeCriticalSection(&criticalSection);
        break;
    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;
    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;
    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        DeleteCriticalSection(&criticalSection);
        break;
    }
    return TRUE;
}

static BOOL IsValidFormat(std::wstring const& versionStr, BOOL useDebug, BOOL usePreview)
{
    std::wstring versionRegexStr = _T("\\d+\\.\\d+\\.\\d+");
    versionRegexStr += (usePreview ? _T("(-build\\d+)?") : _T(""));
    versionRegexStr += (useDebug ? _T("(_Debug)?") : _T(""));
    std::wregex versionRegex(versionRegexStr.c_str());

    return std::regex_match(versionStr, versionRegex) ? TRUE : FALSE;//&& wmatch.size() > 0;
}

static void GetLatestVersionFolder(BOOL useDebug, BOOL usePreview, std::wstring profilerPath, std::wstring& versionFolder)
{
    WIN32_FIND_DATA ffd;
    profilerPath += _T("*"); // FilePattern
    HANDLE hFind = FindFirstFile(profilerPath.c_str(), &ffd);

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (IsValidFormat(ffd.cFileName, useDebug, usePreview) == TRUE)
            {
                if (versionFolder.compare(ffd.cFileName) < 0)
                {
                    versionFolder = ffd.cFileName;
                }
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);
}

/*
 * The proxy searches for the latest [VERSION] folder in order to pick up the InstrumentationEngine profiler dll.
 * The 32-bit proxy searches 32-bit paths only. Similarly for 64-bit proxy.
 *
 * %ProgramFiles|(x86)%
 *     Microsoft CLR Instrumentation Engine
 *         [VERSION]
 *             Instrumentation64|32
 *                 MicrosoftInstrumentationEngine_x64|86.dll
 */
_Check_return_
HRESULT __stdcall DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ PVOID *ppObj)
{
    HRESULT hr = E_FAIL;

    WCHAR wszEnvVar[MAX_PATH];
    if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_DebugWait"), wszEnvVar, MAX_PATH) > 0)
    {
        while (!IsDebuggerPresent())
        {
            Sleep(100);
        }
    }

    LPCWSTR instrumentationEngineFolder = _T("Microsoft CLR Instrumentation Engine\\");
    LPCWSTR useDebugVar = _T("InstrumentationEngineProxy_UseDebug");
    LPCWSTR usePreviewVar = _T("InstrumentationEngineProxy_UsePreview");
    LPCWSTR useSpecificVersionVar = _T("InstrumentationEngineProxy_UseSpecificVersion");

#ifdef _WIN64
    LPCWSTR profilerFileName = _T("MicrosoftInstrumentationEngine_x64.dll");
    LPCWSTR instrumentationPlatformFolder = _T("Instrumentation64\\");
    LPCWSTR programFilesVar = _T("ProgramFiles");
#else
    LPCWSTR profilerFileName = _T("MicrosoftInstrumentationEngine_x86.dll");
    LPCWSTR instrumentationPlatformFolder = _T("Instrumentation32\\");
    LPCWSTR programFilesVar = _T("ProgramFiles(x86)");
#endif

    // ProgramFiles
    WCHAR wszProfilerPath[MAX_PATH];
    ZeroMemory(wszProfilerPath, MAX_PATH);
    if (GetEnvironmentVariable(programFilesVar, wszProfilerPath, MAX_PATH) > 0)
    {
        return E_UNEXPECTED;
    }

    // ProgramFiles/
    std::wstring profilerPath(wszProfilerPath);
    if (profilerPath.back() != '\\')
    {
        profilerPath += '\\';
    }

    // ProgramFiles/Microsoft CLR Instrumentation Engine
    profilerPath += instrumentationEngineFolder;

    WCHAR wszUseDebug[2];
    ZeroMemory(wszUseDebug, 2);
    bool useDebug = GetEnvironmentVariable(useDebugVar, wszUseDebug, 2) > 0 && wcscmp(wszUseDebug, _T("1")) == 0;

    WCHAR wszUsePreviewVal[2];
    ZeroMemory(wszUsePreviewVal, 2);
    bool usePreview = GetEnvironmentVariable(usePreviewVar, wszUsePreviewVal, 2) > 0 && wcscmp(wszUsePreviewVal, _T("1")) == 0;

    WCHAR specificVersionStr[MAX_PATH];
    bool useSpecificVersion = GetEnvironmentVariable(useSpecificVersionVar, specificVersionStr, 1) > 0;

    std::wstring versionFolder;
    if (useSpecificVersion)
    {
        versionFolder = specificVersionStr;
    }
    else
    {
        GetLatestVersionFolder(useDebug, usePreview, profilerPath, versionFolder);
    }
    
    profilerPath += versionFolder;
    profilerPath += _T('\\');
    profilerPath += instrumentationPlatformFolder;
    profilerPath += profilerFileName;

    ::EnterCriticalSection(&criticalSection);

    hProfiler = ::LoadLibrary(profilerPath.c_str());
    if (hProfiler != nullptr)
    {
        LPFNGETCLASSOBJECT dllGetClassObj = (LPFNGETCLASSOBJECT)::GetProcAddress(hProfiler, "DllGetClassObject");
        if (dllGetClassObj != nullptr)
        {
            hr = dllGetClassObj(rclsid, riid, ppObj);
        }
        else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    else
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    ::LeaveCriticalSection(&criticalSection);

    return hr;
}

__control_entrypoint(DllExport)
HRESULT __stdcall DllCanUnloadNow(void)
{
    HRESULT hr = S_OK;

    ::EnterCriticalSection(&criticalSection);

    if (hProfiler != nullptr)
    {
        LPFNCANUNLOADNOW dllCanUnloadNow = (LPFNCANUNLOADNOW)::GetProcAddress(hProfiler, "DllCanUnloadNow");
        if (dllCanUnloadNow != nullptr)
        {
            hr = dllCanUnloadNow();
            if (hr == S_OK)
            {
                ::FreeLibrary(hProfiler);
                hProfiler = nullptr;
            }
        }
    }

    ::LeaveCriticalSection(&criticalSection);

    return hr;
}