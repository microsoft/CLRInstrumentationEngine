// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "stdafx.h"

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
    HRESULT GetName(_Out_ BSTR* pName);

    // Element Value
    HRESULT GetStringValue(_Out_ BSTR* pValue);

    // Attribute Value
    HRESULT GetAttribute(_In_ const WCHAR* wszAttributeName, _Out_ BSTR* pValue);

    DEFINE_DELEGATED_REFCOUNT_ADDREF(CXmlNode);
    DEFINE_DELEGATED_REFCOUNT_RELEASE(CXmlNode);

private:
#ifndef PLATFORM_UNIX
    CComPtr<IXMLDOMNode> m_pNode;
#else
    xmlNode* m_pNode;
#endif
};