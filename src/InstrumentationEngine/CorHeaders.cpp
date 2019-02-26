// Copyright (c) Microsoft Corporation. All rights reserved.
//

////////////////////////////////////////////////////
// File:	CorHeaders.cpp
// Author:  Alex Mikunov
// Date:    8/02/2002
//
// Description: common helpers
//
// Comments:
//
// Version:	0.003
//
// Revisions:	3/27/2007
//			Jean-François Peyroux
////////////////////////////////////////////////////

#include "stdafx.h"

#include "CorHeaders.h"
#define VSASSERT(EXPR, text)
#define ASSERT(EXPR) VSASSERT(EXPR, L"");


//*****************************************************************************
//
//***** Cor Header helper classes
//
//*****************************************************************************

/***************************************************************************/
/* Note that this construtor does not set the LocalSig, but has the
   advantage that it does not have any dependancy on EE structures.
   inside the EE use the FunctionDesc constructor */

void __stdcall DecoderInit(COR_ILMETHOD_DECODER * pThis, const COR_ILMETHOD_FAT* header)
{
    memset(pThis, 0, sizeof(COR_ILMETHOD_DECODER));

    ASSERT ((((size_t) header) & 3) == 0);        // header is aligned
    * (COR_ILMETHOD_FAT*) pThis = * header;
    pThis->Code = header->GetCode();
    ASSERT (header->Size >= 3);        // Size if valid
    pThis->Sect = header->GetSect ();
    if (pThis->Sect != 0 && pThis->Sect->Kind() == CorILMethod_Sect_EHTable)
	{
        pThis->EH = (COR_ILMETHOD_SECT_EH*) (pThis)->Sect;
        pThis->Sect = (pThis)->Sect->Next ();
    }
    return;
    // so we don't asert on trash  ASSERT (!"Unknown format");
}

/*********************************************************************/
/* APIs for emitting sections etc */

unsigned __stdcall IlmethodSize(COR_ILMETHOD_FAT* header, BOOL moreSections)
{
    if (header->MaxStack <= 8 && (header->Flags & ~CorILMethod_FormatMask) == 0
        && header->LocalVarSigTok == 0 && header->CodeSize < 64 && !moreSections)
        return(sizeof(COR_ILMETHOD_TINY));

    return(sizeof(COR_ILMETHOD_FAT));
}

/*********************************************************************/
        // emit the header (bestFormat) return amount emitted
unsigned __stdcall IlmethodEmit(unsigned size, COR_ILMETHOD_FAT* header,
                  BOOL moreSections, _Out_cap_(size) BYTE* outBuff)
{
#ifdef _DEBUG
    BYTE* origBuff = outBuff;
#endif
    if (size == 1) {
            // Tiny format
        *outBuff++ = (BYTE) (CorILMethod_TinyFormat | (header->CodeSize << 2));
    }
    else {
            // Fat format
        ASSERT ((((size_t) outBuff) & 3) == 0);               // header is dword aligned
        COR_ILMETHOD_FAT* fatHeader = (COR_ILMETHOD_FAT*) outBuff;
        outBuff += sizeof(COR_ILMETHOD_FAT);
        *fatHeader = *header;
        fatHeader->Flags |= CorILMethod_FatFormat;
        ASSERT ((fatHeader->Flags & CorILMethod_FormatMask) == CorILMethod_FatFormat);
        if (moreSections)
            fatHeader->Flags |= CorILMethod_MoreSects;
        fatHeader->Size = sizeof(COR_ILMETHOD_FAT) / 4;
    }
    ASSERT (&origBuff[size] == outBuff);
    return(size);
}

/*********************************************************************/
/* static */
IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* __stdcall SectEH_EHClause(void *pSectEH, unsigned idx, IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* buff)
{
    if (((COR_ILMETHOD_SECT_EH *)pSectEH)->IsFat())
        return(&(((COR_ILMETHOD_SECT_EH *)pSectEH)->Fat.Clauses[idx]));

    // mask to remove sign extension - cast just wouldn't work
    buff->Flags         = (CorExceptionFlag)((((COR_ILMETHOD_SECT_EH *)pSectEH)->Small.Clauses[idx].Flags)&0x0000ffff);
    buff->ClassToken  = ((COR_ILMETHOD_SECT_EH *)pSectEH)->Small.Clauses[idx].ClassToken;
    buff->TryOffset     = ((COR_ILMETHOD_SECT_EH *)pSectEH)->Small.Clauses[idx].TryOffset;
    buff->TryLength     = ((COR_ILMETHOD_SECT_EH *)pSectEH)->Small.Clauses[idx].TryLength;
    buff->HandlerLength = ((COR_ILMETHOD_SECT_EH *)pSectEH)->Small.Clauses[idx].HandlerLength;
    buff->HandlerOffset = ((COR_ILMETHOD_SECT_EH *)pSectEH)->Small.Clauses[idx].HandlerOffset;
    return(buff);
}
/*********************************************************************/
        // compute the size of the section (best format)
        // codeSize is the size of the method
    // deprecated
unsigned __stdcall SectEH_SizeWithCode(unsigned ehCount, unsigned codeSize)
{
    return((ehCount)? SectEH_SizeWorst(ehCount) : 0);
}

    // will return worse-case size and then Emit will return actual size
unsigned __stdcall SectEH_SizeWorst(unsigned ehCount)
{
    return((ehCount)? (COR_ILMETHOD_SECT_EH_FAT::Size(ehCount)) : 0);
}

