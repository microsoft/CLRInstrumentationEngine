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
        STDMETHOD(LogMessage)(_In_ const WCHAR* wszMessage) override
        {
            CLogging::LogMessage(wszMessage);
            return S_OK;
        }

        STDMETHOD(LogError)(_In_ const WCHAR* wszError) override
        {
            CLogging::LogError(wszError);
            return S_OK;
        }

        STDMETHOD(LogDumpMessage)(_In_ const WCHAR* wszMessage) override
        {
            CLogging::LogDumpMessage(wszMessage);
            return S_OK;
        }

        STDMETHOD(EnableDiagnosticLogToDebugPort)(_In_ BOOL enable) override
        {
            CLogging::SetLogToDebugPort(enable != 0);
            return S_OK;
        }

        STDMETHOD(GetLoggingFlags)(_Out_ LoggingFlags* pLoggingFlags) override
        {
            return CLogging::GetLoggingFlags(pLoggingFlags);
        }

        STDMETHOD(SetLoggingFlags)(_In_ LoggingFlags loggingFlags) override
        {
            return CLogging::SetLoggingFlags(loggingFlags);
        }
    };
}