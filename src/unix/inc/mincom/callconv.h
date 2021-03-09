
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// callconv.h basic macros declaring calling convensions used
// by IUnknown types

#pragma once

#define NTAPI       __cdecl
#define WINAPI      __cdecl
#define CALLBACK    __cdecl
#define NTSYSAPI

#define STDMETHODCALLTYPE    __cdecl
#define STDMETHODVCALLTYPE   __cdecl

#define STDAPICALLTYPE       __cdecl
#define STDAPIVCALLTYPE      __cdecl

#define STDMETHODIMP         HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)  type STDMETHODCALLTYPE

#define STDMETHODIMPV        HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_(type) type STDMETHODVCALLTYPE

#define STDMETHOD(method)       virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method) virtual type STDMETHODCALLTYPE method

#define STDMETHODV(method)       virtual HRESULT STDMETHODVCALLTYPE method
#define STDMETHODV_(type,method) virtual type STDMETHODVCALLTYPE method

#define STDAPI               EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)        EXTERN_C type STDAPICALLTYPE
#define STDAPI_VIS(vis,type) EXTERN_C vis type STDAPICALLTYPE

#define STDAPIV              EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type)       EXTERN_C type STDAPIVCALLTYPE