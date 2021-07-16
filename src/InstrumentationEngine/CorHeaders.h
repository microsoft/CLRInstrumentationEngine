// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

class COR_ILMETHOD_DECODER;

// Method/SEH headers/tables
/************************************/

//*****************************************************************************
typedef struct tagCOR_ILMETHOD_SECT_SMALL : IMAGE_COR_ILMETHOD_SECT_SMALL
{
    //Data follows
    const BYTE* Data() const
    {
        return (const BYTE*)this + sizeof(struct tagCOR_ILMETHOD_SECT_SMALL);
    }
} COR_ILMETHOD_SECT_SMALL;


/************************************/
/* NOTE this structure must be DWORD aligned!! */
typedef struct tagCOR_ILMETHOD_SECT_FAT : IMAGE_COR_ILMETHOD_SECT_FAT
{
    //Data follows
    const BYTE* Data() const
    {
        return (const BYTE*)this + sizeof(struct tagCOR_ILMETHOD_SECT_FAT);
    }
} COR_ILMETHOD_SECT_FAT;

//*****************************************************************************
struct COR_ILMETHOD_SECT_EH_SMALL : public COR_ILMETHOD_SECT_SMALL
{
    static unsigned Size(unsigned ehCount)
    {
        return sizeof(COR_ILMETHOD_SECT_EH_SMALL) + sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL) * (ehCount - 1);
    }

    WORD Reserved;										  // alignment padding
    IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL Clauses[1];   // actually variable size
};

//*****************************************************************************
struct COR_ILMETHOD_SECT_EH_FAT : public COR_ILMETHOD_SECT_FAT {
    static unsigned Size(unsigned ehCount)
    {
        return sizeof(COR_ILMETHOD_SECT_EH_FAT) + sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT) * (ehCount - 1);
    }

    IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT Clauses[1];     // actually variable size
};

/* NOTE this structure must be DWORD aligned!! */
struct COR_ILMETHOD_SECT
{
    bool More() const
    {
        return (AsSmall()->Kind & CorILMethod_Sect_MoreSects) != 0;
    }
    CorILMethodSect Kind() const
    {
        return (CorILMethodSect)(AsSmall()->Kind & CorILMethod_Sect_KindMask);
    }
    const COR_ILMETHOD_SECT* Next() const
    {
        if (!More()) return 0;
        return IsFat() ? ((COR_ILMETHOD_SECT*)&AsFat()->Data()[AsFat()->DataSize])->Align() : ((COR_ILMETHOD_SECT*)&AsSmall()->Data()[AsSmall()->DataSize])->Align();
    }
    const COR_ILMETHOD_SECT* NextLoc() const
    {
        return IsFat() ? ((COR_ILMETHOD_SECT*)&AsFat()->Data()[AsFat()->DataSize])->Align() : ((COR_ILMETHOD_SECT*)&AsSmall()->Data()[AsSmall()->DataSize])->Align();
    }
    const BYTE* Data() const
    {
        return IsFat() ? AsFat()->Data() : AsSmall()->Data();
    }
    unsigned DataSize() const
    {
        return IsFat() ? AsFat()->DataSize : AsSmall()->DataSize;
    }

    friend struct COR_ILMETHOD;
    friend struct tagCOR_ILMETHOD_FAT;
    friend struct tagCOR_ILMETHOD_TINY;
    bool IsFat() const
    {
        return (AsSmall()->Kind & CorILMethod_Sect_FatFormat) != 0;
    }
    const COR_ILMETHOD_SECT* Align() const
    {
        return (COR_ILMETHOD_SECT*)((((UINT_PTR)this) + 3) & ~3);
    }

protected:
    const COR_ILMETHOD_SECT_FAT* AsFat() const
    {
        return (COR_ILMETHOD_SECT_FAT*)this;
    }
    const COR_ILMETHOD_SECT_SMALL* AsSmall() const
    {
        return (COR_ILMETHOD_SECT_SMALL*)this;
    }

public:
    // The body is either a COR_ILMETHOD_SECT_SMALL or COR_ILMETHOD_SECT_FAT
    // (as indicated by the CorILMethod_Sect_FatFormat bit
    union
    {
        COR_ILMETHOD_SECT_EH_SMALL Small;
        COR_ILMETHOD_SECT_EH_FAT Fat;
    };
}; // COR_ILMETHOD_SECT


