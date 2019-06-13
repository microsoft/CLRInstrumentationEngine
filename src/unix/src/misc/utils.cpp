// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "pch.h"

extern "C" VOID DbgAssertDialog(const char *szFile, int iLine, const char *szExpr);

extern "C" VOID DbgAssertDialog(const char *szFile, int iLine, const char *szExpr)
{
    printf("ASSERT: %s:%d : %s", szFile, iLine, szExpr);
}
