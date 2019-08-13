// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DataContainerAdapter.h"

// GUID for identifying data while reading/writing from/to IMethodInfoSptr.
// IMethodInfoSptr uses 2 guids for better partitioning. Making the 1st one a constant for simplicity
static const GUID GUID_DataItemContainer =
{ 0xb9c9c7e1, 0xd20f, 0x4fe3, { 0xb7, 0x9e, 0x4b, 0xbc, 0xb8, 0xd3, 0x6e, 0x63 } };