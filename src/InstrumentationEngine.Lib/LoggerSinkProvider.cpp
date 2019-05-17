#include "stdafx.h"
#include "LoggerSinkProvider.h"

#ifndef PLATFORM_UNIX
#include "DebugLoggerSink.h"
#include "EventLoggerSink.h"
#endif
#include "FileLoggerSink.h"
#include "HostLoggerSink.h"

using namespace MicrosoftInstrumentationEngine;
using namespace std;

HRESULT CLoggerSinkProvider::GetSinks(vector<shared_ptr<ILoggerSink>>& sinks)
{
#ifndef PLATFORM_UNIX
    sinks.push_back(make_shared<CDebugLoggerSink>());

    if (pEventSinkOverride)
    {
        sinks.push_back(pEventSinkOverride);
    }
    else
    {
        sinks.push_back(make_shared<CEventLoggerSink>());
    }
#endif
    if (pFileSinkOverride)
    {
        sinks.push_back(pFileSinkOverride);
    }
    else
    {
        sinks.push_back(make_shared<CFileLoggerSink>());
    }

    sinks.push_back(make_shared<CHostLoggerSink>());

    return S_OK;
}

#ifndef PLATFORM_UNIX
void CLoggerSinkProvider::SetEventSink(const shared_ptr<ILoggerSink>& pSink)
{
    pEventSinkOverride = pSink;
}
#endif

void CLoggerSinkProvider::SetFileSink(const shared_ptr<ILoggerSink>& pSink)
{
    pFileSinkOverride = pSink;
}