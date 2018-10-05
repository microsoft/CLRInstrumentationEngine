// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"

#include "refcount.h"
#include "intrin.h"

// TODO:
// Clean this up. It was pulled from Debugger\inc and Debugger\common in order to get leak detection on
// ref counted objects. I didn't just use the compiled lib becase it isn't clear to me where this code
// is eventually going to live and if it turns out to be on a CDM server, the debugger lib won't exist.

namespace MicrosoftInstrumentationEngine
{
#pragma intrinsic(_InterlockedOr)

#ifdef _UNICODE
#define TRACE TRACEW
#else
#define TRACE TRACEA
#endif


#ifdef DEBUG
    bool g_fDisplayTrace = true;
    int  g_TraceIndent = 0;

    #define TRACEA DisplayTraceA
    #define TRACEW DisplayTraceW

    extern bool g_fDisplayTrace;
    extern int  g_TraceIndent;

    void _cdecl DisplayTraceA(LPCSTR pszFormat, ...);
    void _cdecl DisplayTraceW(LPCWSTR pszFormat, ...);
    void _cdecl IndentTrace(int indent);

#else // !DEBUG

#define TRACEA
#define TRACEW
#define IndentTrace

#endif

#ifdef DEBUG
    void _cdecl IndentTrace(int indent)
    {
        g_TraceIndent += indent;
    }

    void _cdecl DisplayTraceW(LPCWSTR pszFormat, ...)
    {
        if (g_fDisplayTrace)
        {
            WCHAR szTrace[1024];

            int offset = 0;

            if (g_TraceIndent > 0)
            {
                offset = min(g_TraceIndent, (int)ARRAYSIZE(szTrace) - 1);

                for (int i = 0; i < offset; i++)
                {
                    szTrace[i] = ' ';
                }
            }

            va_list marker;
            va_start(marker, pszFormat);
            _snwprintf_s(szTrace + offset, ARRAYSIZE(szTrace) - offset, _TRUNCATE, pszFormat, marker);
            wcscat_s(szTrace, ARRAYSIZE(szTrace), _T("\r\n"));
            OutputDebugStringW(szTrace);
        }
    }
#endif

    // ----------------------------------------------------------------------------
    // CRefRBMap
#ifdef DEBUG
    class CRefRBMap : public ATL::CRBMap<CRefCount*, ULONG> {};
#endif

    // ----------------------------------------------------------------------------
    // CRefCount

#ifdef DEBUG
    DWORD CRefCount::g_dwRecorderOptions;     //Recording is optional
    bool CRefCount::g_fRecordingInitialized = false;
    CRefRBMap CRefCount::g_map;
    CRITICAL_SECTION CRefCount::g_crst;

    void CRefCount::InitRecorder(DWORD dwOptions)
    {
        //Add an environment variable hook to disable ref-recording, since this can sometimes be really slow.
        if (GetEnvironmentVariable(_T("DisableRefRecording"), nullptr, 0) > 0)
        {
            dwOptions &= ~EnableRecorder;
        }

        g_dwRecorderOptions = dwOptions;
        if (g_dwRecorderOptions & EnableRecorder)
        {
            InitializeCriticalSection(&g_crst);
            g_fRecordingInitialized = true;
        }
    }

    void CRefCount::TerminateRecorder(void)
    {
        if (g_fRecordingInitialized)
        {
            //VSASSERT(g_map.IsEmpty(), "Object recorder: Objects when the object recorder was terminated.");

            for (POSITION nextPosition = g_map.GetHeadPosition(); nextPosition != NULL;)
            {
                CRefRBMap::CPair* pPair = g_map.GetNext(nextPosition);
                TRACE(_T("Object \"%s\" (%lxh) was leaked (%lu)."), pPair->m_key->m_pszCRefCountClassName, pPair->m_key, pPair->m_value);
            }

            g_map.RemoveAll();

            DeleteCriticalSection(&g_crst);
            g_fRecordingInitialized = false;
        }
    }

    void CRefCount::RecordCreation(CRefCount* pCRefCount)
    {
        if (g_fRecordingInitialized)
        {
            EnterCriticalSection(&g_crst);

            CRefRBMap::CPair* pPair = g_map.Lookup(pCRefCount);
            if (pPair)
            {
                //VSASSERT(false, "Object recorder: Object destroyed without the recorder being notified.");
                TRACE(_T("Object \"%s\" (%lxh) destroyed without recorder being notified."), pCRefCount->m_pszCRefCountClassName, pCRefCount);
            }

            g_map.SetAt(pCRefCount, 1);

            LeaveCriticalSection(&g_crst);
        }
    }

    void CRefCount::RecordDestruction(CRefCount* pCRefCount)
    {
        if (g_fRecordingInitialized)
        {
            EnterCriticalSection(&g_crst);

            bool fFound = g_map.RemoveKey(pCRefCount);

            //VSASSERT(fFound, "Object recorder: Object created without the recorder being notified.");

            LeaveCriticalSection(&g_crst);
        }
    }

    void CRefCount::RecordAddRef(CRefCount* pCRefCount)
    {
        if (g_fRecordingInitialized)
        {
            EnterCriticalSection(&g_crst);

            BOOL fFound = FALSE;
            CRefRBMap::CPair* pPair = g_map.Lookup(pCRefCount);
            if (pPair)
            {
                pPair->m_value++;
                fFound = TRUE;
            }

            //VSASSERT(fFound, "Object recorder: Object created without the recorder being notified.");

            LeaveCriticalSection(&g_crst);
        }
    }

    void CRefCount::RecordRelease(CRefCount* pCRefCount)
    {
        if (g_fRecordingInitialized)
        {
            EnterCriticalSection(&g_crst);

            BOOL fFound = FALSE;
            CRefRBMap::CPair* pPair = g_map.Lookup(pCRefCount);
            if (pPair)
            {
                pPair->m_value--;

                //VSASSERT(pPair->m_value != -1, "Object recorder: Object released too many times.");

                if (pPair->m_value == 0)
                {
                    g_map.RemoveAt(pPair);
                }
                fFound = TRUE;
            }

            //VSASSERT(fFound, "Object recorder: Object created without the recorder being notified.");

            LeaveCriticalSection(&g_crst);
        }
    }

    void CRefCount::DumpRefCount(void)
    {
        if (g_fRecordingInitialized)
        {
            EnterCriticalSection(&g_crst);

            if (!g_map.IsEmpty())
            {
                TRACE(_T(">> Begin ref count dump.  Leaked objects are also available in the watch window by evaluating g_map. <<"));

                for (POSITION nextPosition = g_map.GetHeadPosition(); nextPosition != NULL;)
                {
                    CRefRBMap::CPair* pPair = g_map.GetNext(nextPosition);
                    TRACE(_T(">> Object \"%s\" (0x%p) contains ref-count 0x%lu."), pPair->m_key->m_pszCRefCountClassName, pPair->m_key, pPair->m_value);
                }

                TRACE(_T(">> End ref count dump <<"));
            }

            LeaveCriticalSection(&g_crst);
        }
    }

#endif // DEBUG

    // ----------------------------------------------------------------------------
    // CModuleRefCount

    ULONG CModuleRefCount::s_ulcModuleRef = 0;

    // ----------------------------------------------------------------------------
    // module ref count support

    ULONG CModuleRefCount::GetModuleUsage(void)
    {
        return (ULONG)_InterlockedOr((PLONG)&s_ulcModuleRef, 0);
    }

}