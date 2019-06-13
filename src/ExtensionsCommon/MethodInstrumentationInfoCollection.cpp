// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"

#include "NativeInstanceMethodInstrumentationInfo.h"

#include "MethodInstrumentationInfoKey.h"
#include "MethodInstrumentationInfoCollection.h"


namespace Agent
{
namespace Instrumentation
{
	CMethodInstrumentationInfoCollection::CMethodInstrumentationInfoCollection() noexcept
	{
	}
	CMethodInstrumentationInfoCollection::~CMethodInstrumentationInfoCollection()
	{
	}

	HRESULT CMethodInstrumentationInfoCollection::Get(
		const CMethodInstrumentationInfoKey& key,
		_Out_ Native::CNativeInstanceMethodInstrumentationInfoSptr& spResult)
	{
		spResult = nullptr;

		std::lock_guard<decltype(m_mtxLock)> lock(m_mtxLock);

		auto itFound = m_mapMethodTable.find(key);
		if (m_mapMethodTable.end() != itFound)
		{
			spResult = itFound->second;

			return S_OK;
		}

		return S_FALSE;
	}

	HRESULT CMethodInstrumentationInfoCollection::Add(
		_In_ const Native::CNativeInstanceMethodInstrumentationInfoSptr& spItem)
	{
		IfTrueRet(nullptr == spItem, E_INVALIDARG);

		std::lock_guard<decltype(m_mtxLock)> lock(m_mtxLock);

		return m_mapMethodTable.insert(
			make_pair(
				CMethodInstrumentationInfoKey(*spItem), spItem)).second ? S_OK : S_FALSE;
	}
}
}