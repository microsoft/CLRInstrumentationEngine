// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "refcount.h"
// TODO:
// Clean this up. It was pulled from Debugger\inc and Debugger\common in order to get leak detection on
// ref counted objects. I didn't just use the compiled lib becase it isn't clear to me where this code
// is eventually going to live and if it turns out to be on a CDM server, the debugger lib won't exist.

namespace CommonLib
{

#ifdef _UNICODE
#define TRACE TRACEW
#else
#define TRACE TRACEA
#endif


#if defined(DEBUG) & defined(_WINDOWS_)
    bool g_fDisplayTrace = true;
    int  g_TraceIndent = 0;

    #define TRACEA DisplayTraceA
    #define TRACEW DisplayTraceW

    extern bool g_fDisplayTrace;
    extern int  g_TraceIndent;

    void _cdecl DisplayTraceA(LPCSTR pszFormat, ...);
    void _cdecl DisplayTraceW(LPCWSTR pszFormat, ...);
    void _cdecl IndentTrace(int indent);

#else // !(DEBUG & WINDOWS)

#define TRACEA
#define TRACEW
#define IndentTrace

#endif


#if defined(DEBUG) & defined(_WINDOWS_)
    void _cdecl IndentTrace(int indent)
    {
        g_TraceIndent += indent;
    }

    void _cdecl DisplayTraceW(LPCWSTR pszFormat, ...)
    {
        if (g_fDisplayTrace)
        {
            // use std::array, so that we don't need to calculate size.
            std::array<WCHAR, 1024> szTrace;

            int offset = 0;

            if (g_TraceIndent > 0)
            {
                // note: don't use the 'min' macro here because we would like to
                // avoid linking to the CLR PAL.
                int arrayLen = (int)szTrace.size() - 1;
                offset = (g_TraceIndent < arrayLen) ? g_TraceIndent : arrayLen;

                for (int i = 0; i < offset; i++)
                {
                    szTrace[i] = ' ';
                }
            }

            va_list marker;
            va_start(marker, pszFormat);
            _vsnwprintf_s(szTrace.data() + offset, szTrace.size() - offset, _TRUNCATE, pszFormat, marker);
            wcscat_s(szTrace.data(), szTrace.size(), _T("\r\n"));
            OutputDebugStringW(szTrace.data());
        }
    }

    // ----------------------------------------------------------------------------
    // CRefRBMap
    class CRefRBMap : public std::map<CRefCount*, ULONG> {};
#endif

    // ----------------------------------------------------------------------------
    // CRefCount

#if defined(DEBUG) & defined(_WINDOWS_)
    DWORD CRefCount::g_dwRecorderOptions;     //Recording is optional
    bool CRefCount::g_fRecordingInitialized = false;
    CRefRBMap CRefCount::g_map;
    std::recursive_mutex CRefCount::g_mutex;
    using recursive_lock = std::lock_guard<std::recursive_mutex>;

    void CRefCount::InitRecorder(DWORD dwOptions)
    {
        // Add an environment variable hook to disable ref-recording, since this can sometimes be really slow.
        
        g_dwRecorderOptions = dwOptions;
        if (g_dwRecorderOptions & EnableRecorder)
        {
            g_fRecordingInitialized = true;
        }
    }

    void CRefCount::TerminateRecorder(void)
    {
        if (g_fRecordingInitialized)
        {
            //VSASSERT(g_map.IsEmpty(), "Object recorder: Objects when the object recorder was terminated.");
            for (auto next : g_map)
            {
                TRACE(_T("Object \"%s\" (%lxh) was leaked (%lu)."), next.first->m_pszCRefCountClassName, next.first, next.second);
            }
            g_map.clear();
            g_fRecordingInitialized = false;
        }
    }

    void CRefCount::RecordCreation(CRefCount* pCRefCount)
    {
        recursive_lock lock(g_mutex);

        auto found = g_map.find(pCRefCount);
        if (found != g_map.end())
        {
            TRACE(_T("Object \"%s\" (%lxh) destroyed without recorder being notified."), pCRefCount->m_pszCRefCountClassName, pCRefCount);
        }

        g_map[pCRefCount] = 1;

    }

    void CRefCount::RecordDestruction(CRefCount* pCRefCount)
    {
        recursive_lock lock(g_mutex);

        g_map.erase(pCRefCount);

        //VSASSERT(fFound, "Object recorder: Object created without the recorder being notified.");

    }

    void CRefCount::RecordAddRef(CRefCount* pCRefCount)
    {
        recursive_lock lock(g_mutex);

        BOOL fFound = FALSE;
        auto found = g_map.find(pCRefCount);
        if (found != g_map.end())
        {
            g_map[pCRefCount] = found->second + 1;
        }

        //VSASSERT(fFound, "Object recorder: Object created without the recorder being notified.");
    }

    void CRefCount::RecordRelease(CRefCount* pCRefCount)
    {
        recursive_lock lock(g_mutex);

        BOOL fFound = FALSE;
        auto found = g_map.find(pCRefCount);
        if (found != g_map.end())
        {
            ULONG count = found->second - 1;
            if (count == 0)
            {
                g_map.erase(found);
            }
            else
            {
                g_map[pCRefCount] = count;
            }
        }

        //VSASSERT(fFound, "Object recorder: Object created without the recorder being notified.");
    }

    void CRefCount::DumpRefCount(void)
    {
        if (g_fRecordingInitialized)
        {
            recursive_lock lock(g_mutex);

            if (!g_map.empty())
            {
                TRACE(_T(">> Begin ref count dump.  Leaked objects are also available in the watch window by evaluating g_map. <<"));

                for (auto next : g_map)
                {
                    TRACE(_T(">> Object \"%s\" (0x%p) contains ref-count 0x%lu."), next.first->m_pszCRefCountClassName, next.first, next.second);
                }

                TRACE(_T(">> End ref count dump <<"));
            }
        }
    }

#endif // DEBUG & WINDOWS

    // ----------------------------------------------------------------------------
    // CModuleRefCount

    std::atomic<uint32_t> CModuleRefCount::s_ulcModuleRef(0);

    // ----------------------------------------------------------------------------
    // module ref count support

    ULONG CModuleRefCount::GetModuleUsage(void)
    {
        return (ULONG)s_ulcModuleRef;
    }

}