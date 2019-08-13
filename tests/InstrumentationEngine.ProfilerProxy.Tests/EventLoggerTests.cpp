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
            AssertSucceeded(eventLogger.Initialize());

            // Number of items to write to event log
            const int nCount = 100;

            // Write errors to log
            for (int i = 0; i < nCount; i++)
            {
                tstring tsError(L"Error");
                tsError.append(to_wstring(i));
                eventLogger.LogError(tsError.c_str(), nullptr);
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
            AssertSucceeded(eventLogger.Initialize());

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
                    eventLogger.LogError(tsError.c_str(), nullptr);
                    break;
                case 1:
                    // Log Warning
                    tsWarning.append(to_wstring(i));
                    eventLogger.LogWarning(tsWarning.c_str(), nullptr);
                    break;
                case 2:
                    // Log Message
                    tsMessage.append(to_wstring(i));
                    eventLogger.LogMessage(tsMessage.c_str(), nullptr);
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

        // Tests EventLogger cannot write before initialization
        TEST_METHOD(EventLoggerCannotWriteBeforeInitialize)
        {
            TestEventLogger eventLogger;
            eventLogger.LogMessage(L"Test", nullptr);

            // Give event logger time to process messages
            Sleep(10); // ms

            std::vector<EventLogItem> entries = eventLogger.GetEntries();
            Assert::AreEqual((size_t)0, entries.size());
        }

        // Tests EventLogger cannot write after shutdown
        TEST_METHOD(EventLoggerCannotWriteAfterShutdown)
        {
            TestEventLogger eventLogger;
            AssertSucceeded(eventLogger.Initialize());

            eventLogger.LogError(L"Error", nullptr);

            // Check log counts are correct.
            // Shutdown will trigger EventLogger to flush all events.
            AssertSucceeded(eventLogger.Shutdown());
            const vector<EventLogItem> entriesAfterShutdown = eventLogger.GetEntries();
            Assert::AreEqual((size_t)1, entriesAfterShutdown.size());

            eventLogger.LogError(L"Error", nullptr);

            // Give event logger time to process messages
            Sleep(10); // ms

            const vector<EventLogItem> entriesAfterShutdown2 = eventLogger.GetEntries();
            Assert::AreEqual((size_t)1, entriesAfterShutdown2.size());
        }
    };
}