// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ConfigurationLoader.h"
#include "InstrumentationMethod.h"

HRESULT CConfigurationLoader::LoadConfiguration(_In_ BSTR bstrConfigPath, _In_ std::vector<CInstrumentationMethod*>& methods)
{
    // Creates a new instance of the Helper for each config read.
    CConfigurationLoaderHelper loaderHelper;
    return loaderHelper.LoadConfiguration(bstrConfigPath, methods);
}

#ifndef PLATFORM_UNIX
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
    CComPtr<IXMLDOMDocument3> pDocument;
    IfFailRet(CoCreateInstance(CLSID_FreeThreadedDOMDocument60, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument3, (void**)&pDocument));

    VARIANT_BOOL vbResult = VARIANT_TRUE;
    hr = pDocument->load(CComVariant(bstrConfigPath), &vbResult);
    if (FAILED(hr) || vbResult != VARIANT_TRUE)
    {
        CLogging::LogError(_T("Failed to load the configuration"));
        return E_FAIL;
    }

    CComPtr<IXMLDOMElement> pDocumentNode;
    IfFailRet(pDocument->get_documentElement(&pDocumentNode));


    CComBSTR bstrDocumentNodeName;
    IfFailRet(pDocumentNode->get_nodeName(&bstrDocumentNodeName));

    if (wcscmp(bstrDocumentNodeName, _T("InstrumentationEngineConfiguration")) != 0)
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

    CComPtr<IXMLDOMNodeList> pChildNodes;
    IfFailRet(pDocumentNode->get_childNodes(&pChildNodes));

    long cChildren = 0;
    pChildNodes->get_length(&cChildren);

    for (long i = 0; i < cChildren; i++)
    {
        CComPtr<IXMLDOMNode> pCurrChildNode;
        IfFailRet(pChildNodes->get_item(i, &pCurrChildNode));

        CComBSTR bstrCurrNodeName;
        IfFailRet(pCurrChildNode->get_nodeName(&bstrCurrNodeName));

#ifdef X86
        auto InstrumentationEnginePlatformNode = _T("InstrumentationMethod32");
#else
        auto InstrumentationEnginePlatformNode = _T("InstrumentationMethod64");
#endif

        if ((wcscmp(bstrCurrNodeName, _T("InstrumentationMethod")) != 0) && (wcscmp(bstrCurrNodeName, InstrumentationEnginePlatformNode) != 0))
        {
            CLogging::LogError(_T("Invalid configuration. Element should be InstrumentationMethod, InstrumentationMethod32 or InstrumentationMethod64"));
            return E_FAIL;
        }

        IfFailRet(ProcessInstrumentationMethodNode(wszConfigFolder, pCurrChildNode, methods));
    }

    return hr;
}

