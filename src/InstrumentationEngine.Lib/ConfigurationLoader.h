// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once


#define ConfigurationLoaderFieldMaxLength 1024

namespace MicrosoftInstrumentationEngine
{
    class CInstrumentationMethod;
}

namespace CommonLib 
{
    class CXmlNode;
}

class CConfigurationLoader
{
public:
    HRESULT LoadConfiguration(_In_ BSTR bstrConfigPath, _In_ std::vector<MicrosoftInstrumentationEngine::CInstrumentationMethod*>& methods);
};

class CConfigurationLoaderHelper
{
public:
    HRESULT LoadConfiguration(_In_ BSTR bstrConfigPath, _In_ std::vector<MicrosoftInstrumentationEngine::CInstrumentationMethod*>& methods);

private:
    HRESULT ProcessInstrumentationMethodNode(_In_ BSTR bstrInstrumentationMethodFolder, _In_ CommonLib::CXmlNode* pNode, _In_ std::vector<MicrosoftInstrumentationEngine::CInstrumentationMethod*>& methods);
};
