// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"
#include "XmlNode.h"

class CXmlDocWrapper : CModuleRefCount
{
public:
    CXmlDocWrapper();

    HRESULT LoadContent(_In_ LPCWSTR wszValue, _In_ UINT cbValue);
    HRESULT GetRootNode(_Out_ CXmlNode** ppNode);

    DEFINE_DELEGATED_REFCOUNT_ADDREF(CXmlDocWrapper);
    DEFINE_DELEGATED_REFCOUNT_RELEASE(CXmlDocWrapper);

private:
#ifndef PLATFORM_UNIX
    CComPtr<IXMLDOMDocument2> m_pDocument;
#else
    class CCleanXmlDocument
    {
    private:
        xmlDoc* m_pDocument;
    public:
        CCleanXmlDocument()
            : m_pDocument(nullptr)
        {}

        ~CCleanXmlDocument()
        {
            Free();
        }

        void Attach(xmlDoc* pDocument)
        {
            if (m_pDocument != nullptr)
            {
                Free();
            }

            m_pDocument = pDocument;
        }

        void Free()
        {
            xmlFreeDoc(m_pDocument);

            // CONSIDER: is this safe? What if someone else in the process is using the parser?
            xmlCleanupParser();
        }
    };
    CCleanXmlDocument m_cleanupDoc;

    xmlDoc* m_pDocument;
#endif
};