HRESULT CConfigurationLoaderHelper::ProcessInstrumentationMethodNode(_In_ BSTR bstrInstrumentationMethodFolder, _In_ IXMLDOMNode* pNode, _In_ std::vector<CInstrumentationMethod*>& methods)
{
    HRESULT hr = S_OK;
    IfNullRetPointer(pNode);

    CComPtr<IXMLDOMNodeList> pChildNodes;
    IfFailRet(pNode->get_childNodes(&pChildNodes));

    long cChildren = 0;
    pChildNodes->get_length(&cChildren);

    CComBSTR bstrName;
    CComBSTR bstrDescription;
    CComBSTR bstrModule;
    CComBSTR bstrClassGuid;
    DWORD dwPriority = (DWORD)-1;

    for (long i = 0; i < cChildren; i++)
    {
        CComPtr<IXMLDOMNode> pChildNode;
        IfFailRet(pChildNodes->get_item(i, &pChildNode));

        CComBSTR bstrCurrNodeName;
        IfFailRet(pChildNode->get_nodeName(&bstrCurrNodeName));

        if (wcscmp(bstrCurrNodeName, _T("Name")) == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            if (pChildValue != nullptr)
            {
                CComVariant varNodeValue;
                pChildValue->get_nodeValue(&varNodeValue);
                bstrName = varNodeValue.bstrVal;
            }
        }
        else if (wcscmp(bstrCurrNodeName, _T("Description")) == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            if (pChildValue != nullptr)
            {
                CComVariant varNodeValue;
                pChildValue->get_nodeValue(&varNodeValue);
                bstrDescription = varNodeValue.bstrVal;
            }
        }
        else if (wcscmp(bstrCurrNodeName, _T("Module")) == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            if (pChildValue != nullptr)
            {
                CComVariant varNodeValue;
                pChildValue->get_nodeValue(&varNodeValue);
                bstrModule = varNodeValue.bstrVal;
            }
        }
        else if (wcscmp(bstrCurrNodeName, _T("ClassGuid")) == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            if (pChildValue != nullptr)
            {
                CComVariant varNodeValue;
                pChildValue->get_nodeValue(&varNodeValue);
                bstrClassGuid = varNodeValue.bstrVal;
            }
        }
        else if (wcscmp(bstrCurrNodeName, _T("Priority")) == 0)
        {
            CComPtr<IXMLDOMNode> pChildValue;
            pChildNode->get_firstChild(&pChildValue);

            if (pChildValue != nullptr)
            {
                CComVariant varNodeValue;
                pChildValue->get_nodeValue(&varNodeValue);

                dwPriority = (DWORD)(_wtoi64(varNodeValue.bstrVal));
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
    }

    if ((bstrName.Length() == 0) ||
        (bstrDescription.Length() == 0) ||
        (bstrModule.Length() == 0) ||
        (bstrClassGuid.Length() == 0))
    {
        CLogging::LogError(_T("Invalid configuration. Missing child element"));
        return E_FAIL;
    }

    GUID guidClassId;
    hr = IIDFromString(bstrClassGuid, (LPCLSID)&guidClassId);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - Bad classid for instrumentation method"));
        return E_INVALIDARG;
    }

    CInstrumentationMethod* method = new CInstrumentationMethod(bstrInstrumentationMethodFolder, bstrName, bstrDescription, bstrModule, guidClassId, dwPriority);
    methods.push_back(method);

    return S_OK;
}

#else

#include <string.h>

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

    LIBXML_TEST_VERSION

    // Convert utf16 bstrConfigPath to utf-8
    CW2A configFile(bstrConfigPath, CP_UTF8);
    xmlDoc* pDocument = xmlReadFile(configFile, NULL, 0);

    class CCleanXmlDocument
    {
    private:
        xmlDoc* m_pDocument;
    public:
        CCleanXmlDocument(xmlDoc* pDocument) : m_pDocument(pDocument) {}

        ~CCleanXmlDocument()
        {
            xmlFreeDoc(m_pDocument);

            // CONSIDER: is this safe? What if someone else in the process is using the parser?
            xmlCleanupParser();
        }
    };
    CCleanXmlDocument cleanupDoc(pDocument);

    xmlNode* pRoot = xmlDocGetRootElement(pDocument);

    if (pRoot == NULL || pRoot->type != XML_ELEMENT_NODE)
    {
        CLogging::LogError(_T("Invalid configuration file."));
        return E_FAIL;
    }

    if ((strncmp((char*)pRoot->name, "InstrumentationEngineConfiguration", ConfigurationLoaderFieldMaxLength) != 0))
    {
        CLogging::LogError(_T("Invalid configuration. Root element should be InstrumentationEngineConfiguration"));
        return E_FAIL;
    }

    xmlNode* pCurrInstrumenationMethodNode;
    for (xmlNode* pCurrInstrumenationMethodNode = pRoot->children; pCurrInstrumenationMethodNode != NULL; pCurrInstrumenationMethodNode = pCurrInstrumenationMethodNode->next)
    {
        if (pCurrInstrumenationMethodNode->type == XML_ELEMENT_NODE)
        {
    #ifdef X86
            const char* InstrumentationEnginePlatformNode = "InstrumentationMethod32";
    #else
            const char* InstrumentationEnginePlatformNode = "InstrumentationMethod64";
    #endif

            if ((strncmp((char*)pCurrInstrumenationMethodNode->name, "InstrumentationMethod", ConfigurationLoaderFieldMaxLength) != 0) &&
                (strncmp((char*)pCurrInstrumenationMethodNode->name, InstrumentationEnginePlatformNode, ConfigurationLoaderFieldMaxLength) != 0))
            {
                CLogging::LogError(_T("Invalid configuration. Element should be InstrumentationMethod, InstrumentationMethod32 or InstrumentationMethod64"));
                return E_FAIL;
            }

            // For instrumentation methods loaded via config the assumption is that
            // they are in the same folder as their config.
            WCHAR wszConfigFolder[MAX_PATH];
            wcscpy_s(wszConfigFolder, MAX_PATH, bstrConfigPath);
            WCHAR* pFileName = PathFindFileName(wszConfigFolder);
            *pFileName = L'\0';

            IfFailRet(ProcessInstrumentationMethodNode(wszConfigFolder, pDocument, pCurrInstrumenationMethodNode, methods));
        }
    }

    return hr;
}

