// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "CppUnitTest.h"
#include <fstream>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::experimental::filesystem;

#include "ConfigurationLocator.h"

namespace InstrumentationEngineLibTests
{
    TEST_CLASS(ConfigurationLocatorTests)
    {
    private:
#ifdef _WIN64
        static constexpr const WCHAR* s_wszFeatureAVariable = _T("MicrosoftInstrumentationEngine_ConfigPath64_FeatureA");
        static constexpr const WCHAR* s_wszFeatureBVariable = _T("MicrosoftInstrumentationEngine_ConfigPath64_FeatureB");
        static constexpr const WCHAR* s_wszFeatureCVariable = _T("MicrosoftInstrumentationEngine_ConfigPath64_FeatureC");
#else
        static constexpr const WCHAR* s_wszFeatureAVariable = _T("MicrosoftInstrumentationEngine_ConfigPath32_FeatureA");
        static constexpr const WCHAR* s_wszFeatureBVariable = _T("MicrosoftInstrumentationEngine_ConfigPath32_FeatureB");
        static constexpr const WCHAR* s_wszFeatureCVariable = _T("MicrosoftInstrumentationEngine_ConfigPath32_FeatureC");
#endif

    public:
        BEGIN_TEST_METHOD_ATTRIBUTE(TestGetConfigurationPaths)
            TEST_DESCRIPTION(L"Ensures that GetConfigurationPaths expands paths correctly")
            END_TEST_METHOD_ATTRIBUTE()
            TEST_METHOD(TestGetConfigurationPaths)
        {
            _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

            // Create temporary files to use during test that will be deleted upon closing.
            auto filePath1 = fs::temp_directory_path() / L"config1.xml";
            ATL::CHandle hFile1(CreateFile(filePath1.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, nullptr));

            auto filePath2 = fs::temp_directory_path() / L"config2.xml";
            ATL::CHandle hFile2(CreateFile(filePath2.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, nullptr));

            // This variable will be used by other variables that are processed by GetConfigurationPaths
            SetEnvironmentVariable(_T("TEST_FILE_PATH"), filePath1.c_str());
            // This variable will be picked up, but should be excluded from results since the path does not exist.
            SetEnvironmentVariable(s_wszFeatureAVariable, _T("%NON_EXISTING_VAR%"));
            // This variable will be picked up and included since the CWD exists.
            SetEnvironmentVariable(s_wszFeatureBVariable, _T("."));
            // Some of the paths will be picked up from this variable, the first and third one.
            std::wstring wsPaths = _T("%TEST_FILE_PATH%;%NON_EXISTING_VAR%;");
            wsPaths.append(filePath2);
            SetEnvironmentVariable(s_wszFeatureCVariable, wsPaths.c_str());

            std::unique_ptr<WCHAR[]> pszCurrentWorkingDirectory(_wgetcwd(nullptr, 0));

            HRESULT hr = S_OK;
            std::vector<ATL::CComPtr<CConfigurationSource>> sources;
            Assert::AreEqual<HRESULT>(S_OK, hr = CConfigurationLocator::GetFromEnvironment(sources));
            Assert::AreEqual<size_t>(3, sources.size(), _T("Should return 3 paths."));

            CComBSTR firstSourcePath;
            Assert::AreEqual<HRESULT>(S_OK, hr = sources[0]->GetPath(&firstSourcePath));
            Assert::AreEqual(pszCurrentWorkingDirectory.get(), firstSourcePath, _T("'.' should be expanded to the CWD."));

            CComBSTR secondSourcePath;
            Assert::AreEqual<HRESULT>(S_OK, hr = sources[1]->GetPath(&secondSourcePath));
            Assert::AreEqual(filePath1.c_str(), secondSourcePath, _T("filePath1 should be expanded to its value."));

            CComBSTR thirdSourcePath;
            Assert::AreEqual<HRESULT>(S_OK, hr = sources[2]->GetPath(&thirdSourcePath));
            Assert::AreEqual(filePath2.c_str(), thirdSourcePath, _T("filePath2 should be expanded to its value."));
        }
    };
}