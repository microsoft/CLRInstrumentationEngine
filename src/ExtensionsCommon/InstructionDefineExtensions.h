// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

//those could also be or reference valuetype:
//ELEMENT_TYPE_GENERICINST
//ELEMENT_TYPE_MVAR
//ELEMENT_TYPE_VAR
//ELEMENT_TYPE_PTR:
//ELEMENT_TYPE_BYREF:

inline bool IsValueType(CorElementType cet)
{
	return ELEMENT_TYPE_BOOLEAN == cet
		|| ELEMENT_TYPE_CHAR == cet
		|| ELEMENT_TYPE_I1 == cet
		|| ELEMENT_TYPE_U1 == cet
		|| ELEMENT_TYPE_I2 == cet
		|| ELEMENT_TYPE_U2 == cet
		|| ELEMENT_TYPE_I4 == cet
		|| ELEMENT_TYPE_U4 == cet
		|| ELEMENT_TYPE_I8 == cet
		|| ELEMENT_TYPE_U8 == cet
		|| ELEMENT_TYPE_R4 == cet
		|| ELEMENT_TYPE_R8 == cet
		|| ELEMENT_TYPE_I == cet
		|| ELEMENT_TYPE_U == cet
		|| ELEMENT_TYPE_VALUETYPE == cet;
}

inline bool IsRefType(CorElementType cet)
{
	return ELEMENT_TYPE_STRING == cet
		|| ELEMENT_TYPE_CLASS == cet;
}