/***********************************/
// exported functions:

IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* __stdcall SectEH_EHClause(void* pSectEH, unsigned idx, IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* buff);

// compute the size of the section (best format)
// deprecated
unsigned __stdcall SectEH_SizeWithCode(unsigned ehCount, unsigned codeSize);

// will return worse-case size and then Emit will return actual size
unsigned __stdcall SectEH_SizeWorst(unsigned ehCount);

// will return exact size which will match the size returned by Emit
unsigned __stdcall SectEH_SizeExact(unsigned ehCount, const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* clauses);

// emit the section (best format);
unsigned __stdcall SectEH_Emit(unsigned size, unsigned ehCount, __in_ecount(ehCount) const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* clauses, bool moreSections, __inout_ecount_opt(size) BYTE* outBuff, __out_ecount(ehCount) ULONG* ehTypeOffsets = 0);


struct COR_ILMETHOD_SECT_EH : public COR_ILMETHOD_SECT
{
    unsigned EHCount() const
    {
        return (unsigned)(IsFat() ? (Fat.DataSize / sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT)) : (Small.DataSize / sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL)));
    }

    // return one clause in its fat form.  Use 'buff' if needed
    const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* EHClause(unsigned idx, IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* buff) const
    {
        return SectEH_EHClause((void*)this, idx, buff);
    };

    // compute the size of the section (best format)    codeSize is the size of the method   // deprecated
    unsigned static Size(unsigned ehCount, unsigned codeSize)
    {
        return SectEH_SizeWithCode(ehCount, codeSize);
    };

    // will return worse-case size and then Emit will return actual size
    unsigned static Size(unsigned ehCount)
    {
        return SectEH_SizeWorst(ehCount);
    };

    // will return exact size which will match the size returned by Emit
    unsigned static Size(unsigned ehCount, const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* clauses)
    {
        return SectEH_SizeExact(ehCount, clauses);
    };

    // emit the section (best format);
    unsigned static Emit(unsigned size, unsigned ehCount, const IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT* clauses, bool moreSections, _Out_cap_(size) BYTE* outBuff, __out_ecount(ehCount)  ULONG* ehTypeOffsets = 0)
    {
        return SectEH_Emit(size, ehCount, clauses, moreSections, outBuff, ehTypeOffsets);
    };
}; // COR_ILMETHOD_SECT_EH


/***************************************************************************/
/* Used when the method is tiny (< 64 bytes), and there are no local vars */
typedef struct tagCOR_ILMETHOD_TINY : IMAGE_COR_ILMETHOD_TINY
{
    bool     IsTiny() const { return (Flags_CodeSize & (CorILMethod_FormatMask >> 1)) == CorILMethod_TinyFormat; }
    unsigned GetCodeSize() const { return ((unsigned)Flags_CodeSize) >> (CorILMethod_FormatShift - 1); }
    unsigned GetMaxStack() { return 8; }
    BYTE* GetCode() const { return ((BYTE*)this) + sizeof(struct tagCOR_ILMETHOD_TINY); }
    DWORD    GetLocalVarSigTok() { return 0; }
    COR_ILMETHOD_SECT* GetSect() { return 0; }
} COR_ILMETHOD_TINY;

