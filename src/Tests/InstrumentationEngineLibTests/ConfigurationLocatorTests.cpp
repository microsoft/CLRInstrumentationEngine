// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// This file only covers UNIX environment.
// Similar test for windows is in tests/InstrumentationEngine.Lib.Tests
#ifdef PLATFORM_UNIX

#include "pch.h"
#include <fstream>
#include <cstdlib>

#include "Common.Lib/refcount.h"
#include "Common.Lib/systemstring.h"

using namespace CommonLib;

#include "InstrumentationEngine.Api/InstrumentationEngine.h"
#include "InstrumentationEngine.Lib/InstrumentationMethod.h"
#include "InstrumentationEngine.Lib/ConfigurationLocator.h"

using namespace MicrosoftInstrumentationEngine;
using namespace std;

namespace fs = std::experimental::filesystem;

namespace {
    struct temp_file: fs::path {
        temp_file(fs::path p): path(p)
        {
            ofstream{p};
        }

        ~temp_file()
        {
            remove(*this);
        }
    };
}

TEST(ConfigurationTests, GetConfigurationPathsExpandsPathCorrectly)
{
    constexpr const char szFeatureAVariable[] = "MicrosoftInstrumentationEngine_ConfigPath64_FeatureA";
    constexpr const char szFeatureBVariable[] = "MicrosoftInstrumentationEngine_ConfigPath64_FeatureB";
    constexpr const char szFeatureCVariable[] = "MicrosoftInstrumentationEngine_ConfigPath64_FeatureC";

    // Create temporary files to use during test.
    temp_file filePath1 = fs::temp_directory_path() / "config1.xml";
    temp_file filePath2 = fs::temp_directory_path() / "config2.xml";

    // This variable will be picked up, but should be excluded from results since the path does not exist.
    setenv(szFeatureAVariable, "/non/existing/path", true);
    // This variable will be picked up and included since the CWD exists.
    setenv(szFeatureBVariable, ".", true);
    // Some of the paths will be picked up from this variable, the first and third one.
    string sPaths = string(filePath1) + ":/non/existing/path:" + string(filePath2);
    setenv(szFeatureCVariable, sPaths.c_str(), true);

    std::vector<ATL::CComPtr<CConfigurationSource>> sources;
    ASSERT_OK(CConfigurationLocator::GetFromEnvironment(sources));

    // Clear environment variables so that they do not interfere with other tests
    unsetenv(szFeatureAVariable);
    unsetenv(szFeatureBVariable);
    unsetenv(szFeatureCVariable);

    ASSERT_EQ(3, sources.size());

    tstring expected;
    CComBSTR actual;

    ASSERT_OK(sources[0]->GetPath(&actual));
    ASSERT_OK(SystemString::Convert(fs::current_path().c_str(), expected));
    ASSERT_EQ(expected, tstring{actual});

    ASSERT_OK(sources[1]->GetPath(&actual));
    ASSERT_OK(SystemString::Convert(filePath1.c_str(), expected));
    ASSERT_EQ(expected, tstring{actual});

    ASSERT_OK(sources[2]->GetPath(&actual));
    ASSERT_OK(SystemString::Convert(filePath2.c_str(), expected));
    ASSERT_EQ(expected, tstring{actual});
}

#endif
