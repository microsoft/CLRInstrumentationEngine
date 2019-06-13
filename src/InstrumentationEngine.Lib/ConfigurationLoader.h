// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "InstrumentationMethod.h"

#define ConfigurationLoaderFieldMaxLength 1024

class CConfigurationLoader
{
public:
    HRESULT LoadConfiguration(_In_ BSTR bstrConfigPath, _In_ std::vector<CInstrumentationMethod*>& methods);
};

#ifndef PLATFORM_UNIX
class CConfigurationLoaderHelper
{
public:
    HRESULT LoadConfiguration(_In_ BSTR bstrConfigPath, _In_ std::vector<CInstrumentationMethod*>& methods);

private:
    HRESULT ProcessInstrumentationMethodNode(_In_ BSTR bstrInstrumentationMethodFolder, _In_ IXMLDOMNode* pNode, _In_ std::vector< CInstrumentationMethod* >& methods);
};
#else

#include <libxml/parser.h>
#include <libxml/tree.h>

class CConfigurationLoaderHelper
{
public:
    HRESULT LoadConfiguration(_In_ BSTR bstrConfigPath, _In_ std::vector<CInstrumentationMethod*>& methods);

private:
    HRESULT ProcessInstrumentationMethodNode(_In_ BSTR bstrInstrumentationMethodFolder, _In_ xmlDoc* pDocument, _In_ xmlNode* pElement, _In_ std::vector<CInstrumentationMethod*>& methods);
};
#endif