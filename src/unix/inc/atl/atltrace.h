// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLTRACE_H__
#define __ATLTRACE_H__

#pragma once

#include <atldef.h>
#include <atlconv.h>
#include <crtdbg.h>
#include <stdio.h>
#include <stdarg.h>

//
// Tracing mechanism doesn't require AtlTraceTool anymore
// Output from the tracing is passed to _CrtDbgReportW
// If you need to override reporting functionality then you should use CRT Debug Routines
//

#pragma pack(push,_ATL_PACKING)
namespace ATL
{
#define atlTraceGeneral 0x000001
#define atlTraceException 0x002000

#ifdef _DEBUG
class CTrace
{
private:
	static errno_t BeginErrorCheck()
	{
		return errno;
	}

	static void __cdecl CTrace::TraceV(
			_In_opt_z_ const char *pszFileName,
			_In_ int nLine,
			_In_ unsigned int dwCategory,
			_In_ unsigned int nLevel,
			_In_z_ LPCWSTR pwszMessage)
	{
	    // TODO:
	    /*
		if (CTrace::m_nLevel == CTrace::DisableTracing || CTrace::m_nLevel < nLevel || (CTrace::m_nCategory & dwCategory) == 0)
		{
			return;
		}

		WCHAR wszCategory[TraceBufferSize] = {'\0'};
		int categoryLength = 0;
		const WCHAR *const pwszCategoryName = GetCategoryName(dwCategory);
		if (pwszCategoryName != nullptr)
		{
			if ((categoryLength = swprintf_s(wszCategory, TraceBufferSize, _T("%s - "), pwszCategoryName)) == -1)
			{
				return;
			}
		}
		else
		{
			if ((categoryLength = swprintf_s(wszCategory, TraceBufferSize, _T("%u - "), dwCategory)) == -1)
			{
				return;
			}
		}

		int chCount = (int)PAL_wcslen(pwszMessage) + categoryLength + 1;
		CHeapPtr<WCHAR> wszBuf;
		if (!wszBuf.Allocate(chCount))
		{
			return;
		}

		wszBuf[0] = '\0';

		if (swprintf_s(wszBuf, chCount, _T("%s%s"), wszCategory, pwszMessage) == -1)
		{
			return;
		}

		WCHAR fileName[_MAX_PATH] = {'\0'};	
		if (swprintf_s(fileName, _MAX_PATH, _T("%S"), pszFileName) == -1)
		{
			return;
		}

		_CrtDbgReportW(_CRT_WARN, fileName, nLine, nullptr, _T("%s"), wszBuf);
		*/
	}

public:
	enum {
		DefaultTraceLevel = 0,
		MaxLengthOfCategoryName = 0x80,
		MaxCategoryArray = sizeof(unsigned int) * 8, // 32 category names possible
		TraceBufferSize = 1024,
		DisableTracing = 0xffffffff,
		EnableAllCategories = DisableTracing
	};

	static void __cdecl CTrace::TraceV(
		_In_opt_z_ const char *pszFileName,
		_In_ int nLine,
		_In_ unsigned int dwCategory,
		_In_ unsigned int nLevel,
		_In_z_ LPCSTR pszFmt, 
		_In_ va_list args)
	{
		// TODO:
		/*
		int cchNeeded = _vscprintf(pszFmt, args);
		if (cchNeeded < 0)
		{
			return;
		}

		CHeapPtr<char> szBuf;
		if (!szBuf.Allocate(cchNeeded + 1))
		{
			return;
		}

		szBuf[0] = '\0';

		if (_vsnprintf_s(szBuf, cchNeeded + 1, cchNeeded, pszFmt, args) == -1)
		{
			return;
		}

		CHeapPtr<WCHAR> wszBuf;
		if (!wszBuf.Allocate(cchNeeded + 1))
		{
			return;
		}

		wszBuf[0] = '\0';

		if (::MultiByteToWideChar(CP_ACP, 0, szBuf, -1, wszBuf, cchNeeded + 1) == 0)
		{
			return;
		}

		TraceV(pszFileName, nLine, dwCategory, nLevel, wszBuf);
		*/
	}

	static void __cdecl CTrace::TraceV(
		_In_opt_z_ const char *pszFileName,
		_In_ int nLine,
		_In_ unsigned int dwCategory,
		_In_ unsigned int nLevel,
		_In_z_ LPCWSTR pwszFmt,
		_In_ va_list args)
	{
        // TODO:
		/*
		int cchNeeded = _vscwprintf(pwszFmt, args);
		if (cchNeeded < 0)
		{
			return;
		}

		CHeapPtr<WCHAR> wszBuf;
		if (!wszBuf.Allocate(cchNeeded + 1))
		{
			return;
		}

		wszBuf[0] = '\0';

		if (_vsnwprintf_s(wszBuf, cchNeeded + 1, cchNeeded, pwszFmt, args) == -1)
		{
			return;
		}

		TraceV(pszFileName, nLine, dwCategory, nLevel, wszBuf);
		*/
	}
};

class CTraceFileAndLineInfo
{
public:
	CTraceFileAndLineInfo(
			_In_z_ const char *pszFileName,
			_In_ int nLineNo)
		: m_pszFileName(pszFileName), m_nLineNo(nLineNo)
	{
	}

#pragma warning(push)
#pragma warning(disable : 4793)
    void __cdecl operator()(
        _In_ int dwCategory,
        _In_ UINT nLevel,
        _In_z_ const char *pszFmt,
        ...) const
    {
        va_list ptr; va_start(ptr, pszFmt);
        ATL::CTrace::TraceV(m_pszFileName, m_nLineNo, dwCategory, nLevel, pszFmt, ptr);
        va_end(ptr);
    }
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4793)
    void __cdecl operator()(
        _In_ int dwCategory,
        _In_ UINT nLevel,
        _In_z_ const WCHAR *pszFmt,
        ...) const
    {
        va_list ptr; va_start(ptr, pszFmt);
        ATL::CTrace::TraceV(m_pszFileName, m_nLineNo, dwCategory, nLevel, pszFmt, ptr);
        va_end(ptr);
    }
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4793)
	void __cdecl operator()(
		_In_z_ const char *pszFmt, 
		...) const
	{
		va_list ptr; va_start(ptr, pszFmt);
		ATL::CTrace::TraceV(m_pszFileName, m_nLineNo, atlTraceGeneral, 0, pszFmt, ptr);
		va_end(ptr);
	}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4793)
	void __cdecl operator()(
		_In_z_ const WCHAR *pszFmt, 
		...) const
	{
		va_list ptr; va_start(ptr, pszFmt);
		ATL::CTrace::TraceV(m_pszFileName, m_nLineNo, atlTraceGeneral, 0, pszFmt, ptr);
		va_end(ptr);
	}
