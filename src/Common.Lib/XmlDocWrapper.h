// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "XmlNode.h"

#ifndef PLATFORM_UNIX
    struct IXMLDOMDocument3;
#else
    typedef struct _xmlDoc xmlDoc;
#endif

namespace CommonLib
{
    class CXmlDocWrapper : CModuleRefCount
    {
    public:
        CXmlDocWrapper() = default;

        HRESULT LoadFile(_In_ LPCWSTR wszFile);
        HRESULT LoadContent(_In_ LPCWSTR wszValue);
        HRESULT GetRootNode(_Out_ CXmlNode** ppNode);

        DEFINE_DELEGATED_REFCOUNT_ADDREF(CXmlDocWrapper);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CXmlDocWrapper);

    private:

        HRESULT Load(_In_ LPCWSTR data, _In_ bool isFilePath);
    #ifndef PLATFORM_UNIX
        CComPtr<IXMLDOMDocument3> m_pDocument;
    #else
        class CCleanXmlDocument
        {
        private:
            xmlDoc* m_pDocument = nullptr;
        public:
            CCleanXmlDocument() = default;

            ~CCleanXmlDocument()
            {
                Free();
            }

            void Attach(xmlDoc* pDocument);
            void Free();
            
        };
        CCleanXmlDocument m_cleanupDoc;

        xmlDoc* m_pDocument = nullptr;
    #endif
    };
}