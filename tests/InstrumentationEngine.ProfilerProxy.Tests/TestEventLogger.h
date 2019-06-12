// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "EventLogger.h"

using namespace ProfilerProxy;

namespace InstrumentationEngineProfilerProxyTests
{
    class TestEventLogger :
        public CEventLogger
    {
    private:
        std::vector<EventLogItem> m_entries;

    public:
        void ClearEntries()
        {
            m_entries.clear();
        }

        const std::vector<EventLogItem> GetEntries()
        {
            return m_entries;
        }

        void LogEvent(_In_ const EventLogItem& tsEntry) override
        {
            // Sleep some arbitrary amount of time.
            Sleep(m_entries.size() % 25); // ms

            m_entries.push_back(tsEntry);
        }
    };
}