// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// dllmain.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"
#include "ProxyLogging.h"
#include "PathCch.h"

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

namespace ProfilerProxy
{
    // Handle to profiler module.
    static HMODULE g_hProfiler = nullptr;

    // Critical section object which provides synchronization when accessing hProfiler.
    static CCriticalSection g_criticalSection;

    static LPFN_ISWOW64PROCESS fnIsWow64Process;

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
            break;
        case DLL_THREAD_ATTACH:
            // Do thread-specific initialization.
            break;
        case DLL_THREAD_DETACH:
            // Do thread-specific cleanup.
            break;
        case DLL_PROCESS_DETACH:
            // Perform any necessary cleanup.
            break;
        }
        return TRUE;
    }

#ifndef _WIN64
    /*
     * This function returns whether the current process is running under Wow64
     */
    static HRESULT IsWow64(_Out_ BOOL* pIsWow64)
    {
        *pIsWow64 = FALSE;

        // IsWow64Process is not available on all supported versions of Windows.
        // Use GetModuleHandle to get a handle to the DLL that contains the function.
        // and GetProcAddress to get a pointer to the function if available.
        HMODULE hModKernel32 = GetModuleHandle(_T("kernel32"));
        if (nullptr != hModKernel32)
        {
            fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
                hModKernel32, "IsWow64Process");
        }
        else
        {
            CProxyLogging::LogError(_T("dllmain::IsWow64 - Unable to obtain kernel32 handle"));
            return E_UNEXPECTED;
        }

        if (NULL != fnIsWow64Process)
        {
            if (!fnIsWow64Process(GetCurrentProcess(), pIsWow64))
            {
                // handle error
                return HRESULT_FROM_WIN32(GetLastError());
            }

            if (*pIsWow64)
            {
                CProxyLogging::LogMessage(_T("dllmain::IsWow64 - Process is running in WoW64"));
            }
        }

        return S_OK;
    }
