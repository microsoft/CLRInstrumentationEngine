// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "XmlNode.h"
#include "systemstring.h"

using namespace std;

namespace CommonLib
{
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

    HRESULT CXmlNode::GetName(_Inout_ tstring& name)
    {
        HRESULT hr = S_OK;

        IfNullRet(m_pNode);
        name = _T("");

    #ifndef PLATFORM_UNIX
        CComBSTR pName;
        IfFailRet(m_pNode->get_nodeName(&pName));
        name = pName.m_str;

    #else
        // LibXML2 will return an empty string for comment node names so we mark it as '#comment'.
        if (m_pNode->type == XML_COMMENT_NODE)
        {
            name = _T("#comment");
        }
        else
        {
            IfFailRet(SystemString::Convert((const char*)m_pNode->name, name));
        }
    #endif

        return S_OK;
    }

    HRESULT CXmlNode::GetStringValue(_Inout_ tstring& value)
    {
        HRESULT hr = S_OK;

        IfNullRet(m_pNode);
        value = _T("");
    #ifndef PLATFORM_UNIX
        CComVariant varNodeValue;
        IfFailRet(m_pNode->get_nodeValue(&varNodeValue));
        if (varNodeValue.bstrVal == nullptr)
        {
            return E_UNEXPECTED;
        }

        value = varNodeValue.bstrVal;
    #else
        IfFailRet(SystemString::Convert((const char*)m_pNode->content, value));
    #endif

        return S_OK;
    }

    HRESULT CXmlNode::GetAttribute(_In_ LPCWSTR wszAttributeName, _Inout_ tstring& value)
    {
        HRESULT hr = S_OK;

        IfNullRet(m_pNode);
        value = _T("");

    #ifndef PLATFORM_UNIX

        // Get attributes
        CComPtr<IXMLDOMNamedNodeMap> pAttributes;
        IfFailRet(m_pNode->get_attributes(&pAttributes));

        // Get specific attribute by name.
        CComPtr<IXMLDOMNode> pAttributeNode;
        pAttributes->getNamedItem(const_cast<BSTR>(wszAttributeName), &pAttributeNode);

        if (pAttributeNode != nullptr)
        {
            // Get the attribute value.
            CComVariant varValueAttr;
            pAttributeNode->get_nodeValue(&varValueAttr);
            value = varValueAttr.bstrVal;
            return S_OK;
        }

    #else
        string attrName;
        IfFailRet(SystemString::Convert(wszAttributeName, attrName));

        struct xmlHolder
        {
            xmlChar* m_value = nullptr;
            xmlHolder(xmlChar* value) : m_value(value) {}
            ~xmlHolder()
            {
                if (m_value != nullptr)
                {
                    xmlFree(m_value);
                }
            }
        };

        xmlHolder utf8AttrValue(xmlGetProp(m_pNode, (const xmlChar *)attrName.c_str()));

        if (utf8AttrValue.m_value != nullptr)
        {
            IfFailRet(SystemString::Convert((const char*)utf8AttrValue.m_value, value));
            return S_OK;
        }
    #endif

        return E_FAIL;
    }
}