// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "../InstrumentationEngine.ProfilerProxy.Lib/EventLogger.h"
#include <atomic>
#include "../Common.Lib/InitOnce.h"

// CLogging uses *wprintf* methods for formatting strings.
#ifdef PLATFORM_UNIX
// Use %S for WCHAR* on Linux (which is char16_t*) since *wprintf* expects strings to be wchar_t*.
// The capital S specifier means "format the string using the width that is opposite of the formatting method".
// The capital S specifier is appropriate since WCHAR* on Linux is smaller than the *wprintf* width expectation.
#define WCHAR_SPEC _T("%S")
#else
// Use %s for WCHAR* on Windows (which is wchar_t*)
// The lowercase s specifier is appropriate since WCHAR* on Windows matches *wprintf* width expectation.
#define WCHAR_SPEC _T("%s")
#endif

namespace ProfilerProxy
{
    class CProxyLogging
    {
    private:
        static CInitOnce s_initialize;
        static CSingleton<CEventLogger> s_eventLogger;
        static std::atomic_size_t s_initCount;

    public:
        static HRESULT Initialize();
        static void LogMessage(_In_ const WCHAR* szMessage, ...);
        static void LogWarning(_In_ const WCHAR* szMessage, ...);
        static void LogError(_In_ const WCHAR* szError, ...);

        static HRESULT Shutdown();

    private:
        static HRESULT InitializeCore();
    };
}


