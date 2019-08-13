// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <atlcom.h>
#include <atlbase.h>


// {BF49171A-FC33-4BA7-92F3-A88859B53A78}
extern const GUID GUID_CDatItemContainer;

/*
* <summary>
* COM class-wrapper on C++ classes.
* Usage:
*		ATL::CComPtr<ATL::CComObject<CDataItemContainer>> sptrObject;
*		ATL::CComObject<CDataItemContainer>::CreateInstance(&sptrObject)
*		// ! CreateInstance does not increment ref count
*		sptrObject.p->AddRef();
* </summary>
**/
template<typename T>
class ATL_NO_VTABLE CDataItemContainer :
	public ATL::CComObjectRootEx<ATL::CComMultiThreadModel>,
	public ATL::CComCoClass<CDataItemContainer<T>, &GUID_CDatItemContainer>,
	public IUnknown
{
	T m_dataItem;
public:
	DECLARE_NOT_AGGREGATABLE(CDataItemContainer)
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CDataItemContainer)
		COM_INTERFACE_ENTRY(IUnknown)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	void SetDataItem(const T& dataItem)
	{
		this->m_dataItem = dataItem;
	}

	T GetDataItem()
	{
		return  this->m_dataItem;
	}

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
};