#pragma warning(pop)

private:
	/* unimplemented */
	CTraceFileAndLineInfo &__cdecl operator=(_In_ const CTraceFileAndLineInfo &right);

	const char *const m_pszFileName;
	const int m_nLineNo;
};

#endif  // _DEBUG


#ifdef _DEBUG

#ifndef _ATL_NO_DEBUG_CRT
class CNoUIAssertHook
{
public:
	CNoUIAssertHook()
	{
		ATLASSERT( s_pfnPrevHook == NULL );
		s_pfnPrevHook = _CrtSetReportHook(CrtHookProc);
	}
	~CNoUIAssertHook()
	{
		_CrtSetReportHook(s_pfnPrevHook);
		s_pfnPrevHook = NULL;
	}

private:
	static int __cdecl CrtHookProc(
		_In_ int eReportType,
		_In_z_ char* pszMessage,
		_Inout_ int* pnRetVal)
	{

		if (eReportType == _CRT_ASSERT)
		{
			::OutputDebugStringA( "ASSERTION FAILED\n" );
			::OutputDebugStringA( pszMessage );
			//If caller doesn't want retVal, so be it.
			if (pnRetVal != NULL)
			{
				*pnRetVal = 1;
			}
			return TRUE;
		}

		if (s_pfnPrevHook != NULL)
		{
			return s_pfnPrevHook(eReportType, pszMessage, pnRetVal);
		}
		else
		{
			return FALSE;
		}
	}

private:
	static _CRT_REPORT_HOOK s_pfnPrevHook;
};

__declspec( selectany ) _CRT_REPORT_HOOK CNoUIAssertHook::s_pfnPrevHook = NULL;

#define DECLARE_NOUIASSERT() ATL::CNoUIAssertHook _g_NoUIAssertHook;

#endif  // _ATL_NO_DEBUG_CRT

#ifndef ATLTRACE
#define ATLTRACE ATL::CTraceFileAndLineInfo(__FILE__, __LINE__)
#define ATLTRACE2 ATLTRACE
#endif

#pragma warning(push)
#pragma warning(disable : 4793)
inline void __cdecl AtlTrace(_In_z_ _Printf_format_string_ LPCSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	ATL::CTrace::TraceV(NULL, -1, atlTraceGeneral, 0, pszFormat, ptr);
	va_end(ptr);
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4793)
inline void __cdecl AtlTrace(_In_z_ _Printf_format_string_ LPCWSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	ATL::CTrace::TraceV(NULL, -1, atlTraceGeneral, 0, pszFormat, ptr);
	va_end(ptr);
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4793)
inline void __cdecl AtlTrace2(
	_In_ int dwCategory,
	_In_ UINT nLevel,
	_In_z_ _Printf_format_string_ LPCSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	ATL::CTrace::TraceV(NULL, -1, dwCategory, nLevel, pszFormat, ptr);
	va_end(ptr);
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4793)
inline void __cdecl AtlTrace2(
	_In_ int dwCategory,
	_In_ UINT nLevel,
	_In_z_ _Printf_format_string_ LPCWSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	ATL::CTrace::TraceV(NULL, -1, dwCategory, nLevel, pszFormat, ptr);
	va_end(ptr);
}
#pragma warning(pop)

#else // !DEBUG

#pragma warning(push)
#pragma warning(disable : 4793)
inline void __cdecl AtlTraceNull(...)
{
}
inline void __cdecl AtlTrace(
	_In_z_ _Printf_format_string_ LPCSTR, ...)
{
}
inline void __cdecl AtlTrace2(
	_In_ DWORD_PTR,
	_In_ UINT,
	_In_z_ _Printf_format_string_ LPCSTR, ...)
{
}
inline void __cdecl AtlTrace(
	_In_z_ _Printf_format_string_ LPCWSTR, ...)
{
}
inline void __cdecl AtlTrace2(
	_In_ DWORD_PTR,
	_In_ UINT,
	_In_z_ _Printf_format_string_ LPCWSTR, ...)
{
}
#pragma warning(pop)

#ifndef ATLTRACE

#define ATLTRACE            __noop
#define ATLTRACE2           __noop
#endif //ATLTRACE
#define DECLARE_NOUIASSERT()

#endif //!_DEBUG

};  // namespace ATL
#pragma pack(pop)

#endif  // __ATLTRACE_H__
