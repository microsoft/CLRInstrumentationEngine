// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#define VT_VMAX          VT_DECIMAL+1
// The largest unused value in VARENUM enumeration
#define VT_MAX           VT_CLSID
// This is a special value that is used internally for marshaling interfaces
#define VT_INTERFACE     VT_MAX
#define	FADF_UNKNOWN	 ( 0x200 )
#define PHDATA(psa)      (psa)->pvData
#define PVDATA(psa)      (psa)->pvData
#define FADF_AUTO        ( 0x1 )
#define FADF_STATIC      ( 0x2 )
#define FADF_EMBEDDED    ( 0x4 )
#define FADF_FORCEFREE   0x1000  /* SafeArrayFree() ignores FADF_STATIC and frees anyway */

#define IfFailRet(EXPR) \
    do { HRESULT hr; if(FAILED(hr = (EXPR))) { return hr; }} while(false)

#define PVTELEM(psa) ((LONG *)(psa) - 1)
#define SAFEARRAYOVERFLOW 0xffffffff

PALAPI ULONG SafeArraySize3(unsigned short cDims, ULONG cbElements, SAFEARRAYBOUND * psabound)
{
    ULONG cb;
    ULONG dw1;
    ULONG dw2;
    unsigned short us;

    cb = 0L;
    if (cDims)
    {
        cb = (ULONG)cbElements;
        for (us = 0; us < cDims; ++us)
        {
            dw1 = cb;
            dw2 = psabound->cElements;

            // now do a 32x32 multiply, with overflow checking
            // cb = dw1 * dw2;

            // note: DWORD casts on all 16x16 multiplies so that we don't
            // lose the high word of the result.

            if (HIWORD(dw1) == 0 && HIWORD(dw2) == 0)
            {
                // simple case of 16 x 16 -- can't overflow
                cb = (DWORD)LOWORD(dw1) * LOWORD(dw2);
            }
            else if (HIWORD(dw1) != 0 && HIWORD(dw2) != 0)
            {
                // 32 x 32 -- no way
                return SAFEARRAYOVERFLOW;
            }
            else
            {
                // 16 x 32 or 32 x 16
                if (HIWORD(dw2) != 0)
                {      // swap so dw2 is always 16 bit
                    cb = dw1;
                    dw1 = dw2;
                    dw2 = cb;
                }
                // 32(dw1) x 16(dw2)
                ASSERT(HIWORD(dw2) == 0);

                // do first 16 x 16 multiply
                cb = ((DWORD)HIWORD(dw1)) * LOWORD(dw2);
                if (HIWORD(cb) != 0)
                    return SAFEARRAYOVERFLOW;
                // do second multiply
                dw1 = ((DWORD)LOWORD(dw1)) * LOWORD(dw2);    // dw1 = 2nd partial product
                dw2 = MAKELONG(0, LOWORD(cb));   // dw2 = 1st partial product
                cb = dw1 + dw2;            // cb = sum of partial products
                if (cb < dw1)            // check for overflow
                    return SAFEARRAYOVERFLOW;
            }

            ++psabound;
        }
    }

    return cb;
}

