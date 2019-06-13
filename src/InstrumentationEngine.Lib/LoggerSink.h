// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

namespace MicrosoftInstrumentationEngine
{
    class CLoggerService;

    // Base class for individual logging targets to implement. This is only meant to be extended
    // by internal logging sinks and allow for separation of concerns, not as an extensibility mechanism.
    class ILoggerSink
    {
        friend CLoggerService;

    protected:
        // Initializes the sink with the logger service instance
        virtual HRESULT Initialize(_In_ CLoggerService* pLogging) = 0;

        // Log a message to the logging target.
        virtual void LogMessage(_In_ LPCWSTR wszMessage) = 0;

        // Log an error to the logging target.
        virtual void LogError(_In_ LPCWSTR wszMessage) = 0;

        // Log a dump message to the logging target.
        virtual void LogDumpMessage(_In_ LPCWSTR wszMessage) = 0;

        // Called when a logging aspect has changed (e.g. flags, host, etc) and allows the sink
        // to report which flags it cares to handle.
        virtual HRESULT Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags) = 0;

        // Called when the sink will no longer be used and can release resources.
        virtual HRESULT Shutdown() { return S_OK; };
    };
}