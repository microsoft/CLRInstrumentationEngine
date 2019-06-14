// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include <atlcom.h>
#include <atlbase.h>

extern const GUID GUID_DataItemContainer;

/// <summary>
/// Class for reading/writing from and to IDataContainer
/// </summary>
template<
	typename	TDataItem,
	typename	TInterfacePtr,
	const GUID*	DataItemId,
	const GUID* ComponentId = &GUID_DataItemContainer
>
class CDataContainerAdapterImplT : virtual public CProfilerHostServices
{
public:
	CDataContainerAdapterImplT() noexcept {}
	virtual ~CDataContainerAdapterImplT(){}

	HRESULT SetDataItem(
		const TInterfacePtr&	spMethodInfo,
		const TDataItem&		dataItem) const
	{
		IDataContainerSptr sptrIDataContainer;
		IfFailRet(spMethodInfo->QueryInterface(&sptrIDataContainer));

		ATL::CComPtr<ATL::CComObject<CDataItemContainer<TDataItem>>> sptrObject;
		ATL::CComObject<CDataItemContainer<TDataItem>>::CreateInstance(&sptrObject);
		// ! CreateInstance does not increment ref count
		sptrObject.p->AddRef();

		sptrObject->SetDataItem(dataItem);

		auto pUnknown = static_cast<IUnknown*>(sptrObject.p);
		IfFailRet(
			sptrIDataContainer->SetDataItem(
				ComponentId,
				DataItemId,
				pUnknown));

		return S_OK;
	}

	_Success_(return == 0)
	HRESULT GetDataItem(
		const TInterfacePtr&	spMethodInfo,
		_Out_ TDataItem&		dataItem) const
	{
		IDataContainerSptr spIDataContainer;
		IfFailRet(spMethodInfo->QueryInterface(&spIDataContainer));

		ATL::CComPtr<CDataItemContainer<TDataItem>> sptrObject;
		HRESULT hrFound =
			spIDataContainer->GetDataItem(
				ComponentId,
				DataItemId,
				reinterpret_cast<IUnknown**>(&sptrObject));

		if (SUCCEEDED(hrFound))
		{
			dataItem = sptrObject->GetDataItem();

			return S_OK;
		}
		else
		{
			return S_FALSE;
		}
	}
};