#endif

    /*
     * This function returns whether the profiler dll exists.
     */
    static HRESULT HasProfilerDll(_In_ LPCWSTR wszProfilerPath, _In_ LPCWSTR wszVersionFolder, _Out_ BOOL* pHasProfiler)
    {
        HRESULT hr = S_OK;

        *pHasProfiler = FALSE;

        WCHAR wszEngineFullPath[MAX_PATH];
        IfFailRetErrno_Proxy(wcscpy_s(wszEngineFullPath, MAX_PATH, wszProfilerPath));

        IfFailRet_Proxy(PathCchAppend(wszEngineFullPath, MAX_PATH, wszVersionFolder));
        IfFailRet_Proxy(PathCchAppend(wszEngineFullPath, MAX_PATH, profilerRelativeFileName));

        DWORD dwAttrib = GetFileAttributes(wszEngineFullPath);
        *pHasProfiler = dwAttrib != INVALID_FILE_ATTRIBUTES &&
                       !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);

        return S_OK;
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

        // FilePattern is required for FindFirstFile to search
        // the wszProfilerPath directory rather than its parent.
        profilerPathFilter += _T("\\*");

        SafeFindFileHandle hSearchHandle = FindFirstFile(profilerPathFilter.c_str(), &findFileData);
        if (hSearchHandle == INVALID_HANDLE_VALUE)
        {
            CProxyLogging::LogError(_T("dllmain::GetLatestVersionFolder - No files or folders found"));
            return HRESULT_FROM_WIN32(GetLastError());
        }

        InstrumentationEngineVersion* pLatestVersionFolder = nullptr;
        do {
            // Skip any files; we only want directories
            if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                continue;
            }

            // Skip any invalid folder formats
            InstrumentationEngineVersion* pFolderVersion = nullptr;
            if (FAILED(InstrumentationEngineVersion::Create(findFileData.cFileName, &pFolderVersion)))
            {
                continue;
            }

            // Skip any versions that are filtered by configuration flags
            if (pFolderVersion->IsDebug() != useDebug ||
                (!usePreview && pFolderVersion->IsPreview()))
            {
                continue;
            }

            // Skip any folders without profiler
            BOOL hasProfiler;
            if (FAILED(HasProfilerDll(wszProfilerPath, pFolderVersion->GetVersionString().c_str(), &hasProfiler)) || !hasProfiler)
            {
                continue;
            }

            CProxyLogging::LogMessage(_T("dllmain::GetLatestVersionFolder - Found valid version folder '%s'"), pFolderVersion->GetVersionString().c_str());

            if (pLatestVersionFolder == nullptr ||
                pLatestVersionFolder->Compare(*pFolderVersion) < 0)
            {
                pLatestVersionFolder = pFolderVersion;
            }

        } while (FindNextFile(hSearchHandle, &findFileData));

        // Only return if no VersionFolder found
        DWORD dError = GetLastError();
        if (pLatestVersionFolder == nullptr)
        {
            CProxyLogging::LogError(_T("dllmain::GetLatestVersionFolder - Unable to find a valid versioned folder with profiler"));
            return E_UNEXPECTED;
        }
        else if(dError > 0 && dError != ERROR_NO_MORE_FILES)
        {
            versionFolder.clear();
            return HRESULT_FROM_WIN32(dError);
        }

        versionFolder = pLatestVersionFolder->GetVersionString();

        return S_OK;
    }

    /*
     * This function determines the latest profiler's path for LoadLibrary() call.
     */
    static HRESULT LoadProfiler()
    {
        HRESULT hr = S_OK;

        //
        // Determine "Program Files" folder
        //

        LPCWSTR programFilesVar = _T("ProgramFiles");

#ifndef _WIN64
        // WoW64 = Windows (32bit) on Windows 64bit
        BOOL bIsWow64 = FALSE;
        IfFailRet_Proxy(IsWow64(&bIsWow64));
        if (bIsWow64)
        {
            programFilesVar = _T("ProgramFiles(x86)");
        }
#endif

        WCHAR wszProfilerPath[MAX_PATH];
        ZeroMemory(wszProfilerPath, MAX_PATH);
        if (!GetEnvironmentVariable(programFilesVar, wszProfilerPath, MAX_PATH))
        {
            CProxyLogging::LogError(_T("dllmain::LoadProfiler - Unable to resolve environment variable: %s"), programFilesVar);
            return E_UNEXPECTED;
        }

        //
        // Set CIE folder
        //

        IfFailRet_Proxy(PathCchAppend(wszProfilerPath, MAX_PATH, instrumentationEngineFolder));

        //
        // Determine Version folder
        //

        WCHAR wszSpecificVersion[MAX_PATH];
        bool useSpecificVersion = GetEnvironmentVariable(useSpecificVersionVar, wszSpecificVersion, 1) > 0;

        // We provide the option to use a specific version of the proxy in cases of debugging/testing scenarios.
        // This bypasses the proxy's default find-and-use-latest behavior.
        std::wstring versionFolder;
        if (useSpecificVersion)
        {
            CProxyLogging::LogMessage(_T("dllmain::LoadProfiler - Resolving specific version: %s"), wszSpecificVersion);
            versionFolder = wszSpecificVersion;
            BOOL hasProfiler = FALSE;
            if (FAILED(HasProfilerDll(wszProfilerPath, wszSpecificVersion, &hasProfiler)) || !hasProfiler)
            {
                CProxyLogging::LogError(_T("dllmain::LoadProfiler - Unable to find folder version: %s"), wszSpecificVersion);
                return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
            }
        }
        else
        {
            IfFailRet_Proxy(GetLatestVersionFolder(wszProfilerPath, versionFolder));
        }

        IfFailRet_Proxy(PathCchAppend(wszProfilerPath, MAX_PATH, versionFolder.c_str()));

        //
        // Determine and Load Profiler
        //

        IfFailRet_Proxy(PathCchAppend(wszProfilerPath, MAX_PATH, profilerRelativeFileName));

        CProxyLogging::LogMessage(_T("dllmain::LoadProfiler - Loading profiler from path: '%s'"), wszProfilerPath);

        g_hProfiler = ::LoadLibrary(wszProfilerPath);

        if (nullptr == g_hProfiler)
        {
            CProxyLogging::LogError(_T("dllmain::LoadProfiler - Unable to load profiler: '%s'"), wszProfilerPath);
            return E_FAIL;
        }

        return S_OK;
    }

    /*
     * The proxy searches for the latest [VERSION] folder in order to pick up the
     * InstrumentationEngine profiler dll and forwards the DllGetClassObject call.
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

#ifdef DEBUG
        WCHAR wszEnvVar[MAX_PATH];
        if (GetEnvironmentVariable(_T("MicrosoftInstrumentationEngine_DebugWait"), wszEnvVar, MAX_PATH) > 0)
        {
            while (!IsDebuggerPresent())
            {
                Sleep(100);
            }
        }
#endif
        // Guard against g_hProfiler if it's currently being loaded.
        CCriticalSectionHolder lock(&g_criticalSection);

        bool exceptionOccurred = false;
        try
        {
            CProxyLogging::Initialize();

            DWORD pid = GetCurrentProcessId();
            CProxyLogging::LogMessage(_T("dllmain::DllGetClassObject - Loading Proxy from Process: %u"), pid);

            if (g_hProfiler == nullptr)
            {
                hr = LoadProfiler();
            }

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

            if (FAILED(hr))
            {
                // Log error but do not fail since proxy shouldn't impact the process.
                CProxyLogging::LogError(_T("dllmain::DllGetClassObject - Failed to load profiler with error code 0x%x"), hr);
            }
        }
        catch (...)
        {
            // TODO log exception.
            CProxyLogging::Shutdown();
            exceptionOccurred = true;
        }

        if (!exceptionOccurred)
        {
            CProxyLogging::Shutdown();
        }

        return S_OK;
    }

    __control_entrypoint(DllExport)
    STDAPI DLLEXPORT(DllCanUnloadNow, 0)(void)
    {
        HRESULT hr = S_OK;

        CCriticalSectionHolder lock(&g_criticalSection);

        if (g_hProfiler != nullptr)
        {
            LPFNCANUNLOADNOW dllCanUnloadNow = (LPFNCANUNLOADNOW)::GetProcAddress(g_hProfiler, "DllCanUnloadNow");
            if (dllCanUnloadNow != nullptr)
            {
                hr = dllCanUnloadNow();
                if (hr == S_OK)
                {
                    ::FreeLibrary(g_hProfiler);
                    g_hProfiler = nullptr;
                }
            }
        }

        return hr;
    }
}