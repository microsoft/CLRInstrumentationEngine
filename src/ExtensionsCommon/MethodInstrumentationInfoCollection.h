// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "NativeInstanceMethodInstrumentationInfo.h"

namespace Agent
{
namespace Instrumentation
{
    class CMethodInstrumentationInfoKey;

	class CMethodInstrumentationInfoCollection final
	{
		typedef std::map <
			CMethodInstrumentationInfoKey,
			Native::CNativeInstanceMethodInstrumentationInfoSptr> MethodTable;

		MethodTable			m_mapMethodTable;
		std::mutex			m_mtxLock;

		CMethodInstrumentationInfoCollection& operator=(const CMethodInstrumentationInfoCollection&) = delete;
		CMethodInstrumentationInfoCollection(const CMethodInstrumentationInfoCollection&) = delete;

		AGENT_DECLARE_NO_LOGGING;
	public:
		CMethodInstrumentationInfoCollection() noexcept;
		virtual ~CMethodInstrumentationInfoCollection();

		HRESULT Add(
			_In_ const Native::CNativeInstanceMethodInstrumentationInfoSptr& spItem);

		HRESULT Get(
			const CMethodInstrumentationInfoKey& key,
			_Out_ Native::CNativeInstanceMethodInstrumentationInfoSptr& spResult);
	};

	typedef std::shared_ptr<CMethodInstrumentationInfoCollection> CMethodInstrumentationInfoCollectionSptr;
}
}