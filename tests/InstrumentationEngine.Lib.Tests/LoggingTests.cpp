// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#include "stdafx.h"
#include "CppUnitTest.h"
#include "Logging.h"
#include "CorHdr.h"
#include <sstream>
#include <filesystem>
namespace fs = std::experimental::filesystem;

using namespace MicrosoftInstrumentationEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InstrumentationEngineLibTests
{
    TEST_CLASS(LoggingTests)
    {
    public:

        TEST_METHOD(EnableLoggingToFileWillNotFailWithIncorrectPath)
        {
            CLogging::EnableLoggingToFile(LoggingFlags_Errors, L"c:\\c:\\c:\\");

            CLogging::LogError(L"error");

            CLogging::EnableLoggingToFile(LoggingFlags_None, L"");
            //Code would not throw
        }

        TEST_METHOD(EnableLoggingToFileCanWriteFile)
        {
            auto dir = fs::current_path();

            CLogging::EnableLoggingToFile(LoggingFlags_Errors, dir);

            CLogging::LogError(L"error");

            CLogging::EnableLoggingToFile(LoggingFlags_None, L"");
            //Code would not throw
        }

        TEST_METHOD(EnableLoggingToFileCanWriteExactFile)
        {
            auto fileName = fs::current_path() / L"Log.txt";

            if (exists(fileName))
            {
                remove(fileName);
            }

            CLogging::EnableLoggingToFile(LoggingFlags_Errors, fileName);

            CLogging::LogError(L"error");

            CLogging::EnableLoggingToFile(LoggingFlags_None, L"");

            std::wifstream infile(fileName);
            wstring content;
            infile >> content;

            infile.close();

            if (exists(fileName))
            {
                remove(fileName);
            }

            Assert::IsTrue(content.find(L"error") > 0, content.c_str());
        }
    };
}