PALAPI HRESULT
SafeArrayElementSizeFeatures(VARTYPE vt,
         unsigned short* pcbSize,
         unsigned short* pfFeatures,
         IRecordInfo* prinfo)
{
    *pcbSize = 0;
    *pfFeatures = 0;

    // Note: the types that are legal for arrays are slightly different
    // than the types that can be held by a VARIANT or VARIANTARG.
    //

    switch (vt)
    {
    case VT_I1:
    case VT_UI1:
        *pcbSize = sizeof(char);
        break;

    case VT_I2:
    case VT_BOOL:
    case VT_UI2:
        *pcbSize = sizeof(short);
        break;

    case VT_I4:
    case VT_ERROR:
    case VT_R4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
        *pcbSize = sizeof(LONG);
        break;

    case VT_I8:
    case VT_UI8:
        *pcbSize = sizeof(LONGLONG);
        break;

    case VT_INT_PTR:
    case VT_UINT_PTR:
#if _M_AMD64
        *pcbSize = sizeof(LONGLONG);
#else
        *pcbSize = sizeof(LONG);
#endif
        break;

    case VT_R8:
    case VT_CY:
    case VT_DATE:
        *pcbSize = sizeof(double);
        break;

    case VT_DECIMAL:
        *pcbSize = sizeof(DECIMAL);
        break;

    case VT_BSTR:
        *pfFeatures = FADF_BSTR;
        *pcbSize = sizeof(BSTR);
        break;

    case VT_VARIANT:
        *pfFeatures = FADF_VARIANT;
        *pcbSize = sizeof(VARIANT);
        break;

    case VT_UNKNOWN:
    case VT_DISPATCH:
    case VT_RECORD:
        return E_INVALIDARG;

    default:
        return E_INVALIDARG;
    }
    return NOERROR;
}

PALAPI SAFEARRAY *
SafeArrayCreateVectorEx(VARTYPE vt, LONG lLbound, ULONG cElements, LPVOID pvExtra)
{
    SAFEARRAYBOUND sabound = { cElements, lLbound };

    int cbExtra;
    SAFEARRAY* psa;
    ULONG  cbSize;
    unsigned short cbElement, fFeatures;
    IRecordInfo *prinfo = NULL;
    GUID *pguid = NULL;

    if (vt == VT_DISPATCH || vt == VT_UNKNOWN || vt == VT_RECORD)
        ERROR("Invalid variant type");

    if (FAILED(SafeArrayElementSizeFeatures(vt, &cbElement, &fFeatures, prinfo)))
        return NULL;

    cbSize = SafeArraySize3(1, cbElement, &sabound);
    if (cbSize == SAFEARRAYOVERFLOW ||
        (LONG_MAX - sizeof(SAFEARRAY)) < cbSize)
        return NULL;
    cbSize += sizeof(SAFEARRAY); // add the descriptor

    cbExtra = sizeof(GUID); // Space for the IID

    if (NULL == (psa = (SAFEARRAY*)malloc(cbSize + cbExtra)))
        return NULL;

    // zero out the allocated memory
    memset(psa, 0, cbSize + cbExtra);
    psa = (SAFEARRAY*)((BYTE *)psa + cbExtra);

    psa->pvData = (LPVOID)((BYTE *)psa + sizeof(SAFEARRAY));
    psa->cDims = 1;
    psa->cbElements = cbElement;
    psa->fFeatures = fFeatures | FADF_SINGLEALLOC;
    psa->rgsabound[0] = sabound;

    psa->fFeatures |= FADF_HAVEVARTYPE;
    *PVTELEM(psa) = vt;

    return psa;
}

PALAPI SAFEARRAY *
SafeArrayCreateVector(VARTYPE vt, LONG lLbound, ULONG cElements)
{
    SAFEARRAYBOUND sabound = { cElements, lLbound };

    void* pExtra = NULL;

    if ((vt == VT_DISPATCH) || (vt == VT_UNKNOWN))
        ERROR("Invalid variant type");

    return SafeArrayCreateVectorEx(vt, lLbound, cElements, pExtra);
}

static HRESULT IsLegalVartype(VARTYPE vt)
{
    ASSERT(vt >= VT_VARIANT);	// caller should have checked for all  				// vt's < VT_VARIANT for speed.
    // NOTE: optimized for speed, rather than for maintainablity
    if (vt & (VT_BYREF | VT_ARRAY))
    {
        vt &= ~(VT_BYREF | VT_ARRAY);
    }

    if ((vt >= VT_I2 && vt <= VT_DECIMAL) ||
        (vt >= VT_I1 && vt <= VT_UI8) ||
        vt == VT_INT ||
        vt == VT_UINT ||
        vt == VT_RECORD)
    {
        return NOERROR;
    }

    return DISP_E_BADVARTYPE;
}


