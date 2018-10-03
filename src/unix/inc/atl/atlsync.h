// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.


#ifndef __ATLSYNC_H__
#define __ATLSYNC_H__

#pragma once

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(push)
#pragma warning(disable: 4512)  // assignment operator could not be generated
#endif  // !_ATL_NO_PRAGMA_WARNINGS

#include <atlbase.h>

#pragma pack(push,_ATL_PACKING)

namespace ATL
{

class CCriticalSection :
	public CRITICAL_SECTION
{
public:
	CCriticalSection();
	explicit CCriticalSection(_In_ ULONG nSpinCount);

	~CCriticalSection() throw();

	// Acquire the critical section
	_Acquires_lock_(*this)
	void Enter();
	// Release the critical section
	_Releases_lock_(*this)
	void Leave() throw();
};

class CEvent :
	public CHandle
{
public:
	CEvent() throw();
	CEvent(_Inout_ CEvent& h) throw();
	CEvent(
		_In_ BOOL bManualReset,
		_In_ BOOL bInitialState);
	CEvent(
		_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
		_In_ BOOL bManualReset,
		_In_ BOOL bInitialState,
		_In_opt_z_ LPCTSTR pszName);
	explicit CEvent(_In_ HANDLE h) throw();

	// Create a new event
	BOOL Create(
		_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
		_In_ BOOL bManualReset,
		_In_ BOOL bInitialState,
		_In_opt_z_ LPCTSTR pszName) throw();
	// Open an existing named event
	BOOL Open(
		_In_ DWORD dwAccess,
		_In_ BOOL bInheritHandle,
		_In_z_ LPCTSTR pszName) throw();
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	// Pulse the event (signals waiting objects, then resets)
	BOOL Pulse() throw();
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	// Set the event to the non-signaled state
	BOOL Reset() throw();
	// Set the event to the signaled state
	BOOL Set() throw();
};

inline CCriticalSection::CCriticalSection()
{
	if (!::ATL::_AtlInitializeCriticalSectionEx( this, 0, 0 ))
	{
		AtlThrow(HRESULT_FROM_WIN32(GetLastError()));
	}
}

inline CCriticalSection::CCriticalSection(_In_ ULONG nSpinCount)
{
	if (!::ATL::_AtlInitializeCriticalSectionEx( this, nSpinCount, 0 ))
	{
		AtlThrow(HRESULT_FROM_WIN32(GetLastError()));
	}
}

inline CCriticalSection::~CCriticalSection() throw()
{
	::DeleteCriticalSection( this );
}

_Acquires_lock_(*this)
inline void CCriticalSection::Enter()
{
	::EnterCriticalSection( this );
}

_Releases_lock_(*this)
inline void CCriticalSection::Leave() throw()
{
	::LeaveCriticalSection( this );
}

};

inline CEvent::CEvent() throw()
{
}

inline CEvent::CEvent(_Inout_ CEvent& hEvent) throw() :
	CHandle( hEvent )
{
}

inline CEvent::CEvent(
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState)
{
	BOOL bSuccess;

	bSuccess = Create( NULL, bManualReset, bInitialState, NULL );
	if( !bSuccess )
	{
		AtlThrowLastWin32();
	}
}

inline CEvent::CEvent(
	_In_opt_ LPSECURITY_ATTRIBUTES pAttributes,
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState,
	_In_opt_z_ LPCTSTR pszName)
{
	BOOL bSuccess;

	bSuccess = Create( pAttributes, bManualReset, bInitialState, pszName );
	if( !bSuccess )
	{
		AtlThrowLastWin32();
	}
}


inline CEvent::CEvent(_In_ HANDLE h) throw() :
	CHandle( h )
{
}

inline BOOL CEvent::Create(
	_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState,
	_In_opt_z_ LPCTSTR pszName) throw()
{
	ATLASSUME( m_h == NULL );

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	m_h = ::CreateEvent( pSecurity, bManualReset, bInitialState, pszName );
#else
	DWORD dwFlags = 0;

	if (bManualReset)
		dwFlags |= CREATE_EVENT_MANUAL_RESET;

	if (bInitialState)
		dwFlags |= CREATE_EVENT_INITIAL_SET;

	m_h = ::CreateEventEx( pSecurity, pszName, dwFlags, EVENT_ALL_ACCESS );
#endif

	return( m_h != NULL );
}

inline BOOL CEvent::Open(
	_In_ DWORD dwAccess,
	_In_ BOOL bInheritHandle,
	_In_z_ LPCTSTR pszName) throw()
{
	ATLASSUME( m_h == NULL );

	m_h = ::OpenEvent( dwAccess, bInheritHandle, pszName );
	return( m_h != NULL );
}

// #ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
// inline BOOL CEvent::Pulse() throw()
// {
// 	ATLASSUME( m_h != NULL );

// 	return( ::PulseEvent( m_h ) );
// }
// #endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

inline BOOL CEvent::Reset() throw()
{
	ATLASSUME( m_h != NULL );

	return( ::ResetEvent( m_h ) );
}

inline BOOL CEvent::Set() throw()
{
	ATLASSUME( m_h != NULL );

	return( ::SetEvent( m_h ) );
}  // namespace ATL

#pragma pack(pop)
#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(pop)
#endif  // !_ATL_NO_PRAGMA_WARNINGS

#endif  // __ATLSYNC_H__