// will return exact size which will match the size returned by Emit
unsigned __stdcall SectEH_SizeExact(unsigned ehCount, IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* clauses)
{
    if (ehCount == 0)
        return(0);

    unsigned smallSize = COR_ILMETHOD_SECT_EH_SMALL::Size(ehCount);
    if (smallSize > COR_ILMETHOD_SECT_SMALL_MAX_DATASIZE)
            return(COR_ILMETHOD_SECT_EH_FAT::Size(ehCount));
    for (unsigned i = 0; i < ehCount; i++) {
        if (clauses[i].TryOffset > 0xFFFF ||
                clauses[i].TryLength > 0xFF ||
                clauses[i].HandlerOffset > 0xFFFF ||
                clauses[i].HandlerLength > 0xFF) {
            return(COR_ILMETHOD_SECT_EH_FAT::Size(ehCount));
        }
    }
    return smallSize;
}

/*********************************************************************/

// emit the section (best format);
unsigned __stdcall SectEH_Emit(unsigned size, unsigned ehCount,
                  __in_ecount(ehCount) IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* clauses,
                  BOOL moreSections, __inout_ecount_opt(size) BYTE* outBuff,
                  __out_ecount(ehCount) ULONG* ehTypeOffsets)
{
    HRESULT hr = S_OK;
    if (size == 0 || outBuff == nullptr)
        return 0;

    ASSERT ((((size_t) outBuff) & 3) == 0);               // header is dword aligned
    const BYTE* origBuff = outBuff;
    if (ehCount == 0 || clauses == nullptr)
        return 0;

    // Initialize the ehTypeOffsets array.
    if (ehTypeOffsets)
    {
        for (unsigned int i = 0; i < ehCount; i++)
            ehTypeOffsets[i] = (ULONG) -1;
    }

    if (COR_ILMETHOD_SECT_EH_SMALL::Size(ehCount) < COR_ILMETHOD_SECT_SMALL_MAX_DATASIZE) {
        COR_ILMETHOD_SECT_EH_SMALL* EHSect = (COR_ILMETHOD_SECT_EH_SMALL*) outBuff;
		unsigned i;
        for (i = 0; i < ehCount; i++) {
            if (clauses[i].TryOffset > 0xFFFF ||
                    clauses[i].TryLength > 0xFF ||
                    clauses[i].HandlerOffset > 0xFFFF ||
                    clauses[i].HandlerLength > 0xFF) {
                break;  // fall through and generate as FAT
            }
            ASSERT ((clauses[i].Flags & ~0xFFFF) == 0);
            ASSERT ((clauses[i].TryOffset & ~0xFFFF) == 0);
            ASSERT ((clauses[i].TryLength & ~0xFF) == 0);
            ASSERT ((clauses[i].HandlerOffset & ~0xFFFF) == 0);
            ASSERT ((clauses[i].HandlerLength & ~0xFF) == 0);
            EHSect->Clauses[i].Flags         = (CorExceptionFlag) clauses[i].Flags;
            EHSect->Clauses[i].TryOffset     = (WORD) clauses[i].TryOffset;
            EHSect->Clauses[i].TryLength     = (WORD) clauses[i].TryLength;
            EHSect->Clauses[i].HandlerOffset = (WORD) clauses[i].HandlerOffset;
            EHSect->Clauses[i].HandlerLength = (WORD) clauses[i].HandlerLength;
            EHSect->Clauses[i].ClassToken  = clauses[i].ClassToken;
        }
        if (i >= ehCount) {
            // if actually got through all the clauses and they are small enough
            EHSect->Kind = CorILMethod_Sect_EHTable;
            if (moreSections)
                EHSect->Kind |= CorILMethod_Sect_MoreSects;
            EHSect->DataSize = EHSect->Size(ehCount);
            EHSect->Reserved = 0;
            ASSERT (EHSect->DataSize == EHSect->Size(ehCount)); // make sure didn't overflow
            outBuff = (BYTE*) &EHSect->Clauses[ehCount];
            // Set the offsets for the exception type tokens.
            if (ehTypeOffsets)
            {
                for (unsigned int j = 0; j < ehCount; j++) {
                    if (EHSect->Clauses[j].Flags == COR_ILEXCEPTION_CLAUSE_NONE)
                    {
                        ehTypeOffsets[j] = (ULONG)((BYTE *)&EHSect->Clauses[j].ClassToken - origBuff);
                    }
                }
            }
            return(size);
        }
    }
    // either total size too big or one of constituent elements too big (eg. offset or length)
    COR_ILMETHOD_SECT_EH_FAT* EHSect = (COR_ILMETHOD_SECT_EH_FAT*) outBuff;
    EHSect->Kind = CorILMethod_Sect_EHTable | CorILMethod_Sect_FatFormat;
    if (moreSections)
        EHSect->Kind |= CorILMethod_Sect_MoreSects;
    EHSect->DataSize = EHSect->Size(ehCount);
    IfFailRetErrno(memcpy_s(EHSect->Clauses, ehCount * sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT), clauses, ehCount * sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT)));
    outBuff = (BYTE*) &EHSect->Clauses[ehCount];
    ASSERT (&origBuff[size] == outBuff);
    // Set the offsets for the exception type tokens.
    if (ehTypeOffsets)
    {
        for (unsigned int i = 0; i < ehCount; i++) {
            if (EHSect->Clauses[i].Flags == COR_ILEXCEPTION_CLAUSE_NONE)
            {
                ehTypeOffsets[i] = (ULONG)((BYTE *)&EHSect->Clauses[i].ClassToken - origBuff);
            }
        }
    }
    return(size);
}