HRESULT GetBstrFromXmlString(char* utf8String, BSTR* ppbstrString)
{
    // Convert utf8 input string to utf-16
    iconv_t icnv = iconv_open("UTF-16LE", "UTF-8");

    size_t inputStringLength = strnlen((char*)utf8String, ConfigurationLoaderFieldMaxLength) + 1;

    // Utf-16 chars can be up to 4 bytes, so the max size is 4 times the utf-8 string
    size_t outputStringLen = inputStringLength * 4;
    unique_ptr<WCHAR[]> outputString(new WCHAR[outputStringLen]);

    // iconv is destructive on the string pointers. So, pass dummy values.
    char* pOriginal = (char*)utf8String;
    char* pNew = (char*)outputString.get();
    iconv(icnv, (char**)&pOriginal, &inputStringLength, &pNew, &outputStringLen);
    iconv_close(icnv);

    CComBSTR bstrOutput = outputString.get();
    *ppbstrString = bstrOutput.Detach();

    return S_OK;
}

HRESULT CConfigurationLoaderHelper::ProcessInstrumentationMethodNode(_In_ BSTR bstrInstrumentationMethodFolder, _In_ xmlDoc* pDocument, _In_ xmlNode* pElement, _In_ std::vector<CInstrumentationMethod*>& methods)
{
    HRESULT hr = S_OK;

    // Technically, the values being read via xml are utf-8. However,
    CComBSTR bstrName;
    CComBSTR bstrDescription;
    CComBSTR bstrModule;
    CComBSTR bstrClassGuid;
    DWORD dwPriority = (DWORD)-1;

    for (xmlNode* pCurrNode = pElement->children; pCurrNode != NULL; pCurrNode = pCurrNode->next)
    {
        if (pCurrNode->type != XML_ELEMENT_NODE)
        {
            continue;
        }

        char* pCurrNodeName = (char*)pCurrNode->name;

        if (strncmp(pCurrNodeName, "Name", ConfigurationLoaderFieldMaxLength) == 0)
        {
            xmlNode* pChildValue = pCurrNode->children;

            if (pChildValue != nullptr && pChildValue->type == XML_TEXT_NODE)
            {
                xmlChar* key = xmlNodeListGetString(pDocument, pChildValue, 1);
                hr = GetBstrFromXmlString((char*)key, &bstrName);
                xmlFree(key);
                IfFailRet(hr);
            }
        }
        else if (strncmp(pCurrNodeName, "Description", ConfigurationLoaderFieldMaxLength) == 0)
        {
            xmlNode* pChildValue = pCurrNode->children;

            if (pChildValue != nullptr && pChildValue->type == XML_TEXT_NODE)
            {
                xmlChar* key = xmlNodeListGetString(pDocument, pChildValue, 1);
                hr = GetBstrFromXmlString((char*)key, &bstrDescription);
                xmlFree(key);
                IfFailRet(hr);
            }
        }
        else if (strncmp(pCurrNodeName, "Module", ConfigurationLoaderFieldMaxLength) == 0)
        {
            xmlNode* pChildValue = pCurrNode->children;

            if (pChildValue != nullptr && pChildValue->type == XML_TEXT_NODE)
            {
                xmlChar* key = xmlNodeListGetString(pDocument, pChildValue, 1);
                hr = GetBstrFromXmlString((char*)key, &bstrModule);
                xmlFree(key);
                IfFailRet(hr);
            }
        }
        else if (strncmp(pCurrNodeName, "ClassGuid", ConfigurationLoaderFieldMaxLength) == 0)
        {
            xmlNode* pChildValue = pCurrNode->children;

            if (pChildValue != nullptr && pChildValue->type == XML_TEXT_NODE)
            {
                xmlChar* key = xmlNodeListGetString(pDocument, pChildValue, 1);
                hr = GetBstrFromXmlString((char*)key, &bstrClassGuid);
                xmlFree(key);
                IfFailRet(hr);
            }
        }
        else if (strncmp(pCurrNodeName, "Priority", ConfigurationLoaderFieldMaxLength) == 0)
        {
            xmlNode* pChildValue = pCurrNode->children;

            if (pChildValue != nullptr && pChildValue->type == XML_TEXT_NODE)
            {
                xmlChar* key = xmlNodeListGetString(pDocument, pChildValue, 1);
                dwPriority = (DWORD)(atoi((char*)key));
                xmlFree(key);

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
    }

    if ((bstrName.Length() == 0) ||
        (bstrDescription.Length() == 0) ||
        (bstrModule.Length() == 0) ||
        (bstrClassGuid.Length() == 0))
    {
        CLogging::LogError(_T("Invalid configuration. Missing child element"));
        return E_FAIL;
    }

    GUID guidClassId;
    hr = IIDFromString(bstrClassGuid, (LPCLSID)&guidClassId);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("CInstrumentationMethod::Initialize - Bad classid for instrumentation method"));
        return E_INVALIDARG;
    }

    CInstrumentationMethod* method = new CInstrumentationMethod(bstrInstrumentationMethodFolder, bstrName, bstrDescription, bstrModule, guidClassId, dwPriority);

    methods.push_back(method);

    return S_OK;
}


#endif
