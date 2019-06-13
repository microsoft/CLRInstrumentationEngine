// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "ExtensionsCommon\MethodInfoDataContainerAdatpter.h"

namespace Agent
{
	namespace Interop
	{
		//NOTE: IT IS IMPORTANT THAT EVERY INSTRUMENTATION METHOD DEFINES IT'S OWN GUID.
		//DO NOT MOVE THIS FILE TO COMMON FOLDER

		// GUID for identifying data while reading/writing from/to IMethodInfoSptr.
		// We store a MethodRecordSptr in IMethodInfoSptr between InternalShouldInstrumentMethod and InternalInstrumentMethod.
		// {5E05BE46-7A1A-4B8F-A5F2-F594A40DDEB7}
		static const GUID GUID_MethodInfoDataItem =
			{ 0x5e05be46, 0x7a1a, 0x4b8f,{ 0xa5, 0xf2, 0xf5, 0x94, 0xa4, 0xd, 0xde, 0xb7 } };
	}
}