// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CppUnitTest.h"
#include "EventLoggerSink.h"
#include "FileLoggerSink.h"
#include "TestLoggerService.h"
#include "TestLoggingHost.h"

#include <filesystem>

namespace fs = std::experimental::filesystem;

using namespace MicrosoftInstrumentationEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InstrumentationEngineLibTests
{
    TEST_CLASS(LoggingTests)
    {
    private:
        void AssertNoLogFile(const CTestLoggerService& loggerService)
        {
            const tstring tsFilePath = loggerService.GetFilePathIfExists();
            Assert::IsTrue(tsFilePath.empty(), tsFilePath.c_str());
        }

        void AssertLogFileExists(const CTestLoggerService& loggerService)
        {
            const tstring tsFilePath = loggerService.GetFilePathIfExists();
            Assert::IsFalse(tsFilePath.empty(), tsFilePath.c_str());
        }

        void AssertSucceeded(HRESULT hr)
        {
            Assert::IsTrue(SUCCEEDED(hr));
        }

        void ReadAllLines(tstring path, vector<tstring>& lines)
        {
            wifstream infile(path);
            while (!infile.eof())
            {
                tstring line;
                getline(infile, line);
                if (!infile.eof())
                {
                    lines.push_back(line);
                }
            }
            infile.close();
        }

    public:

        // Tests that the log file sink gracefully fails to create
        // the log file when specifying an invalid file path.
        TEST_METHOD(LogFileCannotWriteIncorrectPath)
        {
            CTestLoggerService loggerService;
            loggerService.SetFileFlags(LoggingFlags_Errors);
            // Set invalid logging path shall not create a new log file
            loggerService.SetFilePath(L"c:\\c:\\c:\\");

            AssertSucceeded(loggerService.Initialize());

            loggerService.LogError(L"Error1");

            AssertNoLogFile(loggerService);

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the log file sink can write to a file using
        // an automatically generated file name.
        TEST_METHOD(LogFileCanWrite)
        {
            CTestLoggerService loggerService;
            loggerService.SetFileFlags(LoggingFlags_Errors);

            // Create partial file path
            auto dir = fs::current_path() / L"\\.";
            loggerService.SetFilePath(dir);

            AssertSucceeded(loggerService.Initialize());

            loggerService.LogError(L"Error1");

            AssertLogFileExists(loggerService);

            fs::path filePath(loggerService.GetFilePathIfExists());

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(filePath.c_str(), lines);

            AssertSucceeded(loggerService.Shutdown());

            // Remove old log file
            remove(filePath);

            // Check file content only has one error
            Assert::AreEqual((size_t)1, lines.size(), filePath.c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(L"Error1"), lines.at(0).c_str());
        }

        // Tests that the log file sink can write to
        // a fully qualified file name.
        TEST_METHOD(LogFileCanWriteExact)
        {
            CTestLoggerService loggerService;
            loggerService.SetFileFlags(LoggingFlags_Errors);

            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";
            if (exists(fileName))
            {
                remove(fileName);
            }

            loggerService.SetFilePath(fileName);

            AssertSucceeded(loggerService.Initialize());

            loggerService.LogError(L"Error1");

            AssertLogFileExists(loggerService);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            AssertSucceeded(loggerService.Shutdown());

            // Remove old log file
            remove(fileName);

            // Check file content only has one error
            Assert::AreEqual((size_t)1, lines.size(), fileName.c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(L"Error1"), lines.at(0).c_str());
        }

        // Tests that the log file sink can write to the log file
        // using the log file flags, which should override the
        // default flags for general logging.
        TEST_METHOD(LogFileCanWriteDifferentLevel)
        {
            CTestLoggerService loggerService;
            loggerService.SetFileFlags(LoggingFlags_Errors);

            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";
            if (exists(fileName))
            {
                remove(fileName);
            }

            loggerService.SetFilePath(fileName);

            AssertSucceeded(loggerService.Initialize());

            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_Trace));

            loggerService.LogError(L"Error1");
            loggerService.LogMessage(L"Message1");

            // Log file shall exist
            AssertLogFileExists(loggerService);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            // Check file content only has error
            Assert::AreEqual((size_t)1, lines.size(), fileName.c_str());
            Assert::AreEqual(wstring::npos, lines.at(0).find(L"Message1"), lines.at(0).c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(L"Error1"), lines.at(0).c_str());

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the log file sink can write to the log file
        // using the default logging flags.
        TEST_METHOD(LogFileCanWriteDefaultFlags)
        {
            CTestLoggerService loggerService;

            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";
            if (exists(fileName))
            {
                remove(fileName);
            }

            loggerService.SetFilePath(fileName);

            AssertSucceeded(loggerService.Initialize());

            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_Trace));

            // Log an error
            tstring tsError1(L"Error1");
            loggerService.LogError(tsError1.c_str());

            // Log file shall exist
            AssertLogFileExists(loggerService);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            // Log file shall be empty
            Assert::AreEqual((size_t)0, lines.size(), fileName.c_str());

            // Write an message
            tstring tsMessage1(L"Message1");
            loggerService.LogMessage(tsMessage1.c_str());

            // Get contents of log file
            ReadAllLines(fileName, lines);

            // Check file content only has message
            Assert::AreEqual((size_t)1, lines.size(), fileName.c_str());
            Assert::AreEqual(wstring::npos, lines.at(0).find(tsError1), lines.at(0).c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(tsMessage1), lines.at(0).c_str());

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the log file sink cannot write to a log file
        // if the log file name is not specified.
        TEST_METHOD(LogFileCannotWriteWithoutFileName)
        {
            CTestLoggerService loggerService;
            loggerService.SetFileFlags(LoggingFlags_Errors);

            AssertSucceeded(loggerService.Initialize());

            loggerService.LogError(L"Error");

            // Check for non-existence of file
            AssertNoLogFile(loggerService);

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the log file sink cannot write to a log file
        // when both sets of logging flags are not specified.
        TEST_METHOD(LogFileCannotWriteWithoutFileLogFlags)
        {
            CTestLoggerService loggerService;

            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";
            if (exists(fileName))
            {
                remove(fileName);
            }

            loggerService.SetFilePath(fileName);

            AssertSucceeded(loggerService.Initialize());

            loggerService.LogError(L"Error");

            // Check for non-existence of file
            AssertNoLogFile(loggerService);

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the log file sink cannot write to a log file
        // after the logger has been shutdown.
        TEST_METHOD(LogFileCannotWriteAfterShutdown)
        {
            CTestLoggerService loggerService;
            loggerService.SetFileFlags(LoggingFlags_Errors);

            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";
            if (exists(fileName))
            {
                remove(fileName);
            }

            loggerService.SetFilePath(fileName);

            AssertSucceeded(loggerService.Initialize());

            loggerService.LogError(L"Error1");

            // Check for existence of file
            AssertLogFileExists(loggerService);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            // Check content only has one error
            Assert::AreEqual((size_t)1, lines.size(), fileName.c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(L"Error1"), lines.at(0).c_str());

            AssertSucceeded(loggerService.Shutdown());

            // Attempt to log more
            loggerService.LogError(L"Error2");

            // Get contents of log file
            lines.clear();
            ReadAllLines(fileName, lines);

            // Remove log file
            remove(fileName);

            // Check content only has one error
            Assert::AreEqual((size_t)1, lines.size(), fileName.c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(L"Error1"), lines.at(0).c_str());
            Assert::AreEqual(wstring::npos, lines.at(0).find(L"Error2"), lines.at(0).c_str());
        }

        // Tests that the logging host sink cannot write to the logging host
        // if no flags have been specified.
        TEST_METHOD(LogHostCannotWriteWithoutFlags)
        {
            CTestLoggerService loggerService;
            AssertSucceeded(loggerService.Initialize());

            CComPtr<CTestLoggingHost> pLoggingHost(new (nothrow) CTestLoggingHost());
            Assert::IsNotNull(pLoggingHost.p);

            AssertSucceeded(loggerService.SetLoggingHost(pLoggingHost));

            // Write message to log
            tstring tsMessage1(L"Message1");
            loggerService.LogMessage(tsMessage1.c_str());

            // Write dump to log
            tstring tsDump1(L"<Data>Value1</Data>");
            loggerService.LogDumpMessage(tsDump1.c_str());

            // Check log counts are correct (nothing)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_messages.size());

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the logging host sink can write to the logging host.
        TEST_METHOD(LogHostCanWrite)
        {
            CTestLoggerService loggerService;
            AssertSucceeded(loggerService.Initialize());

            CComPtr<CTestLoggingHost> pLoggingHost(new (nothrow) CTestLoggingHost());
            Assert::IsNotNull(pLoggingHost.p);

            AssertSucceeded(loggerService.SetLoggingHost(pLoggingHost));
            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_Trace));

            // Write message to log
            tstring tsMessage1(L"Message1");
            loggerService.LogMessage(tsMessage1.c_str());

            // Write dump to log
            tstring tsDump1(L"<Data>Value1</Data>");
            loggerService.LogDumpMessage(tsDump1.c_str());

            // Check log counts are correct (1 message)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)1, pLoggingHost->m_messages.size());
            Assert::AreEqual(tsMessage1, pLoggingHost->m_messages[0]);

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the logging host sink can write to the logging host
        // using different logging levels.
        TEST_METHOD(LogHostCanWriteWithLevelChange)
        {
            CTestLoggerService loggerService;
            AssertSucceeded(loggerService.Initialize());

            CComPtr<CTestLoggingHost> pLoggingHost(new (nothrow) CTestLoggingHost());
            Assert::IsNotNull(pLoggingHost.p);

            AssertSucceeded(loggerService.SetLoggingHost(pLoggingHost));
            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_Trace));

            // Write message to log
            tstring tsMessage1(L"Message1");
            loggerService.LogMessage(tsMessage1.c_str());

            // Write dump to log
            tstring tsDump1(L"<Data>Value1</Data>");
            loggerService.LogDumpMessage(tsDump1.c_str());

            // Check log counts are correct (1 message)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)1, pLoggingHost->m_messages.size());
            Assert::AreEqual(tsMessage1, pLoggingHost->m_messages[0]);

            pLoggingHost->Reset();

            // Changing logging flags
            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_InstrumentationResults));

            // Write message to log
            tstring tsMessage2(L"Message2");
            loggerService.LogMessage(tsMessage2.c_str());

            // Write dump to log
            tstring tsDump2(L"<Data>Value2</Data>");
            loggerService.LogDumpMessage(tsDump2.c_str());

            // Check log counts are correct (1 dump)
            Assert::AreEqual((size_t)1, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_messages.size());
            Assert::AreEqual(tsDump2, pLoggingHost->m_dumps[0]);

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the logging host sink cannot write to a logging host
        // after the logger has been shutdown.
        TEST_METHOD(LogHostCannotWriteAfterShutdown)
        {
            CTestLoggerService loggerService;
            AssertSucceeded(loggerService.Initialize());

            CComPtr<CTestLoggingHost> pLoggingHost(new (nothrow) CTestLoggingHost());
            Assert::IsNotNull(pLoggingHost.p);

            AssertSucceeded(loggerService.SetLoggingHost(pLoggingHost));
            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_Trace));

            // Write message to log
            tstring tsMessage1(L"Message1");
            loggerService.LogMessage(tsMessage1.c_str());

            // Check log counts are correct (1 message)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)1, pLoggingHost->m_messages.size());
            Assert::AreEqual(tsMessage1, pLoggingHost->m_messages[0]);

            pLoggingHost->Reset();

            AssertSucceeded(loggerService.Shutdown());

            // Attempt to log more (should not make it to log file)
            loggerService.LogMessage(L"Message2");

            // Check log counts are correct (nothing)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_messages.size());
        }

        // Tests that the event sink can write to the event source
        TEST_METHOD(LogEventCanWrite)
        {
            CTestLoggerService loggerService;
            AssertSucceeded(loggerService.Initialize());

            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_Errors));

            // Write error to log
            tstring tsError1(L"Error1");
            loggerService.LogError(tsError1.c_str());

            // Give event logger time to process messages
            Sleep(100); // ms

            const vector<tstring> entries = loggerService.GetEventEntries();

            // Check log counts are correct (1 error)
            Assert::AreEqual((size_t)1, entries.size());
            Assert::AreEqual(tsError1, entries.at(0), entries.at(0).c_str());

            AssertSucceeded(loggerService.Shutdown());
        }

        // Tests that the event sink can write to the event source
        TEST_METHOD(LogEventCanWriteWaitForDrain)
        {
            CTestLoggerService loggerService;
            AssertSucceeded(loggerService.Initialize());

            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_Errors));

            // Number of items to write to event log
            const int nCount = 100;

            // Write errors to log
            for (int i = 0; i < 100; i++)
            {
                tstring tsError(L"Error");
                tsError.append(to_wstring(i));
                loggerService.LogError(tsError.c_str());
            }

            const vector<tstring> entries = loggerService.GetEventEntries();

            // Check log counts are correct
            // Event logger sink should not have processed all items just yet,
            // so check that it is less than the total number of items.
            Assert::AreNotEqual((size_t)nCount, entries.size());

            AssertSucceeded(loggerService.Shutdown());

            const vector<tstring> entriesAfterShutdown = loggerService.GetEventEntries();

            // Check log counts are correct
            // Event logger sink should always process remaining items in queue
            // immediately after being signaled to shutdown.
            Assert::AreEqual((size_t)nCount, entriesAfterShutdown.size());
        }

        // Tests that the event sink cannot write after the logger has been shutdown.
        TEST_METHOD(LogEventCannotWriteAfterShutdown)
        {
            CTestLoggerService loggerService;
            AssertSucceeded(loggerService.Initialize());

            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_Errors));

            // Write error to log
            tstring tsError1(L"Error1");
            loggerService.LogError(tsError1.c_str());

            // Give event logger time to process messages
            Sleep(10); // ms

            const vector<tstring> entries = loggerService.GetEventEntries();

            // Check log counts are correct (1 error)
            Assert::AreEqual((size_t)1, entries.size());
            Assert::AreEqual(tsError1, entries.at(0), entries.at(0).c_str());

            AssertSucceeded(loggerService.Shutdown());

            loggerService.ClearEventEntries();

            // Attempt to log more (should not make it to event log)
            loggerService.LogError(L"Error2");

            // Give event logger time to process messages
            Sleep(10); // ms

            const vector<tstring> entriesAfterShutdown = loggerService.GetEventEntries();

            // Check log counts are correct (1 error)
            Assert::AreEqual((size_t)0, entriesAfterShutdown.size());
        }

        // Tests that the log file sink can write to a file using
        // InstrumentationMethodFlags
        TEST_METHOD(LogFileCanWriteWithInstrumentationMethodFlags)
        {
            CTestLoggerService loggerService;
            loggerService.SetFileFlags((LoggingFlags)(LoggingFlags_Errors & LoggingFlags_Trace));

            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";
            if (exists(fileName))
            {
                remove(fileName);
            }

            loggerService.SetFilePath(fileName);

            AssertSucceeded(loggerService.Initialize());

            AssertSucceeded(loggerService.SetLoggingFlags(LoggingFlags_None));

            GUID testGuid;
            IIDFromString(L"{00000000-0000-0000-0000-000000000000}", &testGuid);
            loggerService.UpdateInstrumentationMethodLoggingFlags(testGuid, LoggingFlags_Errors);

            GUID testGuid2;
            IIDFromString(L"{00000000-0000-0000-0000-000000000001}", &testGuid2);
            loggerService.UpdateInstrumentationMethodLoggingFlags(testGuid2, LoggingFlags_Trace);

            loggerService.LogError(L"Error1");
            loggerService.LogMessage(L"Message1");
            loggerService.LogDumpMessage(L"Dump1");

            // Log file shall exist
            AssertLogFileExists(loggerService);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            // Check file content only has error
            Assert::AreEqual((size_t)2, lines.size(), fileName.c_str());
            Assert::AreEqual(wstring::npos, lines.at(0).find(L"Message1"), lines.at(0).c_str());
            Assert::AreEqual(wstring::npos, lines.at(1).find(L"Error1"), lines.at(1).c_str());

            AssertSucceeded(loggerService.Shutdown());
        }
    };
}