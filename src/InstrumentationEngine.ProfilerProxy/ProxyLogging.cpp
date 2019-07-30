// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ProxyLogging.h"
#include "../InstrumentationEngine.ProfilerProxy.Lib/EventLogger.h"

#include <atomic>

using namespace ProfilerProxy;
using namespace CommonLib;

CInitOnce CProxyLogging::s_initialize([]() { return InitializeCore(); });
CSingleton<CEventLogger> CProxyLogging::s_eventLogger;
std::atomic_size_t CProxyLogging::s_initCount(0);

// static
HRESULT CProxyLogging::Initialize()
{
    HRESULT hr = s_initialize.Get();

    if (s_initialize.IsSuccessful())
    {
        ++s_initCount;
    }

    return hr;
}

// static
HRESULT CProxyLogging::InitializeCore()
{
    return s_eventLogger.Get()->Initialize();
}

// static
void CProxyLogging::LogMessage(_In_ const WCHAR* wszMessage, ...)
{
    IfNotInitRet(s_initialize);

    va_list argptr;
    va_start(argptr, wszMessage);
    s_eventLogger.Get()->LogMessage(wszMessage, argptr);
    va_end(argptr);
}

// static
void CProxyLogging::LogWarning(_In_ const WCHAR* wszMessage, ...)
{
    IfNotInitRet(s_initialize);

    va_list argptr;
    va_start(argptr, wszMessage);
    s_eventLogger.Get()->LogWarning(wszMessage, argptr);
    va_end(argptr);
}

// static
void CProxyLogging::LogError(_In_ const WCHAR* wszError, ...)
{
    IfNotInitRet(s_initialize);

    va_list argptr;
    va_start(argptr, wszError);
    s_eventLogger.Get()->LogError(wszError, argptr);
    va_end(argptr);
}


// static
HRESULT CProxyLogging::Shutdown()
{
    IfNotInitRetUnexpected(s_initialize);

    if (0 == --s_initCount)
    {
        HRESULT hr = S_OK;
        IfFailRetNoLog(s_eventLogger.Get()->Shutdown());
        s_initialize.Reset();
    }
    return S_OK;
}
