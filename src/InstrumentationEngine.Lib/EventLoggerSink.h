// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "LoggerService.h"
#include "LoggerSink.h"
#include "../Common.Lib/EventLoggingBase.h"

namespace MicrosoftInstrumentationEngine
{
    class CEventLoggerSink :
        public ILoggerSink,
        public CEventLoggingBase
    {
    private:
        CInitOnce m_initEventSource;

    public:
        CEventLoggerSink();
        ~CEventLoggerSink();

        // ILoggerSink Members
    protected:
        HRESULT Initialize(_In_ CLoggerService* pLogging) override;

        void LogMessage(_In_ LPCWSTR wszMessage) override;

        void LogError(_In_ LPCWSTR wszMessage) override;

        void LogDumpMessage(_In_ LPCWSTR wszMessage) override;

        HRESULT Reset(_In_ LoggingFlags defaultFlags, _Out_ LoggingFlags* pEffectiveFlags) override;

        HRESULT Shutdown() override;

    };
}