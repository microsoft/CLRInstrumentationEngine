// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "InstrumentationEngineVersion.h"
#include "TestEventLogger.h"

using namespace ProfilerProxy;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InstrumentationEngineProfilerProxyTests
{
    TEST_CLASS(EventLoggerTests)
    {
    private:
        void AssertSucceeded(HRESULT hr)
        {
            Assert::IsTrue(SUCCEEDED(hr));
        }

        void AssertFailed(HRESULT hr)
        {
            Assert::IsTrue(FAILED(hr));
        }

    public:
        // Tests that the EventLogger can write to the event source
        TEST_METHOD(EventLoggerCanWriteWaitForDrain)
        {
            TestEventLogger eventLogger;

            // Number of items to write to event log
            const int nCount = 100;

            // Write errors to log
            for (int i = 0; i < nCount; i++)
            {
                tstring tsError(L"Error");
                tsError.append(to_wstring(i));
                eventLogger.LogError(tsError.c_str());
            }

            // Check log counts are correct.
            // EventLogger should not have processed all items yet.
            const vector<EventLogItem> entries = eventLogger.GetEntries();
            Assert::AreNotEqual((size_t)nCount, entries.size());

            // Check log counts are correct.
            // Shutdown will trigger EventLogger to flush all events.
            AssertSucceeded(eventLogger.Shutdown());
            const vector<EventLogItem> entriesAfterShutdown = eventLogger.GetEntries();
            Assert::AreEqual((size_t)nCount, entriesAfterShutdown.size());
        }

        // Tests that the EventLogger can write all message types.
        TEST_METHOD(EventLoggerCanWriteAllTypes)
        {
            TestEventLogger eventLogger;

            // Number of items to write to event log
            const int nCount = 99; // divisible by 3

            // Write errors to log
            for (int i = 0; i < nCount; i++)
            {
                tstring tsError(L"Error");
                tstring tsWarning(L"Warning");
                tstring tsMessage(L"Message");

                switch (i % 3)
                {
                case 0:
                    // Log Error
                    tsError.append(to_wstring(i));
                    eventLogger.LogError(tsError.c_str());
                    break;
                case 1:
                    // Log Warning
                    tsWarning.append(to_wstring(i));
                    eventLogger.LogWarning(tsWarning.c_str());
                    break;
                case 2:
                    // Log Message
                    tsMessage.append(to_wstring(i));
                    eventLogger.LogMessage(tsMessage.c_str());
                    break;
                }
            }

            // Check log counts are correct.
            // Shutdown will trigger EventLogger to flush all events.
            AssertSucceeded(eventLogger.Shutdown());
            const vector<EventLogItem> entriesAfterShutdown = eventLogger.GetEntries();
            Assert::AreEqual((size_t)nCount, entriesAfterShutdown.size());

            int errorCount = 0;
            int warningCount = 0;
            int messageCount = 0;
            for (int i = 0; i < nCount; i++)
            {
                switch (entriesAfterShutdown[i].wEventType)
                {
                case EVENTLOG_ERROR_TYPE:
                    errorCount++;
                    break;
                case EVENTLOG_WARNING_TYPE:
                    warningCount++;
                    break;
                case EVENTLOG_INFORMATION_TYPE:
                    messageCount++;
                    break;
                }
            }

            const int expectedCount = nCount / 3;
            Assert::AreEqual(expectedCount, errorCount);
            Assert::AreEqual(expectedCount, warningCount);
            Assert::AreEqual(expectedCount, messageCount);
        }

        // Tests EventLogger cannot write after shutdown
        TEST_METHOD(EventLoggerCannotWriteAfterShutdown)
        {
            TestEventLogger eventLogger;

            eventLogger.LogError(L"Error");

            // Check log counts are correct.
            // Shutdown will trigger EventLogger to flush all events.
            AssertSucceeded(eventLogger.Shutdown());
            const vector<EventLogItem> entriesAfterShutdown = eventLogger.GetEntries();
            Assert::AreEqual((size_t)1, entriesAfterShutdown.size());

            eventLogger.LogError(L"Error");

            // Give event logger time to process messages
            Sleep(10); // ms

            const vector<EventLogItem> entriesAfterShutdown2 = eventLogger.GetEntries();
            Assert::AreEqual((size_t)1, entriesAfterShutdown2.size());
        }

        // Tests multiple EventLoggers do not affect each other.
        TEST_METHOD(MultipleEventLoggersCanWriteSimultaneously)
        {
            TestEventLogger eventLogger1;
            TestEventLogger eventLogger2;

            eventLogger1.LogError(L"Error1");
            eventLogger2.LogError(L"Error2");

            // Check log counts are correct.
            // Shutdown will trigger EventLogger to flush all events.
            AssertSucceeded(eventLogger1.Shutdown());
            const vector<EventLogItem> entries1AfterShutdown = eventLogger1.GetEntries();
            Assert::AreEqual((size_t)1, entries1AfterShutdown.size());

            // Logger 2 should still be running even if Logger 1 shuts down.
            eventLogger2.LogError(L"Error2");

            // Give event logger time to process messages
            Sleep(10); // ms

            const vector<EventLogItem> entries2 = eventLogger2.GetEntries();
            Assert::AreEqual((size_t)2, entries2.size());
        }
    };
}