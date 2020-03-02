// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "XmlNode.h"
#include "Encoding.h"

#ifndef PLATFORM_UNIX
CXmlNode::CXmlNode(_In_ IXMLDOMNode* pNode)
{
    m_pNode = pNode;
}
#else
CXmlNode::CXmlNode(_In_ xmlNode* pNode)
{
    m_pNode = pNode;
}
#endif

HRESULT CXmlNode::GetChildNode(_Out_ CXmlNode** ppNode)
{
    HRESULT hr = S_OK;

    IfNullRet(ppNode);
    IfNullRet(m_pNode);
    *ppNode = nullptr;

#ifndef PLATFORM_UNIX
    CComPtr<IXMLDOMNode> pNode;
    IfFailRet(m_pNode->get_firstChild(&pNode));
#else
    xmlNode* pNode = m_pNode->children;
#endif

    if (pNode != nullptr)
    {
        *ppNode = new CXmlNode(pNode);
    }

    return S_OK;
}

CXmlNode* CXmlNode::Next()
{
    if (m_pNode == nullptr)
    {
        return nullptr;
    }

#ifndef PLATFORM_UNIX
    CComPtr<IXMLDOMNode> pNextSibling;
    if (SUCCEEDED(m_pNode->get_nextSibling(&pNextSibling)) &&
        pNextSibling != nullptr)
    {
        return new CXmlNode(pNextSibling);
    }
#else
    xmlNode* pNextSibling = m_pNode->next;
    if (pNextSibling != nullptr)
    {
        return new CXmlNode(pNextSibling);
    }
#endif

    return nullptr;
}

HRESULT CXmlNode::GetName(_Out_ BSTR* pName)
{
    HRESULT hr = S_OK;

    IfNullRet(pName);
    IfNullRet(m_pNode);

#ifndef PLATFORM_UNIX
    IfFailRet(m_pNode->get_nodeName(pName));

    return S_OK;
#else
    return CEncoding::ConvertUtf8ToUtf16Bstr((const char*)m_pNode->name, pName);
#endif
}

HRESULT CXmlNode::GetStringValue(_Out_ BSTR* pValue)
{
    HRESULT hr = S_OK;

    IfNullRet(pValue);
    IfNullRet(m_pNode);

#ifndef PLATFORM_UNIX
    CComVariant varNodeValue;
    IfFailRet(m_pNode->get_nodeValue(&varNodeValue));

    CComBSTR bstrNodeValue = varNodeValue.bstrVal;
    *pValue = bstrNodeValue.Detach();

    return S_OK;

#else
    return CEncoding::ConvertUtf8ToUtf16Bstr((const char*)m_pNode->content, pValue);
#endif
}

HRESULT CXmlNode::GetAttribute(_In_ const WCHAR* wszAttributeName, _Out_ BSTR* pValue)
{
    HRESULT hr = S_OK;

    IfNullRet(pValue);
    IfNullRet(m_pNode);

#ifndef PLATFORM_UNIX

    // Get attributes
    CComPtr<IXMLDOMNamedNodeMap> pAttributes;
    IfFailRet(m_pNode->get_attributes(&pAttributes));

    // Get specific attribute by name.
    CComPtr<IXMLDOMNode> pAttributeNode;
    pAttributes->getNamedItem(const_cast<BSTR>(wszAttributeName), &pAttributeNode);

    // Get the attribute value.
    CComVariant varValueAttr;
    pAttributeNode->get_nodeValue(&varValueAttr);
    CComBSTR bstrAttrVal = varValueAttr.bstrVal;
    *pValue = bstrAttrVal.Detach();

    return S_OK;
#else
    CAutoVectorPtr<char> utf8AttrName;
    CEncoding::ConvertUtf16ToUtf8(wszAttributeName, utf8AttrName);
    xmlChar* utf8AttrValue = xmlGetProp(m_pNode, (const xmlChar *)utf8AttrName.m_p);
    //xmlChar* utf8AttrValue = xmlGetProp(m_pNode, (const xmlChar *)CW2A(wszAttributeName, CP_UTF8));

    if (utf8AttrValue != nullptr)
    {
        hr = CEncoding::ConvertUtf8ToUtf16Bstr((const char*)utf8AttrValue, pValue);
        xmlFree(utf8AttrValue);
        return hr;
    }
    else
    {
        // TODO: willxie do something better than this if no attribute found (ie. xmlGetProp() returns NULL)
        return E_FAIL;
    }
#endif
}