PALAPI HRESULT
VariantClear(VARIANTARG FAR* pvarg)
{
    VARTYPE vt;
    HRESULT hr;

    vt = V_VT(pvarg);

    if (vt >= VT_BSTR)
    {
        if (vt >= VT_VARIANT)
        {
            hr = IsLegalVartype(vt);
            if (hr != S_OK)
            {
                return hr;
            }
        }

        switch (vt)
        {
        case VT_BSTR:
            SysFreeString(V_BSTR(pvarg));
            break;

        case VT_UNKNOWN:
        case VT_INTERFACE:
            if (V_UNKNOWN(pvarg) != NULL)
                V_UNKNOWN(pvarg)->Release();
            break;

        case VT_DISPATCH:
            if (V_DISPATCH(pvarg) != NULL)
                V_DISPATCH(pvarg)->Release();
            break;

        case VT_RECORD:
            return E_NOTIMPL;
            /*
               if(V_RECORDINFO(pvarg) != NULL)
               {
               if (V_RECORD(pvarg) != NULL)
               V_RECORDINFO(pvarg)->RecordClear(V_RECORD(pvarg));
               V_RECORDINFO(pvarg)->Release();
               delete V_RECORD(pvarg);
               }
               break;
               */

        default:
            if (vt & VT_ARRAY) // V_ISARRAY
            {
                if (!V_ISBYREF(pvarg))
                {
                    hr = SafeArrayDestroy(V_ARRAY(pvarg));
                    if (hr != S_OK)
                    {
                        return hr;
                    }
                }
            }

            break;
        }
    }

#ifdef DEBUG
    memset(pvarg, -1, sizeof(*pvarg));
#endif

    V_VT(pvarg) = VT_EMPTY;
    return NOERROR;
}

PALAPI HRESULT
VariantCopy(_Inout_ VARIANTARG * pvargDest, _In_ const VARIANTARG * pvargSrc)
{
    VARTYPE vt;
    HRESULT hr;

    vt = V_VT(pvargSrc);
    if (vt >= VT_VARIANT)
    {
        hr = IsLegalVartype(vt);
        if (hr != S_OK)
        {
            return hr;
        }
    }

    if (pvargDest == pvargSrc)
        return NOERROR;

    IfFailRet(VariantClear(pvargDest));

    if ((vt & VT_ARRAY) == VT_ARRAY)
    {
        return E_NOTIMPL;
    }
    else if (vt == VT_BSTR)
    {
        BSTR bstrSrc = V_BSTR(pvargSrc);
        BSTR bstrDest = SysAllocStringLen(bstrSrc, SysStringLen(bstrSrc));
        if (bstrDest == nullptr)
            return E_OUTOFMEMORY;
        V_BSTR(pvargDest) = bstrDest;
        V_VT(pvargDest) = VT_BSTR;
    }
    else if ((vt & ~VT_BYREF) == VT_RECORD)
    {
        return E_NOTIMPL;
    }
    else
    {
        memcpy(pvargDest, pvargSrc, sizeof(VARIANTARG));

        switch (vt)
        {
        case VT_UNKNOWN:
            if (V_UNKNOWN(pvargDest) != NULL)
                V_UNKNOWN(pvargDest)->AddRef();
            break;

        case VT_DISPATCH:
            if (V_DISPATCH(pvargDest) != NULL)
                V_DISPATCH(pvargDest)->AddRef();
            break;
        }
    }

    return S_OK;
}

ULONG SafeArraySize(SAFEARRAY FAR* psa)
{
    return SafeArraySize3(psa->cDims, psa->cbElements, psa->rgsabound);
}

/***
 *PRIVATE void ReleaseResources(SAFEARRAY *, void*, ULONG, unsigned short, unsigned short/LONG)
 *Purpose:
 * Release any resources held by the given chunk of memory.
 *
 *Entry:
 * pv = the chunk
 * cbSize = the size of the chunk in bytes
 * fFeatures = features bits describing the chunk
 * cbElement = the size of each element of the chunk
 *
 *Exit:
 * return value =
 *
 ***********************************************************************/
