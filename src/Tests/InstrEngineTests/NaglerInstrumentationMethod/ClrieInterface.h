#pragma once
#include "stdafx.h"

typedef HRESULT (*fnFreeString)(BSTR bstr);

class CClrieInterface
{
public:
    static HRESULT FreeString()
};