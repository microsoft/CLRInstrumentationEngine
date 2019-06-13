// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

#include "DecorationCallbackInfo.h"

class CDecorationCallbackRefInfo final
{
public:
	CDecorationCallbackRefInfo(
		mdMemberRef tkMethodRef,
		const CDecorationCallbackInfoSptr& sptrCallbackInfo)
		: m_tkMethodRef(tkMethodRef)
		, m_sptrCallbackInfo(sptrCallbackInfo)
	{
	}
	virtual ~CDecorationCallbackRefInfo() {}

	mdMemberRef GetMethodRef() const
	{
		return m_tkMethodRef;
	}

	CDecorationCallbackInfoSptr GetCallbackInfo() const
	{
		return m_sptrCallbackInfo;
	}
private:
	const mdMemberRef m_tkMethodRef;
	const CDecorationCallbackInfoSptr m_sptrCallbackInfo;
};

typedef std::shared_ptr<CDecorationCallbackRefInfo> CDecorationCallbackRefInfoSptr;