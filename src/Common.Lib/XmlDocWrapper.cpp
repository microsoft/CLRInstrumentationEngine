// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "XmlDocWrapper.h"
#include "systemstring.h"

#ifndef PLATFORM_UNIX
using namespace ATL;
#else
#include <string>
using namespace std;
#endif

namespace CommonLib
{
    HRESULT CXmlDocWrapper::LoadFile(_In_ LPCWSTR wszFile)
    {
        return Load(wszFile, true);
    }

    HRESULT CXmlDocWrapper::LoadContent(_In_ LPCWSTR wszValue)
    {
        return Load(wszValue, false);
    }

    HRESULT CXmlDocWrapper::Load(_In_ LPCWSTR wszValue, _In_ bool isFile)
    {
        HRESULT hr = S_OK;

        if (m_pDocument != nullptr)
        {
            AssertLogFailure(_T("Attempted to load multiple xml files into a single document."));
            return E_FAIL;
        }

    #ifndef PLATFORM_UNIX
        // IMPORTANT: Default configuration here is safe from XXE attacks by default in MSXML 6.0 (which we are using), but not MSXML 3.0!
        CComPtr<IXMLDOMDocument3> pDocument;
        hr = CoCreateInstance(CLSID_FreeThreadedDOMDocument60, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument3, (void**)&pDocument);
        if (FAILED(hr))
        {
            AssertLogFailure(_T("Failed to create xml document"));
            return E_FAIL;
        }

        IfFailLog(pDocument->put_async(VARIANT_FALSE));
        IfFailLog(pDocument->put_validateOnParse(VARIANT_FALSE));
        IfFailLog(pDocument->put_resolveExternals(VARIANT_TRUE));

        VARIANT_BOOL vbResult = VARIANT_TRUE;
        if (isFile)
        {
            hr = pDocument->load(CComVariant(CComBSTR(wszValue)), &vbResult);
        }
        else
        {
            hr = pDocument->loadXML(CComBSTR(wszValue), &vbResult);
        }

        if (FAILED(hr) || vbResult == VARIANT_FALSE)
        {
            CComPtr<IXMLDOMParseError> pError;
            pDocument->get_parseError(&pError);

            if (pError != nullptr)
            {
                CComBSTR bstrReason;
                CComBSTR bstrSrcText;
                long lineNumber = 0;

                if (!SUCCEEDED(pError->get_reason(&bstrReason)))
                {
                    bstrReason = _T("unknown");
                }

                if (!SUCCEEDED(pError->get_srcText(&bstrSrcText)))
                {
                    bstrSrcText = _T("unknown");
                }

                if (!SUCCEEDED(pError->get_line(&lineNumber)))
                {
                    lineNumber = 0;
                }

                WCHAR wszLineNumber[33];
                _itow_s(lineNumber, wszLineNumber, 33, 10);

                tstring errorText = _T("Failed to load xml content.\r\n");

                if (bstrReason != nullptr)
                {
                    errorText.append(_T("\r\nReason:"));
                    errorText.append(bstrReason);
                }

                if (bstrSrcText != nullptr)
                {
                    errorText.append(_T("Line Text:"));
                    errorText.append(bstrSrcText);
                }

                errorText.append(_T("\r\nLine Number:"));
                errorText.append(wszLineNumber);

                AssertLogFailure(errorText.c_str());
            }
            else
            {
                AssertLogFailure(_T("Failed to load xml file."));
            }

            return E_FAIL;
        }
    #else
        LIBXML_TEST_VERSION

        string utf8Value;
        IfFailRet(SystemString::Convert(wszValue, utf8Value));
        xmlDoc* pDocument = nullptr;

        if (isFile)
        {
            pDocument = xmlReadFile(utf8Value.c_str(), NULL, XML_PARSE_NOBLANKS);
        }
        else
        {
            pDocument = xmlReadMemory(
                utf8Value.c_str(),   // buffer
                utf8Value.length(),  // size of the buffer
                "",             // the base URL to use for the document
                NULL,           // document encoding
                XML_PARSE_NOBLANKS               // xmlParserOption
            );
        }

        IfNullRet(pDocument);

        m_cleanupDoc.Attach(pDocument);
    #endif

        m_pDocument = pDocument;

        return S_OK;
    }

    HRESULT CXmlDocWrapper::GetRootNode(_Out_ CXmlNode** ppNode)
    {
        HRESULT hr = S_OK;

        IfNullRet(ppNode);
        *ppNode = nullptr;

    #ifndef PLATFORM_UNIX
        CComPtr<IXMLDOMElement> pDocumentElement;
        IfFailRet(m_pDocument->get_documentElement(&pDocumentElement));
        *ppNode = new CXmlNode(pDocumentElement);
    #else
        xmlNode* pRoot = xmlDocGetRootElement(m_pDocument);
        IfNullRet(pRoot);
        *ppNode = new CXmlNode(pRoot);
    #endif

        return S_OK;
    }

    #ifdef PLATFORM_UNIX
    void CXmlDocWrapper::CCleanXmlDocument::Attach(xmlDoc* pDocument)
    {
        if (m_pDocument != nullptr)
        {
            Free();
        }

        m_pDocument = pDocument;
    }

    void CXmlDocWrapper::CCleanXmlDocument::Free()
    {
        xmlFreeDoc(m_pDocument);

        // CONSIDER: is this safe? What if someone else in the process is using the parser?
        xmlCleanupParser();
    }
    #endif
}
