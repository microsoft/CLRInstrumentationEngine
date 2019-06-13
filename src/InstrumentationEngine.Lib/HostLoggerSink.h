// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "LoggerService.h"
#include "LoggerSink.h"

namespace MicrosoftInstrumentationEngine
{
    class CHostLoggerSink :
        public ILoggerSink
    {
    private:
        CLoggerService* m_pLogging;
        CComPtr<IProfilerManagerLoggingHost> m_pLoggingHost;

        // ILoggerSink Members
    protected:
        HRESULT Initialize(_In_ CLoggerService* pLogging) override;

        void LogMessage(_In_ LPCWSTR wszMessage) override;

        void LogError(_In_ LPCWSTR wszMessage) override;

        void LogDumpMessage(_In_ LPCWSTR wszMessage) override;

        HRESULT Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags) override;
    };
}