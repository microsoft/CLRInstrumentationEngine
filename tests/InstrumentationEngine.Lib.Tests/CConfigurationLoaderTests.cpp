// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ConfigurationLoader.h"
#include "CppUnitTest.h"
#include <fstream>
#include <sstream>
namespace fs = std::experimental::filesystem;


using namespace MicrosoftInstrumentationEngine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InstrumentationEngineLibTests
{
    TEST_CLASS(CConfigurationLoaderTests)
    {
    public:

        TEST_METHOD(LoadConfigurationLoadsOneInstrumentationMethodCorectly)
        {
            auto fileName = fs::current_path() / L"Config.xml";
            if (exists(fileName))
            {
                remove(fileName);
            }

            wfstream output;
            output.open(fileName, ios_base::app | ios_base::out);
            auto error = _wcserror(GetLastError());
            Assert::IsTrue(!output.fail() && !output.bad(), error);

            output << L"<InstrumentationEngineConfiguration>" << std::endl;
            output << L"<InstrumentationMethod>" << std::endl;
            output << L"  <Name>Squid Instrumentation</Name>" << std::endl;
            output << L"  <Description>Dynamically make squids swim</Description>" << std::endl;
            output << L"  <Module>SeafoodInstrumentation.dll</Module>" << std::endl;
            output << L"  <ClassGuid>{249E89A6-12D9-4E03-82FF-7FEAA41310E9}</ClassGuid>" << std::endl;
            output << L"  <Priority>50</Priority>" << std::endl;
            output << L"</InstrumentationMethod>" << std::endl;
            output << L"</InstrumentationEngineConfiguration>" << std::endl;

            output.close();

            CConfigurationLoader loader;
            std::vector<CInstrumentationMethod*> methods;

            Assert::AreEqual(S_OK, loader.LoadConfiguration(BSTR(fileName.c_str()), methods));

            Assert::AreEqual((size_t)1, methods.size());
            auto method = methods.at(0);

            GUID expected = { 0x249E89A6, 0x12D9, 0x4E03, { 0x82 ,0xFF, 0x7F, 0xEA, 0xA4, 0x13, 0x10, 0xE9 } };
            Assert::IsTrue(method->GetClassId() == expected);
        }

        TEST_METHOD(LoadConfigurationDoesNotCrashWhenDescriptionIsEmpty)
        {
            auto fileName = fs::current_path() / L"Config.xml";
            if (exists(fileName))
            {
                remove(fileName);
            }
            wfstream output;
            output.open(fileName, ios_base::app | ios_base::out);
            auto error = _wcserror(GetLastError());
            Assert::IsTrue(!output.fail() && !output.bad(), error);

            output << L"<InstrumentationEngineConfiguration>" << std::endl;
            output << L"<InstrumentationMethod>" << std::endl;
            output << L"  <Name>Squid Instrumentation</Name>" << std::endl;
            output << L"  <Description></Description>" << std::endl;
            output << L"  <Module>SeafoodInstrumentation.dll</Module>" << std::endl;
            output << L"  <ClassGuid>{249E89A6-12D9-4E03-82FF-7FEAA41310E9}</ClassGuid>" << std::endl;
            output << L"  <Priority>50</Priority>" << std::endl;
            output << L"</InstrumentationMethod>" << std::endl;
            output << L"</InstrumentationEngineConfiguration>" << std::endl;

            output.close();

            CConfigurationLoader loader;
            std::vector<CInstrumentationMethod*> methods;

            Assert::AreEqual(E_FAIL, loader.LoadConfiguration(BSTR(fileName.c_str()), methods));

            Assert::AreEqual((size_t)0, methods.size());
        }

        TEST_METHOD(LoadConfigurationDoesNotCrashWhenDescriptionIsMissing)
        {
            auto fileName = fs::current_path() / L"Config.xml";
            if (exists(fileName))
            {
                remove(fileName);
            }
            wfstream output;
            output.open(fileName, ios_base::app | ios_base::out);
            auto error = _wcserror(GetLastError());
            Assert::IsTrue(!output.fail() && !output.bad(), error);

            output << L"<InstrumentationEngineConfiguration>" << std::endl;
            output << L"<InstrumentationMethod>" << std::endl;
            output << L"  <Name>Squid Instrumentation</Name>" << std::endl;
            output << L"  <Module>SeafoodInstrumentation.dll</Module>" << std::endl;
            output << L"  <ClassGuid>{249E89A6-12D9-4E03-82FF-7FEAA41310E9}</ClassGuid>" << std::endl;
            output << L"  <Priority>50</Priority>" << std::endl;
            output << L"</InstrumentationMethod>" << std::endl;
            output << L"</InstrumentationEngineConfiguration>" << std::endl;

            output.close();

            CConfigurationLoader loader;
            std::vector<CInstrumentationMethod*> methods;

            Assert::AreEqual(E_FAIL, loader.LoadConfiguration(BSTR(fileName.c_str()), methods));

            Assert::AreEqual((size_t)0, methods.size());
        }
    };
}