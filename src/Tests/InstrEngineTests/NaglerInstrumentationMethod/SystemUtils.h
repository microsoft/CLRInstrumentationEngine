// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// SystemUtils.h: utilities for accessing OS info.
#pragma once
namespace NaglerSystemUtils
{
    HRESULT GetEnvVar(_In_z_ const WCHAR* wszVar, _Inout_ tstring& value);
}