// This strucuture is the 'fat' layout, where no compression is attempted.
// Note that this structure can be added on at the end, thus making it extensible
typedef struct tagCOR_ILMETHOD_FAT : IMAGE_COR_ILMETHOD_FAT
{
    bool     IsFat() const { return (Flags & CorILMethod_FormatMask) == CorILMethod_FatFormat; }
    unsigned GetMaxStack() const { return MaxStack; }
    unsigned GetCodeSize() const { return CodeSize; }
    mdToken  GetLocalVarSigTok() const { return LocalVarSigTok; }
    BYTE* GetCode() const { return ((BYTE*)this) + (4 * Size)/*size in DWords of this structure (currently 3)*/; } // lgtm[cpp/incorrect-pointer-scaling-char]

    // SEH sections are the only additional sections in the first release of CLR
    // additional section begins with the section header,
    // which contains two entries: Kind and DataSize.
    // there are two types of headers:
    // IMAGE_COR_ILMETHOD_SECT_SMALL and IMAGE_COR_ILMETHOD_SECT_FAT
    bool HasSEH() const
    {
        return (Flags & CorILMethod_MoreSects) != 0;
    }

    const COR_ILMETHOD_SECT* GetSect() const
    {
        if (!(Flags & CorILMethod_MoreSects))
            return 0;
        return ((COR_ILMETHOD_SECT*)(GetCode() + GetCodeSize()))->Align();
    }

} COR_ILMETHOD_FAT;

typedef COR_ILMETHOD_FAT* PCOR_ILMETHOD_FAT;

/************************************/
// exported functions (impl. Format\Format.cpp)
unsigned __stdcall IlmethodSize(const COR_ILMETHOD_FAT* header, bool MoreSections);

// emit the header (bestFormat) return amount emitted
unsigned __stdcall IlmethodEmit(unsigned size, const COR_ILMETHOD_FAT* header, bool moreSections, _Out_cap_(size) BYTE* outBuff);

struct COR_ILMETHOD
{
    // a COR_ILMETHOD header should not be decoded by hand.  Instead us
    // COR_ILMETHOD_DECODER to decode it.
    friend class COR_ILMETHOD_DECODER;

    // compute the size of the header (best format)
    unsigned static Size(const COR_ILMETHOD_FAT* header, bool MoreSections)
    {
        return IlmethodSize(header, MoreSections);
    };

    // emit the header (bestFormat) return amount emitted
    unsigned static Emit(unsigned size, const COR_ILMETHOD_FAT* header, bool moreSections, _Out_cap_(size) BYTE* outBuff)
    {
        return IlmethodEmit(size, header, moreSections, outBuff);
    };

    //private:
    union
    {
        COR_ILMETHOD_TINY       Tiny;
        COR_ILMETHOD_FAT        Fat;
    };
    // Code follows the Header, then immedately after the code comes
    // any sections (COR_ILMETHOD_SECT).
};

/***************************************************************************/
/* COR_ILMETHOD_DECODER is the only way functions internal to the EE should
   fetch data from a COR_ILMETHOD.  This way any dependancy on the file format
   (and the multiple ways of encoding the header) is centralized to the
   COR_ILMETHOD_DECODER constructor) */
void __stdcall DecoderInit(COR_ILMETHOD_DECODER* pThis, const COR_ILMETHOD_FAT* header);


class COR_ILMETHOD_DECODER : public COR_ILMETHOD_FAT
{
public:
    // Typically the ONLY way you should access COR_ILMETHOD is through
    // this constructor so format changes are easier.
    COR_ILMETHOD_DECODER(const COR_ILMETHOD_FAT* header) { DecoderInit(this, header); };

    // The above variant of the constructor can not do a 'complete' job, because
    // it can not look up the local variable signature meta-data token.
    // This method should be used when you have access to the Meta data API
    // If the consturction fails, the 'Code' field is set to
    COR_ILMETHOD_DECODER(COR_ILMETHOD_FAT* header, void* pInternalImport = nullptr,
        bool verify = false);

    unsigned EHCount() const
    {
        return EH == 0 ? 0 : EH->EHCount();
    }

    // Flags        these are available because we inherit COR_ILMETHOD_FAT
    // MaxStack
    // CodeSize
    const BYTE* Code = nullptr;
    PCCOR_SIGNATURE LocalVarSig = nullptr;        // pointer to signature blob, or 0 if none
    const COR_ILMETHOD_SECT_EH* EH = nullptr;     // eh table if any  0 if none
    const COR_ILMETHOD_SECT* Sect = nullptr;      // additional sections  0 if none
};