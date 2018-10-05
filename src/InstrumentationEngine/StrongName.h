// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include <windows.h>
#include <strongname.h>

BOOLEAN (__stdcall *GetStrongNameTokenFromAssemblyPtr())(LPCWSTR, BYTE**, ULONG*);
BOOLEAN (__stdcall *GetStrongNameTokenFromPublicKeyPtr())(BYTE*, ULONG, BYTE**, ULONG*);
DWORD(__stdcall *GetStrongNameErrorInfoPtr())();
VOID (__stdcall *GetStrongNameFreeBufferPtr())(BYTE*);
