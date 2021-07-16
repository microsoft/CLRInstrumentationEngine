// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "InstrumentationMethod.h"
#include "../Common.Lib/XmlDocWrapper.h"
#include "ConfigurationLoader.h"

HRESULT CConfigurationLoader::LoadConfiguration(_In_ BSTR bstrConfigPath, _In_ std::vector<CInstrumentationMethod*>& methods)
{
    // Creates a new instance of the Helper for each config read.
    CConfigurationLoaderHelper loaderHelper;
    return loaderHelper.LoadConfiguration(bstrConfigPath, methods);
}

HRESULT CConfigurationLoaderHelper::LoadConfiguration(_In_ BSTR bstrConfigPath, _In_ std::vector<CInstrumentationMethod*>& methods)
{
    //<InstrumentationEngineConfiguration>
    //    <InstrumentationMethod>
    //        <Name>Squid Instrumentation</Name>
    //        <Description>Dynamically make squids swim</Description>
    //        <Module>SeafoodInstrumentation.dll</Module>
    //        <ClassGuid>{249E89A6-12D9-4E03-82FF-7FEAA41310E9}</ClassGuid>
    //        <Priority>50</Priority>
    //    </InstrumentationMethod>
    //</InstrumentationEngineConfiguration>

    HRESULT hr = S_OK;

    CComPtr<CXmlDocWrapper> pDocument;
    pDocument.Attach(new CXmlDocWrapper());
    IfFailRet(pDocument->LoadFile(bstrConfigPath));

    CComPtr<CXmlNode> pDocumentNode;
    IfFailRet(pDocument->GetRootNode(&pDocumentNode));

    tstring documentNodeName;
    IfFailRet(pDocumentNode->GetName(documentNodeName));

    if (wcscmp(documentNodeName.c_str(), _T("InstrumentationEngineConfiguration")) != 0)
    {
        CLogging::LogError(_T("Invalid configuration. Root element should be InstrumentationEngineConfiguration"));
        return E_FAIL;
    }

    // Configs that define the path to Instrumentation method dlls are searched relative to the config file.
    WCHAR wszConfigFolder[MAX_PATH];
    errno_t error = wcscpy_s(wszConfigFolder, MAX_PATH, bstrConfigPath);
    if (error != 0)
    {
        CLogging::LogError(_T("Copy failed in CConfigurationLoader::LoadConfiguration. Errno=%d"), error);
        return E_FAIL;
    }
    wszConfigFolder[MAX_PATH - 1] = 0;

    WCHAR* pFileName = PathFindFileName(wszConfigFolder);
    *pFileName = L'\0';

    CComPtr<CXmlNode> pCurrChildNode;
    IfFailRet(pDocumentNode->GetChildNode(&pCurrChildNode));

    while (pCurrChildNode != nullptr)
    {
        tstring currNodeName;
        IfFailRet(pCurrChildNode->GetName(currNodeName));

#ifdef X86
        auto InstrumentationEnginePlatformNode = _T("InstrumentationMethod32");
#else
        auto InstrumentationEnginePlatformNode = _T("InstrumentationMethod64");
#endif

        if ((wcscmp(currNodeName.c_str(), _T("InstrumentationMethod")) != 0) && (wcscmp(currNodeName.c_str(), InstrumentationEnginePlatformNode) != 0))
        {
            CLogging::LogError(_T("Invalid configuration. Element should be InstrumentationMethod, InstrumentationMethod32 or InstrumentationMethod64"));
            return E_FAIL;
        }

        IfFailRet(ProcessInstrumentationMethodNode(wszConfigFolder, pCurrChildNode, methods));
        CXmlNode* next = pCurrChildNode->Next();
        pCurrChildNode.Release();
        pCurrChildNode.Attach(next);
    }

    return hr;
}

HRESULT CConfigurationLoaderHelper::ProcessInstrumentationMethodNode(_In_ BSTR bstrInstrumentationMethodFolder, _In_ CXmlNode* pNode, _In_ std::vector<CInstrumentationMethod*>& methods)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pNode);

    CComPtr<CXmlNode> pChildNode;
    IfFailRet(pNode->GetChildNode(&pChildNode));

    tstring name;
    tstring description;
    tstring module;
    tstring classGuid;
    DWORD dwPriority = (DWORD)-1;

    while (pChildNode != nullptr)
    {
        tstring currNodeName;
        IfFailRet(pChildNode->GetName(currNodeName));

        if (wcscmp(currNodeName.c_str(), _T("Name")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            if (pChildValue != nullptr)
            {
                IfFailRet(pChildValue->GetStringValue(name));
            }
        }
        else if (wcscmp(currNodeName.c_str(), _T("Description")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            if (pChildValue != nullptr)
            {
                CComVariant varNodeValue;
                pChildValue->GetStringValue(description);
            }
        }
        else if (wcscmp(currNodeName.c_str(), _T("Module")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));

            if (pChildValue != nullptr)
            {
                IfFailRet(pChildValue->GetStringValue(module));
            }
        }
        else if (wcscmp(currNodeName.c_str(), _T("ClassGuid")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            IfFailRet(pChildNode->GetChildNode(&pChildValue));
            if (pChildValue != nullptr)
            {
                IfFailRet(pChildValue->GetStringValue(classGuid));
            }
        }
        else if (wcscmp(currNodeName.c_str(), _T("Priority")) == 0)
        {
            CComPtr<CXmlNode> pChildValue;
            pChildNode->GetChildNode(&pChildValue);

            if (pChildValue != nullptr)
            {
                tstring strNodeValue;
                IfFailRet(pChildValue->GetStringValue(strNodeValue));
                dwPriority = (DWORD)(_wtoi(strNodeValue.c_str()));
                if (errno == ERANGE)
                {
                    CLogging::LogError(_T("Invalid configuration. Priority should be a positive number"));
                    return E_FAIL;
                }
            }
        }
        else
        {
            CLogging::LogError(_T("Invalid configuration. Unknown Element"));
            return E_FAIL;
        }

        CXmlNode* next = pChildNode->Next();
        pChildNode.Release();
        pChildNode.Attach(next);
    }

    if ((name.length() == 0) ||
        (description.length() == 0) ||
        (module.length() == 0) ||
        (classGuid.length() == 0))
    {
        CLogging::LogError(_T("Invalid configuration. Missing child element"));
        return E_FAIL;
    }

    GUID guidClassId;
    CComBSTR bstrClassGuid = classGuid.c_str();
    hr = IIDFromString(bstrClassGuid, (LPCLSID)&guidClassId);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - Bad classid for instrumentation method"));
        return E_INVALIDARG;
    }

    CInstrumentationMethod* method = new CInstrumentationMethod(bstrInstrumentationMethodFolder, name.c_str(), description.c_str(), module.c_str(), guidClassId, dwPriority);
    methods.push_back(method);

    return S_OK;
}
