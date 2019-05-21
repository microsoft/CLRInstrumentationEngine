#pragma once

#include "stdafx.h"

namespace MicrosoftInstrumentationEngine
{
    class CLoggingWrapper :
        public IProfilerManagerLogging,
        public CModuleRefCount
    {
        // IUnknown Members
    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CLoggingWrapper);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CLoggingWrapper);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IProfilerManagerLogging*>(this),
                riid,
                ppvObject
            );
        }

        // IProfilerManagerLogging Members
    public:
        // If trace logging in enabled in the profiler manager, trace messages are sent to the
        // profiler manager through this function.
        STDMETHOD(LogMessage)(_In_ const WCHAR* wszMessage) override
        {
            CLogging::LogMessage(wszMessage);
            return S_OK;
        }

        // Errors detected during profiling will be sent to the host through this method
        STDMETHOD(LogError)(_In_ const WCHAR* wszError) override
        {
            CLogging::LogError(wszError);
            return S_OK;
        }

        // If instrumentation result tracing is enabled, the detailed results of each instrumented
        // method will be sent to the profiler manager host through this method.
        STDMETHOD(LogDumpMessage)(_In_ const WCHAR* wszMessage) override
        {
            CLogging::LogDumpMessage(wszMessage);
            return S_OK;
        }

        // Called to cause logging to be written to the debug output port (via DebugOutputString) as well
        // as to the host.
        STDMETHOD(EnableDiagnosticLogToDebugPort)(_In_ BOOL enable) override
        {
            CLogging::EnableDiagnosticLogToDebugPort(enable != 0);
            return S_OK;
        }

        // Allows instrumentation methods and hosts to ask for the current logging flags
        STDMETHOD(GetLoggingFlags)(_Out_ LoggingFlags* pLoggingFlags) override
        {
            return CLogging::GetLoggingFlags(pLoggingFlags);
        }

        // Allows instrumentation methods and hosts to modify the current logging flags
        STDMETHOD(SetLoggingFlags)(_In_ LoggingFlags loggingFlags) override
        {
            return CLogging::SetLoggingFlags(loggingFlags);
        }
    };
}