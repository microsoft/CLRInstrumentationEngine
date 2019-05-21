// Copyright (c) Microsoft Corporation. All rights reserved.
//

// dllmain.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

namespace ProfilerProxy
{
    // Handle to profiler module.
    HMODULE g_hProfiler = nullptr;

    // Critical section object which provides synchronization when accessing hProfiler.
    CRITICAL_SECTION g_criticalSection;

    LPFN_ISWOW64PROCESS fnIsWow64Process;

    static constexpr const WCHAR* instrumentationEngineFolder = _T("Microsoft CLR Instrumentation Engine");
    static constexpr const WCHAR* useDebugVar = _T("InstrumentationEngineProxy_UseDebug");
    static constexpr const WCHAR* usePreviewVar = _T("InstrumentationEngineProxy_UsePreview");
    static constexpr const WCHAR* useSpecificVersionVar = _T("InstrumentationEngineProxy_UseSpecificVersion");

#ifdef _WIN64
    static constexpr const WCHAR* profilerRelativeFileName = _T("Instrumentation64\\MicrosoftInstrumentationEngine_x64.dll");
#else
    static constexpr const WCHAR* profilerRelativeFileName = _T("Instrumentation32\\MicrosoftInstrumentationEngine_x86.dll");
#endif
    extern "C" BOOL APIENTRY DllMain(
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
            InitializeCriticalSection(&g_criticalSection);
            break;
        case DLL_THREAD_ATTACH:
            // Do thread-specific initialization.
            break;
        case DLL_THREAD_DETACH:
            // Do thread-specific cleanup.
            break;
        case DLL_PROCESS_DETACH:
            // Perform any necessary cleanup.
            DeleteCriticalSection(&g_criticalSection);
            break;
        }
        return TRUE;
    }

    /*
     * This function returns whether the current process is running under Wow64
     */
    static HRESULT IsWow64(_Out_ BOOL* bIsWow64)
    {
        *bIsWow64 = FALSE;

        // IsWow64Process is not available on all supported versions of Windows.
        // Use GetModuleHandle to get a handle to the DLL that contains the function.
        // and GetProcAddress to get a pointer to the function if available.
        fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
            GetModuleHandle(_T("kernel32")), "IsWow64Process");

        if (NULL != fnIsWow64Process)
        {
            if (!fnIsWow64Process(GetCurrentProcess(), bIsWow64))
            {
                // handle error
                return HRESULT_FROM_WIN32(GetLastError());
            }
        }

        return S_OK;
    }

    /*
     * This function returns whether the profiler dll exists.
     */
    static HRESULT HasProfilerDll(_In_ LPCWSTR wszProfilerPath, _In_ LPCWSTR wszVersionFolder, _Out_ BOOL* hasProfiler)
    {
        HRESULT hr = S_OK;
        *hasProfiler = FALSE;

        WCHAR wszEngineFullPath[MAX_PATH];
        errno_t error = wcscpy_s(wszEngineFullPath, MAX_PATH, wszProfilerPath);
        if (error != 0)
        {
            return MAKE_HRESULT_FROM_ERRNO(error);
        }

        IfFailRetNoLog(PathCchAppend(wszEngineFullPath, MAX_PATH, wszVersionFolder));
        IfFailRetNoLog(PathCchAppend(wszEngineFullPath, MAX_PATH, profilerRelativeFileName));

        FILE* file;
        error = _wfopen_s(&file, wszEngineFullPath, _T("r"));
        if (file != nullptr)
        {
            fclose(file);
        }

        if (error == 0)
        {
            *hasProfiler = TRUE;
            return S_OK;
        }

        return E_FAIL;
    }

    /*
     * This function iterates over the versioned folders in the profilerPath and returns the latest folder containing the profiler dll.
     */
    static HRESULT GetLatestVersionFolder(_In_ LPCWSTR wszProfilerPath, _Out_ std::wstring& versionFolder)
    {
        HRESULT hr = S_OK;

        //
        // Check configuration flags
        //

        WCHAR wszBuffer[2];
        ZeroMemory(wszBuffer, 2);
        BOOL useDebug = GetEnvironmentVariable(useDebugVar, wszBuffer, 2) > 0 && wcscmp(wszBuffer, _T("1")) == 0;

        ZeroMemory(wszBuffer, 2);
        BOOL usePreview = GetEnvironmentVariable(usePreviewVar, wszBuffer, 2) > 0 && wcscmp(wszBuffer, _T("1")) == 0;

        //
        // Iterate folders & compare versions
        //

        versionFolder.clear();

        WIN32_FIND_DATA findFileData;
        std::wstring profilerPathFilter = std::wstring(wszProfilerPath);

        // FilePattern
        profilerPathFilter += _T("\\*");

        SafeFindFileHandle hSearchHandle = FindFirstFile(profilerPathFilter.c_str(), &findFileData);
        if (hSearchHandle == INVALID_HANDLE_VALUE)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        InstrumentationEngineVersion latestVersionFolder;
        do {
            // Skip any files; we only want directories
            if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                continue;
            }

            // Skip any invalid folder formats
            InstrumentationEngineVersion folderVersion;
            if (FAILED(InstrumentationEngineVersion::Create(findFileData.cFileName, folderVersion)))
            {
                continue;
            }

            // Skip any versions that are filtered by configuration flags
            if (folderVersion.IsDebug() != useDebug ||
                (!usePreview && folderVersion.IsPreview()))
            {
                continue;
            }

            // Skip any folders without profiler
            BOOL hasProfiler;
            if (FAILED(HasProfilerDll(wszProfilerPath, folderVersion.c_str(), &hasProfiler)) || !hasProfiler)
            {
                continue;
            }

            if (latestVersionFolder.Compare(folderVersion) < 0)
            {
                latestVersionFolder = folderVersion;
            }

        } while (FindNextFile(hSearchHandle, &findFileData));

        // Only return if no VersionFolder found
        DWORD dError = GetLastError();
        if (latestVersionFolder.ToString().empty() ||
            dError > 0 && dError != ERROR_NO_MORE_FILES)
        {
            versionFolder.clear();
            return HRESULT_FROM_WIN32(dError);
        }

        versionFolder = latestVersionFolder.ToString();
        return S_OK;
    }

    /*
     * The proxy searches for the latest [VERSION] folder in order to pick up the
    *  InstrumentationEngine profiler dll and forwards the DllGetClassObject call.
     * The 32-bit proxy searches 32-bit paths only. Similarly for 64-bit proxy.
     *
     * %ProgramFiles|(x86)%
     *     Microsoft CLR Instrumentation Engine
     *         [VERSION]
     *             Instrumentation64|32
     *                 MicrosoftInstrumentationEngine_x64|86.dll
     */
    _Check_return_
    STDAPI DLLEXPORT(DllGetClassObject, 12)(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ PVOID* ppObj)
    {
        HRESULT hr = S_OK;
        WCHAR wszEnvVar[MAX_PATH];
        if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_DebugWait"), wszEnvVar, MAX_PATH) > 0)
        {
            while (!IsDebuggerPresent())
            {
                Sleep(100);
            }
        }

        //
        // Determine "Program Files" folder
        //

        LPCWSTR programFilesVar = _T("ProgramFiles");

#ifndef _WIN64
        // WoW64 = Windows (32bit) on Windows 64bit
        BOOL bIsWow64 = FALSE;
        IfFailRetNoLog(IsWow64(&bIsWow64));
        if (bIsWow64)
        {
            programFilesVar = _T("ProgramFiles(x86)");
        }
#endif

        WCHAR wszProfilerPath[MAX_PATH];
        ZeroMemory(wszProfilerPath, MAX_PATH);
        if (!GetEnvironmentVariable(programFilesVar, wszProfilerPath, MAX_PATH))
        {
            return E_UNEXPECTED;
        }

        //
        // Determine CIE folder
        //

        IfFailRetNoLog(PathCchAppend(wszProfilerPath, MAX_PATH, instrumentationEngineFolder));

        //
        // Determine Version folder
        //

        WCHAR specificVersionStr[MAX_PATH];
        BOOL useSpecificVersion = GetEnvironmentVariable(useSpecificVersionVar, specificVersionStr, 1) > 0;

        std::wstring versionFolder;
        if (useSpecificVersion)
        {
            versionFolder = specificVersionStr;
        }
        else
        {
            IfFailRetNoLog(GetLatestVersionFolder(wszProfilerPath, versionFolder));
        }

        IfFailRetNoLog(PathCchAppend(wszProfilerPath, MAX_PATH, versionFolder.c_str()));

        //
        // Determine and load Profiler
        //

        IfFailRetNoLog(PathCchAppend(wszProfilerPath, MAX_PATH, profilerRelativeFileName));

        ::EnterCriticalSection(&g_criticalSection);

        g_hProfiler = ::LoadLibrary(wszProfilerPath);
        if (g_hProfiler != nullptr)
        {
            LPFNGETCLASSOBJECT dllGetClassObj = (LPFNGETCLASSOBJECT)::GetProcAddress(g_hProfiler, "DllGetClassObject");
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

        ::LeaveCriticalSection(&g_criticalSection);

        return S_OK;
    }

    __control_entrypoint(DllExport)
    STDAPI DLLEXPORT(DllCanUnloadNow, 0)(void)
    {
        HRESULT hr = S_OK;

        ::EnterCriticalSection(&g_criticalSection);

        if (g_hProfiler != nullptr)
        {
            LPFNCANUNLOADNOW dllCanUnloadNow = (LPFNCANUNLOADNOW)::GetProcAddress(g_hProfiler, "DllCanUnloadNow");
            if (dllCanUnloadNow != nullptr)
            {
                hr = dllCanUnloadNow();
                if (hr == S_OK)
                {
                    ::FreeLibrary(g_hProfiler);
                    hProfiler = nullptr;
                }
            }
        }

        ::LeaveCriticalSection(&g_criticalSection);

        return hr;
    }
}