// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "CppUnitTest.h"
#include "EventLoggerSink.h"
#include "FileLoggerSink.h"
#include "LoggerService.h"
#include "LoggerSinkProvider.h"
#include "TestLoggingHost.h"

#include <sstream>
#include <filesystem>

namespace fs = std::experimental::filesystem;

using namespace MicrosoftInstrumentationEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InstrumentationEngineLibTests
{
    TEST_CLASS(LoggingTests)
    {
    private:
        void AssertNoLogFile(const shared_ptr<CFileLoggerSink>& pSink)
        {
            const tstring tsFilePath = pSink->GetEffectiveFilePath();
            Assert::IsTrue(tsFilePath.empty(), tsFilePath.c_str());
        }

        fs::path AssertLogFileExists(const shared_ptr<CFileLoggerSink>& pSink)
        {
            const tstring tsFilePath = pSink->GetEffectiveFilePath();
            Assert::IsTrue(!tsFilePath.empty());

            fs::path filePath(tsFilePath);
            Assert::IsTrue(exists(filePath), tsFilePath.c_str());

            return filePath;
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
            // Set invalid logging path shall not create a new log file
            shared_ptr<CFileLoggerSink> pFileSink = make_shared<CFileLoggerSink>();
            pFileSink->SetFlags(LoggingFlags_Errors);
            pFileSink->SetFilePath(L"c:\\c:\\c:\\");

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetFileSink(pFileSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            logger.LogError(L"Error1");

            AssertNoLogFile(pFileSink);

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that the log file sink can write to a file using
        // an automatically generated file name.
        TEST_METHOD(LogFileCanWrite)
        {
            // Create partial file path
            auto dir = fs::current_path() / L"\\.";
            shared_ptr<CFileLoggerSink> pFileSink = make_shared<CFileLoggerSink>();
            pFileSink->SetFlags(LoggingFlags_Errors);
            pFileSink->SetFilePath(dir);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetFileSink(pFileSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            logger.LogError(L"Error1");

            fs::path filePath = AssertLogFileExists(pFileSink);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(filePath.c_str(), lines);

            AssertSucceeded(logger.Shutdown());

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
            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";

            if (exists(fileName))
            {
                remove(fileName);
            }

            shared_ptr<CFileLoggerSink> pFileSink = make_shared<CFileLoggerSink>();
            pFileSink->SetFlags(LoggingFlags_Errors);
            pFileSink->SetFilePath(fileName);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetFileSink(pFileSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            logger.LogError(L"Error1");

            AssertLogFileExists(pFileSink);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            AssertSucceeded(logger.Shutdown());

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
            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";

            if (exists(fileName))
            {
                remove(fileName);
            }

            shared_ptr<CFileLoggerSink> pFileSink = make_shared<CFileLoggerSink>();
            pFileSink->SetFlags(LoggingFlags_Errors);
            pFileSink->SetFilePath(fileName);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetFileSink(pFileSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_Trace));
            
            logger.LogError(L"Error1");
            logger.LogMessage(L"Message1");

            // Log file shall exist
            AssertLogFileExists(pFileSink);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            // Check file content only has error
            Assert::AreEqual((size_t)1, lines.size(), fileName.c_str());
            Assert::AreEqual(wstring::npos, lines.at(0).find(L"Message1"), lines.at(0).c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(L"Error1"), lines.at(0).c_str());

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that the log file sink can write to the log file
        // using the default logging flags.
        TEST_METHOD(LogFileCanWriteDefaultFlags)
        {
            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";

            if (exists(fileName))
            {
                remove(fileName);
            }

            shared_ptr<CFileLoggerSink> pFileSink = make_shared<CFileLoggerSink>();
            pFileSink->SetFlags(LoggingFlags_Errors);
            pFileSink->SetFilePath(fileName);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetFileSink(pFileSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_Trace));

            logger.LogMessage(L"Message1");

            // Log file shall exist
            AssertLogFileExists(pFileSink);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            // Log file shall be empty
            Assert::AreEqual((size_t)0, lines.size(), fileName.c_str());

            // Write an error
            logger.LogError(L"Error1");

            // Get contents of log file
            lines.clear();
            ReadAllLines(fileName, lines);

            // Check file content only has error
            Assert::AreEqual((size_t)1, lines.size(), fileName.c_str());
            Assert::AreEqual(wstring::npos, lines.at(0).find(L"Message1"), lines.at(0).c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(L"Error1"), lines.at(0).c_str());

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that the log file sink cannot write to a log file
        // if the log file name is not specified.
        TEST_METHOD(LogFileCannotWriteWithoutFileName)
        {
            shared_ptr<CFileLoggerSink> pFileSink = make_shared<CFileLoggerSink>();
            pFileSink->SetFlags(LoggingFlags_Errors);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetFileSink(pFileSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            logger.LogError(L"Error");

            // Check for non-existence of file
            AssertNoLogFile(pFileSink);

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that the log file sink cannot write to a log file
        // when both sets of logging flags are not specified.
        TEST_METHOD(LogFileCannotWriteWithoutFileLogFlags)
        {
            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";

            if (exists(fileName))
            {
                remove(fileName);
            }

            shared_ptr<CFileLoggerSink> pFileSink = make_shared<CFileLoggerSink>();
            pFileSink->SetFilePath(fileName);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetFileSink(pFileSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            logger.LogError(L"Error");

            // Check for non-existence of file
            AssertNoLogFile(pFileSink);

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that tht log file sink cannot write to a log file
        // after the logger has been shutdown.
        TEST_METHOD(LogFileCannotWriteAfterShutdown)
        {
            // Create exact file path
            auto fileName = fs::current_path() / L"Log.txt";

            if (exists(fileName))
            {
                remove(fileName);
            }

            shared_ptr<CFileLoggerSink> pFileSink = make_shared<CFileLoggerSink>();
            pFileSink->SetFlags(LoggingFlags_Errors);
            pFileSink->SetFilePath(fileName);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetFileSink(pFileSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            logger.LogError(L"Error1");

            // Check for existence of file
            AssertLogFileExists(pFileSink);

            // Get contents of log file
            vector<tstring> lines;
            ReadAllLines(fileName, lines);

            // Check content only has one error
            Assert::AreEqual((size_t)1, lines.size(), fileName.c_str());
            Assert::AreNotEqual(wstring::npos, lines.at(0).find(L"Error1"), lines.at(0).c_str());

            AssertSucceeded(logger.Shutdown());

            // Attempt to log more
            logger.LogError(L"Error2");

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
            CLoggerService logger;
            AssertSucceeded(logger.Initialize());

            CComPtr<CTestLoggingHost> pLoggingHost(new CTestLoggingHost());

            AssertSucceeded(logger.SetLoggingHost(pLoggingHost));

            // Write message to log
            tstring tsMessage1(L"Message1");
            logger.LogMessage(tsMessage1.c_str());

            // Write dump to log
            tstring tsDump1(L"<Data>Value1</Data>");
            logger.LogDumpMessage(tsDump1.c_str());

            // Check log counts are correct (nothing)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_messages.size());

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that the logging host sink can write to the logging host.
        TEST_METHOD(LogHostCanWrite)
        {
            CLoggerService logger;
            AssertSucceeded(logger.Initialize());

            CComPtr<CTestLoggingHost> pLoggingHost(new CTestLoggingHost());

            AssertSucceeded(logger.SetLoggingHost(pLoggingHost));
            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_Trace));

            // Write message to log
            tstring tsMessage1(L"Message1");
            logger.LogMessage(tsMessage1.c_str());

            // Write dump to log
            tstring tsDump1(L"<Data>Value1</Data>");
            logger.LogDumpMessage(tsDump1.c_str());

            // Check log counts are correct (1 message)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)1, pLoggingHost->m_messages.size());
            Assert::AreEqual(tsMessage1, pLoggingHost->m_messages[0]);

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that the logging host sink can write to the logging host
        // using different logging levels.
        TEST_METHOD(LogHostCanWriteWithLevelChange)
        {
            CLoggerService logger;
            AssertSucceeded(logger.Initialize());

            CComPtr<CTestLoggingHost> pLoggingHost(new CTestLoggingHost());

            AssertSucceeded(logger.SetLoggingHost(pLoggingHost));
            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_Trace));

            // Write message to log
            tstring tsMessage1(L"Message1");
            logger.LogMessage(tsMessage1.c_str());

            // Write dump to log
            tstring tsDump1(L"<Data>Value1</Data>");
            logger.LogDumpMessage(tsDump1.c_str());

            // Check log counts are correct (1 message)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)1, pLoggingHost->m_messages.size());
            Assert::AreEqual(tsMessage1, pLoggingHost->m_messages[0]);

            pLoggingHost->Reset();

            // Changing logging flags
            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_InstrumentationResults));

            // Write message to log
            tstring tsMessage2(L"Message2");
            logger.LogMessage(tsMessage2.c_str());

            // Write dump to log
            tstring tsDump2(L"<Data>Value2</Data>");
            logger.LogDumpMessage(tsDump2.c_str());

            // Check log counts are correct (1 dump)
            Assert::AreEqual((size_t)1, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_messages.size());
            Assert::AreEqual(tsDump2, pLoggingHost->m_dumps[0]);

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that tht logging host sink cannot write to a logging host
        // after the logger has been shutdown.
        TEST_METHOD(LogHostCannotWriteAfterShutdown)
        {
            CLoggerService logger;
            AssertSucceeded(logger.Initialize());

            CComPtr<CTestLoggingHost> pLoggingHost(new CTestLoggingHost());

            AssertSucceeded(logger.SetLoggingHost(pLoggingHost));
            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_Trace));

            // Write message to log
            tstring tsMessage1(L"Message1");
            logger.LogMessage(tsMessage1.c_str());

            // Check log counts are correct (1 message)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)1, pLoggingHost->m_messages.size());
            Assert::AreEqual(tsMessage1, pLoggingHost->m_messages[0]);

            pLoggingHost->Reset();

            AssertSucceeded(logger.Shutdown());

            // Attempt to log more (should not make it to log file)
            logger.LogMessage(L"Message2");

            // Check log counts are correct (nothing)
            Assert::AreEqual((size_t)0, pLoggingHost->m_dumps.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_errors.size());
            Assert::AreEqual((size_t)0, pLoggingHost->m_messages.size());
        }

        // Tests that tht event sink can write to the event source
        TEST_METHOD(LogEventCanWrite)
        {
            // Register callback for recording entries processed by event logger sink
            vector<tstring> entries;
            function<void(const tstring&)> callback([&entries](const tstring& entry)
                {
                    entries.push_back(entry);
                });

            shared_ptr<CEventLoggerSink> pEventSink = make_shared<CEventLoggerSink>();
            pEventSink->SetEventCallback(callback);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetEventSink(pEventSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_Errors));

            // Write error to log
            tstring tsError1(L"Error1");
            logger.LogError(tsError1.c_str());

            // Give event logger time to process messages
            Sleep(10); // ms

            // Check log counts are correct (1 error)
            Assert::AreEqual((size_t)1, entries.size());
            Assert::AreEqual(tsError1, entries.at(0), entries.at(0).c_str());

            AssertSucceeded(logger.Shutdown());
        }

        // Tests that tht event sink can write to the event source
        TEST_METHOD(LogEventCanWriteWaitForDrain)
        {
            // Register callback for recording entries processed by event logger sink
            vector<tstring> entries;
            function<void(const tstring&)> callback([&entries](const tstring& entry)
                {
                    // Sleep some arbitrary amount of time to allow appending of all
                    // items to the queue before draining all items from the queue.
                    Sleep(entries.size() % 25);

                    entries.push_back(entry);
                });

            shared_ptr<CEventLoggerSink> pEventSink = make_shared<CEventLoggerSink>();
            pEventSink->SetEventCallback(callback);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetEventSink(pEventSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_Errors));

            // Number of items to write to event log
            const int nCount = 100;

            // Write errors to log
            for (int i = 0; i < 100; i++)
            {
                tstring tsError(L"Error");
                tsError.append(to_wstring(i));
                logger.LogError(tsError.c_str());
            }

            // Check log counts are correct
            // Event logger sink should not have processed all items just yet,
            // so check that it is less than the total number of items.
            Assert::AreNotEqual((size_t)nCount, entries.size());

            AssertSucceeded(logger.Shutdown());

            // Check log counts are correct
            // Event logger sink should always process remaining items in queue
            // immediately after being signaled to shutdown.
            Assert::AreEqual((size_t)nCount, entries.size());
        }

        // Tests that tht event sink cannot write after the logger has been shutdown.
        TEST_METHOD(LogEventCannotWriteAfterShutdown)
        {
            // Register callback for recording entries processed by event logger sink
            vector<tstring> entries;
            function<void(const tstring&)> callback([&entries](const tstring& entry)
                {
                    entries.push_back(entry);
                });

            shared_ptr<CEventLoggerSink> pEventSink = make_shared<CEventLoggerSink>();
            pEventSink->SetEventCallback(callback);

            shared_ptr<CLoggerSinkProvider> pSinkProvider = make_shared<CLoggerSinkProvider>();
            pSinkProvider->SetEventSink(pEventSink);

            CLoggerService logger;
            logger.SetProvider(pSinkProvider);

            AssertSucceeded(logger.Initialize());

            AssertSucceeded(logger.SetLoggingFlags(LoggingFlags_Errors));

            // Write error to log
            tstring tsError1(L"Error1");
            logger.LogError(tsError1.c_str());

            // Give event logger time to process messages
            Sleep(10); // ms

            // Check log counts are correct (1 error)
            Assert::AreEqual((size_t)1, entries.size());
            Assert::AreEqual(tsError1, entries.at(0), entries.at(0).c_str());

            AssertSucceeded(logger.Shutdown());

            entries.clear();

            // Attempt to log more (should not make it to event log)
            logger.LogError(L"Error2");

            // Give event logger time to process messages
            Sleep(10); // ms

            // Check log counts are correct (1 error)
            Assert::AreEqual((size_t)0, entries.size());
        }
    };
}