// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "XmlDocWrapper.h"
#include "Encoding.h"
#include "../Common.Lib/StringUtils.h"

using namespace ATL;

CXmlDocWrapper::CXmlDocWrapper()
#ifdef PLATFORM_UNIX
    : m_cleanupDoc(),
    m_pDocument(nullptr)
#endif
{

}

HRESULT CXmlDocWrapper::LoadContent(_In_ LPCWSTR wszValue)
{
    HRESULT hr = S_OK;

    if (m_pDocument != nullptr)
    {
        CLogging::LogError(_T("Attempted to load multiple xml files into a single document."));
        return E_FAIL;
    }

#ifndef PLATFORM_UNIX
    // IMPORTANT: Default configuration here is safe from XXE attacks by default in MSXML 6.0 (which we are using), but not MSXML 3.0!
    CComPtr<IXMLDOMDocument2> pDocument;
    hr = CoCreateInstance(CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument2, (void**)&pDocument);
    if (FAILED(hr))
    {
        CLogging::LogError(_T("Failed to create xml document"));
        return E_FAIL;
    }

    IfFailLog(pDocument->put_async(VARIANT_FALSE));
    IfFailLog(pDocument->put_validateOnParse(VARIANT_FALSE));
    IfFailLog(pDocument->put_resolveExternals(VARIANT_TRUE));

    VARIANT_BOOL vbResult = VARIANT_TRUE;
    hr = pDocument->loadXML(CComBSTR(wszValue), &vbResult);
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

            CLogging::LogError(errorText.c_str());
        }
        else
        {
            CLogging::LogError(_T("Failed to load xml file."));
        }

        return E_FAIL;
    }
#else
    LIBXML_TEST_VERSION

    CAutoVectorPtr<char> utf8Value;
    CEncoding::ConvertUtf16ToUtf8(wszValue, utf8Value);

    size_t utf8BufLen = 0;
    IfFailRet(StringUtils::StringLen(utf8Value.m_p, utf8BufLen));

    xmlDoc* pDocument = xmlReadMemory(
        utf8Value.m_p,  // buffer
        utf8BufLen,     // size of the buffer
        "",             // the base URL to use for the document
        NULL,           // document encoding
        0               // xmlParserOption
    );

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
