// Copyright (c) Microsoft Corporation. All rights reserved.
// 

// dllmain.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"

namespace ProfilerProxy
{
    // Handle to profiler module.
    HMODULE hProfiler = nullptr;

    // Critical section object which provides synchronization when accessing hProfiler.
    CRITICAL_SECTION criticalSection;

    static constexpr const WCHAR* instrumentationEngineFolder = _T("Microsoft CLR Instrumentation Engine");
    static constexpr const WCHAR* useDebugVar = _T("InstrumentationEngineProxy_UseDebug");
    static constexpr const WCHAR* usePreviewVar = _T("InstrumentationEngineProxy_UsePreview");
    static constexpr const WCHAR* useSpecificVersionVar = _T("InstrumentationEngineProxy_UseSpecificVersion");

#ifdef _WIN64
    static constexpr const WCHAR* profilerRelativeFileName = _T("Instrumentation64\\MicrosoftInstrumentationEngine_x64.dll");
#else
    static constexpr const WCHAR* profilerRelativeFileName = _T("Instrumentation32\\MicrosoftInstrumentationEngine_x86.dll");
#endif

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

    /*
     * This function returns whether the profiler dll exists.
     */
    static HRESULT HasProfilerDll(_In_ LPCWSTR wszProfilerPath, _In_ LPCWSTR wszVersionFolder, _Out_ BOOL* hasProfiler)
    {
        HRESULT hr = S_OK;
        *hasProfiler = FALSE;

        WIN32_FIND_DATA findFileData;
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
        if (error == 0)
        {
            fclose(file);
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

#pragma region ConfigurationFlags

        WCHAR wszUseDebug[2];
        ZeroMemory(wszUseDebug, 2);
        BOOL useDebug = GetEnvironmentVariable(useDebugVar, wszUseDebug, 2) > 0 && wcscmp(wszUseDebug, _T("1")) == 0;

        WCHAR wszUsePreviewVal[2];
        ZeroMemory(wszUsePreviewVal, 2);
        BOOL usePreview = GetEnvironmentVariable(usePreviewVar, wszUsePreviewVal, 2) > 0 && wcscmp(wszUsePreviewVal, _T("1")) == 0;

#pragma endregion

        versionFolder.clear();
        
        WIN32_FIND_DATA findFileData;
        std::wstring profilerPathFilter = std::wstring(wszProfilerPath);
        profilerPathFilter += _T("*"); // FilePattern

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
            InstrumentationEngineVersion folderVersion(findFileData.cFileName);
            if (!folderVersion.IsValid())
            {
                continue;
            }

            // Skip any versions that are filtered by Configuration flags
            if (folderVersion.IsDebug() != useDebug ||
                (!usePreview && folderVersion.IsPreview()))
            {
                continue;
            }

            // Skip any folders without profiler
            BOOL hasProfiler;
            IfFailRetNoLog(HasProfilerDll(wszProfilerPath, folderVersion.c_str(), &hasProfiler));
            if (!hasProfiler)
            {
                continue;
            }

            if (!latestVersionFolder.IsValid() ||
                latestVersionFolder.Compare(folderVersion) < 0)
            {
                latestVersionFolder = folderVersion;
            }

        } while (FindNextFile(hSearchHandle, &findFileData));

        // Only return if no VersionFolder found
        DWORD dError = GetLastError();
        if (!latestVersionFolder.IsValid() && dError == ERROR_NO_MORE_FILES ||
            dError != 0)
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
        HRESULT __stdcall DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ PVOID* ppObj)
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

#pragma region ProgramFilesFolder

        LPCWSTR programFilesVar = _T("ProgramFiles");

#ifndef _WIN64
        // WoW64 = Windows (32bit) on Windows 64bit
        BOOL fIsOs64bit = FALSE;
        if (IsWow64Process(GetCurrentProcess(), &fIsOs64bit) && fIsOs64bit)
        {
            programFilesVar = _T("ProgramFiles(x86)");
        }
#endif

        WCHAR wszProfilerPath[MAX_PATH];
        ZeroMemory(wszProfilerPath, MAX_PATH);
        if (GetEnvironmentVariable(programFilesVar, wszProfilerPath, MAX_PATH) > 0)
        {
            return E_UNEXPECTED;
        }

#pragma endregion 

#pragma region CIEFolder

        IfFailRetNoLog(PathCchAppend(wszProfilerPath, MAX_PATH, instrumentationEngineFolder));

#pragma endregion

#pragma region VersionFolder

        WCHAR specificVersionStr[MAX_PATH];
        bool useSpecificVersion = GetEnvironmentVariable(useSpecificVersionVar, specificVersionStr, 1) > 0;

        // Find the latest version
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

#pragma endregion

#pragma region ProfilerLoad

        IfFailRetNoLog(PathCchAppend(wszProfilerPath, MAX_PATH, profilerRelativeFileName));

        ::EnterCriticalSection(&criticalSection);

        hProfiler = ::LoadLibrary(wszProfilerPath);
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

#pragma endregion

        return S_OK;
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

}