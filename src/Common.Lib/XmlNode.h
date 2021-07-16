// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef PLATFORM_UNIX
#include <libxml/parser.h>
#include <libxml/tree.h>
#else
#include <msxml6.h>
#endif

#include "../Common.Headers/tstring.h"

namespace CommonLib
{
    class CXmlNode : CModuleRefCount
    {
    public:
    #ifndef PLATFORM_UNIX
        CXmlNode(_In_ IXMLDOMNode* pNode);
    #else
        CXmlNode(_In_ xmlNode* pNode);
    #endif

        HRESULT GetChildNode(_Out_ CXmlNode** ppNode);

        CXmlNode* Next();

        // Element Name
        HRESULT GetName(_Inout_ tstring& name);

        // Element Value
        HRESULT GetStringValue(_Inout_ tstring& value);

        // Attribute Value
        HRESULT GetAttribute(_In_ LPCWSTR wszAttributeName, _Inout_ tstring& value);

        DEFINE_DELEGATED_REFCOUNT_ADDREF(CXmlNode);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CXmlNode);

    private:
    #ifndef PLATFORM_UNIX
        CComPtr<IXMLDOMNode> m_pNode;
    #else
        xmlNode* m_pNode;
    #endif
    };
}