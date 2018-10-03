// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// afxv_cpu.h - target version/configuration control for various CPUs

/////////////////////////////////////////////////////////////////////////////

// Why do we use #pragma pack at all?
#ifdef _ARM_
// specific overrides for ARM...
#define _AFX_PACKING    8       // default ARM alignment (required)
#endif //_MIPS_

/////////////////////////////////////////////////////////////////////////////
#ifdef _AMD64_
// specific overrides for AMD64...
#define _AFX_PACKING    8
#endif //_AMD64_