// release the resources held by the given chunk of memory
static void ReleaseResources(
    SAFEARRAY * psa,
    void * pv,
    ULONG cbSize,
    unsigned short fFeatures,
    ULONG cbElement)
{
    ULONG i, cElements;

    if (cbElement == 0 || pv == NULL)
        return;

    cElements = cbSize / cbElement;

    if (fFeatures & FADF_BSTR)
    {

        BSTR * pbstr;

        pbstr = (BSTR *)pv;
        for (i = 0; i < cElements; ++i)
            SysFreeString(*pbstr++);

    }
    else if (fFeatures & FADF_UNKNOWN)
    {

        IUnknown FAR* * ppunk;

        ppunk = (IUnknown **)pv;
        for (i = 0; i < cElements; ++i)
        {
            if (*ppunk != NULL)
                (*ppunk)->Release();
            ++ppunk;
        }

    }
    else if (fFeatures & FADF_DISPATCH)
    {

        IDispatch ** ppdisp;

        ppdisp = (IDispatch **)pv;
        for (i = 0; i < cElements; ++i)
        {
            if (*ppdisp != NULL)
                (*ppdisp)->Release();
            ++ppdisp;
        }

    }
    else if (fFeatures & FADF_VARIANT)
    {

        VARIANT * pvar;

        pvar = (VARIANT *)pv;
        for (i = 0; i < cElements; ++i)
            VariantClear(pvar++);

        /*
           }else if(fFeatures & FADF_RECORD)
           {

           BYTE * pb;
           IRecordInfo *pri;

           pb = (BYTE *)pv;
           pri = *PPIRIELEM(psa);

           if (pri == NULL)
           return;

           for(i = 0; i < cElements; ++i)
           {
           pri->RecordClear(pb);
           pb += cbElement;
           }
           */
    }
}

/***
 *PRIVATE HRESULT SafeArrayDestroyData(SAFEARRAY*)
 *Purpose:
 * Release the contents of the given SafeArray.
 *
 *Entry:
 * psa = pointer the the array descriptor of the array to erase
 *
 *Exit:
 * return value = HRESULT
 *
 ***********************************************************************/
static HRESULT
_SafeArrayDestroyData(SAFEARRAY FAR* psa, BOOL bRelease)
{
    ULONG cbSize;

    if (psa == NULL)
        return NOERROR;

    if (psa->cLocks > 0)
        return DISP_E_ARRAYISLOCKED;

    if (PHDATA(psa) == NULL)
        return NOERROR;

    cbSize = SafeArraySize(psa);
    ASSERT(cbSize != SAFEARRAYOVERFLOW); // existing array, no overflow possible

    if (bRelease)
    {
        ReleaseResources(
            psa,
            psa->pvData,
            cbSize,
            psa->fFeatures,
            psa->cbElements);

        // We're not gonna be freeing the data allocation
        // so we must zero out the bits
        if (psa->fFeatures & FADF_STATIC)
        {
            memset(psa->pvData, 0, cbSize);
        }
    }

    if ((psa->fFeatures & (FADF_AUTO | FADF_STATIC | FADF_EMBEDDED)) == 0 ||
        (psa->fFeatures & FADF_FORCEFREE))
    {

        if (psa->fFeatures & FADF_SINGLEALLOC)
        {
            psa->fFeatures &= ~FADF_SINGLEALLOC; // no longer a single-alloc array
            return NOERROR; // can't release now. Mem will go away when
            // descriptor is released.
        }

        free(psa->pvData);
        psa->pvData = NULL;
    }
    return NOERROR;
}

PALAPI HRESULT SafeArrayDestroy(SAFEARRAY FAR* psa)
{
    return _SafeArrayDestroyData(psa, TRUE);
}
