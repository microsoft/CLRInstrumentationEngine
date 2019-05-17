#pragma once

#include "LoggerSink.h"

namespace MicrosoftInstrumentationEngine
{
    class CLoggerSinkProvider
    {
    private:
#ifndef PLATFORM_UNIX
        std::shared_ptr<ILoggerSink> pEventSinkOverride;
#endif
        std::shared_ptr<ILoggerSink> pFileSinkOverride;

    public:
        HRESULT GetSinks(std::vector<std::shared_ptr<ILoggerSink>>& sinks);

        // Used for testing
    public:
#ifndef PLATFORM_UNIX
        void SetEventSink(const std::shared_ptr<ILoggerSink>& pSink);
#endif
        void SetFileSink(const std::shared_ptr<ILoggerSink>& pSink);
    };
}