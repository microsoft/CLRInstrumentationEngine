// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLBASE_H__
#define __ATLBASE_H__

#pragma once

#ifdef _ATL_ALL_USER_WARNINGS
#pragma warning( push )  // disable 4505/4710/4514/4511/4512/4355
#endif

// Warnings outside of the push/pop sequence will be disabled for all user
// projects.  The only warnings that should be disabled outside the push/pop
// are warnings that are a) benign and b) will show up in user projects
// without being directly caused by the user

#pragma warning(disable: 4505) // unreferenced local function has been removed
#pragma warning(disable: 4710) // function couldn't be inlined
#pragma warning(disable: 4514) // unreferenced inlines are common

// These two warnings will occur in any class that contains or derives from a
// class with a private copy constructor or copy assignment operator.
#pragma warning(disable: 4511) // copy constructor could not be generated
#pragma warning(disable: 4512) // assignment operator could not be generated

// This is a very common pattern for us
#pragma warning(disable: 4355) // 'this' : used in base member initializer list

#ifdef _ATL_ALL_WARNINGS
#pragma warning( push )  // disable 4668/4820/4917/4127/4097/4786/4291/4201/4103/4268
#endif

#pragma warning(disable : 4668)	// is not defined as a preprocessor macro, replacing with '0' for '#if/#elif
#pragma warning(disable : 4820)	// padding added after member
#pragma warning(disable : 4917)	// a GUID can only be associated with a class, interface or namespace
#pragma warning(disable: 4127) // constant expression
#pragma warning(disable: 4097) // typedef name used as synonym for class-name
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#pragma warning(disable: 4291) // allow placement new
#pragma warning(disable: 4201) // nameless unions are part of C++
#pragma warning(disable: 4103) // pragma pack
#pragma warning(disable: 4268) // const static/global data initialized to zeros

#pragma warning (push)  // disable 4702/4571
// Warning 4702 is generated based on compiler backend data flow analysis. This means that for
// some specific instantiations of a template it can be generated even when the code branch is
// required for other instantiations. In future we should find a way to be more selective about this
#pragma warning(disable: 4702) // Unreachable code
#pragma warning(disable: 4571) //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions
#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif
#ifndef ATL_NO_LEAN_AND_MEAN
#define ATL_NO_LEAN_AND_MEAN
#endif

#include <atldef.h>
#include <objidl.h>

#ifndef _WINSOCKAPI_
#include <winsock2.h>
#endif

#include <atlcore.h>
#include <ole2.h>
#include <atlcomcli.h>

// #include <comcat.h>
#include <stddef.h>

#include <tchar.h>
#include <limits.h>

#include <olectl.h>

#include <errno.h>
#include <process.h>    // for _beginthreadex, _endthreadex

#include <stdio.h>
#include <stdarg.h>

#include <atlconv.h>
#include <shlwapi.h>
#include <atlsimpcoll.h>
#include <atltrace.h>
#include <atlexcept.h>

#pragma pack(push, _ATL_PACKING)

namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// Threading Model Support

template< class TLock >
class CComCritSecLock
{
public:
	_Post_same_lock_(cs, this->m_cs)
	_When_(bInitialLock != 0, _Acquires_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked != 0))
	_When_(bInitialLock == 0, _Post_satisfies_(this->m_bLocked == 0))
	CComCritSecLock(
		_Inout_ TLock& cs,
		_In_ bool bInitialLock = true );

	_When_(this->m_bLocked != 0, _Requires_lock_held_(this->m_cs) _Releases_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked == 0))
	~CComCritSecLock() throw();

	_Acquires_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked != 0)
	_On_failure_(_Post_satisfies_(this->m_bLocked == 0))
	HRESULT Lock() throw();

	_Releases_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked == 0)
	void Unlock() throw();

// Implementation
private:
	TLock& m_cs;
	bool m_bLocked;

// Private to avoid accidental use
	CComCritSecLock(_In_ const CComCritSecLock&) throw();
	CComCritSecLock& operator=(_In_ const CComCritSecLock&) throw();
};

template< class TLock >
_Post_same_lock_(cs, this->m_cs)
_When_(bInitialLock != 0, _Acquires_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked != 0))
_When_(bInitialLock == 0, _Post_satisfies_(this->m_bLocked == 0))
#pragma warning(suppress: 26166) // Constructor throws on failure of Lock() method
inline CComCritSecLock< TLock >::CComCritSecLock(
		_Inout_ TLock& cs,
		_In_ bool bInitialLock) :
	m_cs( cs ),
	m_bLocked( false )
{
	if( bInitialLock )
	{
		HRESULT hr;

		hr = Lock();
		if( FAILED( hr ) )
		{
			AtlThrow( hr );
		}
	}
}

template< class TLock >
_When_(this->m_bLocked != 0, _Requires_lock_held_(this->m_cs) _Releases_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked == 0))
inline CComCritSecLock< TLock >::~CComCritSecLock() throw()
{
	if( m_bLocked )
	{
		Unlock();
	}
}

template< class TLock >
_Acquires_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked != 0)
_On_failure_(_Post_satisfies_(this->m_bLocked == 0))
#pragma warning(suppress: 26165) // Lock is acquired by template lock object '(this->m_cs).m_sec'
inline HRESULT CComCritSecLock< TLock >::Lock() throw()
{
	HRESULT hr;

	ATLASSERT( !m_bLocked );
	ATLASSUME(!m_bLocked);
	hr = m_cs.Lock();
	if( FAILED( hr ) )
	{
		return( hr );
	}
	m_bLocked = true;

	return( S_OK );
}

template< class TLock >
_Releases_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked == 0)
#pragma warning(suppress: 26167) // Lock is released by template lock object '(this->m_cs).m_sec'
inline void CComCritSecLock< TLock >::Unlock() throw()
{
	ATLASSUME( m_bLocked );
#pragma warning(suppress: 26110) // Template parameter hides lock object
	m_cs.Unlock();
	m_bLocked = false;
}

template< typename T >
class CAutoVectorPtr
{
public:
	CAutoVectorPtr() throw() :
		m_p( NULL )
	{
	}
	CAutoVectorPtr(_Inout_ CAutoVectorPtr< T >& p) throw()
	{
		m_p = p.Detach();  // Transfer ownership
	}
	explicit CAutoVectorPtr(_Pre_maybenull_ T* p) throw() :
		m_p( p )
	{
	}
	~CAutoVectorPtr() throw()
	{
		Free();
	}

	operator T*() const throw()
	{
		return( m_p );
	}

	CAutoVectorPtr< T >& operator=(_Inout_ CAutoVectorPtr< T >& p) throw()
	{
		if(*this==p)
		{
			if(m_p == NULL)
			{
				// This branch means both two pointers are NULL, do nothing.
			}
			else if(this!=&p)
			{
				// If this assert fires, it means you attempted to assign one CAutoVectorPtr to another when they both contained
				// a pointer to the same underlying vector. This means a bug in your code, since your vector will get
				// double-deleted.
				ATLASSERT(FALSE);

				// For safety, we are going to detach the other CAutoVectorPtr to avoid a double-free. Your code still
				// has a bug, though.
				p.Detach();
			}
			else
			{
				// Alternatively, this branch means that you are assigning a CAutoVectorPtr to itself, which is
				// pointless but permissible

				// nothing to do
			}
		}
		else
		{
			Free();
			Attach( p.Detach() );  // Transfer ownership
		}
		return( *this );
	}

	// basic comparison operators
	bool operator!=(_In_ CAutoVectorPtr<T>& p) const
	{
		return !operator==(p);
	}

	bool operator==(_In_ CAutoVectorPtr<T>& p) const
	{
		return m_p==p.m_p;
	}

	// Allocate the vector
	bool Allocate(_In_ size_t nElements) throw()
	{
		ATLASSUME( m_p == NULL );
		ATLTRY( m_p = _ATL_NEW T[nElements] );
		if( m_p == NULL )
		{
			return( false );
		}

		return( true );
	}
	// Attach to an existing pointer (takes ownership)
	void Attach(_In_opt_ T* p) throw()
	{
		ATLASSUME( m_p == NULL );
		m_p = p;
	}
	// Detach the pointer (releases ownership)
	T* Detach() throw()
	{
		T* p;

		p = m_p;
		m_p = NULL;

		return( p );
	}
	// Delete the vector pointed to, and set the pointer to NULL
	void Free() throw()
	{
		delete[] m_p;
		m_p = NULL;
	}

public:
	T* m_p;
};

template< typename T >
class CAutoPtr
{
public:
	CAutoPtr() throw() :
		m_p( NULL )
	{
	}
	template< typename TSrc >
	CAutoPtr(_Inout_ CAutoPtr< TSrc >& p) throw()
	{
		m_p = p.Detach();  // Transfer ownership
	}
	CAutoPtr(_Inout_ CAutoPtr< T >& p) throw()
	{
		m_p = p.Detach();  // Transfer ownership
	}
	explicit CAutoPtr(_In_opt_ T* p) throw() :
		m_p( p )
	{
	}
	~CAutoPtr() throw()
	{
		Free();
	}

	// Templated version to allow pBase = pDerived
	template< typename TSrc >
	CAutoPtr< T >& operator=(_Inout_ CAutoPtr< TSrc >& p) throw()
	{
		if(m_p==p.m_p)
		{
			// This means that two CAutoPtrs of two different types had the same m_p in them
			// which is never correct
			ATLASSERT(FALSE);
		}
		else
		{
			Free();
			Attach( p.Detach() );  // Transfer ownership
		}
		return( *this );
	}
	CAutoPtr< T >& operator=(_Inout_ CAutoPtr< T >& p) throw()
	{
		if(*this==p)
		{
			if(this!=&p)
			{
				// If this assert fires, it means you attempted to assign one CAutoPtr to another when they both contained
				// a pointer to the same underlying object. This means a bug in your code, since your object will get
				// double-deleted.
#ifdef ATL_AUTOPTR_ASSIGNMENT_ASSERT
				ATLASSERT(FALSE);
#endif

				// For safety, we are going to detach the other CAutoPtr to avoid a double-free. Your code still
				// has a bug, though.
				p.Detach();
			}
			else
			{
				// Alternatively, this branch means that you are assigning a CAutoPtr to itself, which is
				// pointless but permissible

				// nothing to do
			}
		}
		else
		{
			Free();
			Attach( p.Detach() );  // Transfer ownership
		}
		return( *this );
	}

	// basic comparison operators
	bool operator!=(_In_ CAutoPtr<T>& p) const
	{
		return !operator==(p);
	}

	bool operator==(_In_ CAutoPtr<T>& p) const
	{
		return m_p==p.m_p;
	}

	operator T*() const throw()
	{
		return( m_p );
	}
	T* operator->() const throw()
	{
		ATLASSUME( m_p != NULL );
		return( m_p );
	}

	// Attach to an existing pointer (takes ownership)
	void Attach(_In_opt_ T* p) throw()
	{
		ATLASSUME( m_p == NULL );
		m_p = p;
	}
	// Detach the pointer (releases ownership)
	T* Detach() throw()
	{
		T* p;

		p = m_p;
		m_p = NULL;

		return( p );
	}
	// Delete the object pointed to, and set the pointer to NULL
	void Free() throw()
	{
		delete m_p;
		m_p = NULL;
	}

public:
	T* m_p;
};

/* Automatic cleanup for _malloca objects */
template< typename T >
class CAutoStackPtr
{
public:
	CAutoStackPtr() throw() :
		m_p( NULL )
	{
	}
	template< typename TSrc >
	CAutoStackPtr(_Inout_ CAutoStackPtr< TSrc >& p) throw()
	{
		m_p = p.Detach();  // Transfer ownership
	}
	CAutoStackPtr(_Inout_ CAutoStackPtr< T >& p) throw()
	{
		m_p = p.Detach();  // Transfer ownership
	}
	explicit CAutoStackPtr(_In_opt_ T* p) throw() :
		m_p( p )
	{
	}
	~CAutoStackPtr() throw()
	{
		Free();
	}

	// Templated version to allow pBase = pDerived
	template< typename TSrc >
	CAutoStackPtr< T >& operator=(_Inout_ CAutoStackPtr< TSrc >& p) throw()
	{
		if(m_p==p.m_p)
		{
			// This means that two CAutoPtrs of two different types had the same m_p in them
			// which is never correct
			ATLASSERT(FALSE);
		}
		else
		{
			Free();
			Attach( p.Detach() );  // Transfer ownership
		}
		return( *this );
	}
	CAutoStackPtr< T >& operator=(_Inout_ CAutoStackPtr< T >& p) throw()
	{
		if(*this==p)
		{
			if(this!=&p)
			{
				// If this assert fires, it means you attempted to assign one CAutoPtr to another when they both contained
				// a pointer to the same underlying object. This means a bug in your code, since your object will get
				// double-deleted.
				ATLASSERT(FALSE);

				// For safety, we are going to detach the other CAutoPtr to avoid a double-free. Your code still
				// has a bug, though.
				p.Detach();
			}
			else
			{
				// Alternatively, this branch means that you are assigning a CAutoPtr to itself, which is
				// pointless but permissible

				// nothing to do
			}
		}
		else
		{
			Free();
			Attach( p.Detach() );  // Transfer ownership
		}
		return( *this );
	}

	// basic comparison operators
	bool operator!=(_In_ CAutoStackPtr<T>& p) const
	{
		return !operator==(p);
	}

	bool operator==(_In_ CAutoStackPtr<T>& p) const
	{
		return m_p==p.m_p;
	}

	operator T*() const throw()
	{
		return( m_p );
	}
	T* operator->() const throw()
	{
		ATLASSUME( m_p != NULL );
		return( m_p );
	}

	// Attach to an existing pointer (takes ownership)
	void Attach(_In_opt_ T* p) throw()
	{
		ATLASSUME( m_p == NULL );
		m_p = p;
	}
	// Detach the pointer (releases ownership)
	T* Detach() throw()
	{
		T* p;

		p = m_p;
		m_p = NULL;

		return( p );
	}
	// Delete the object pointed to, and set the pointer to NULL
	void Free() throw()
	{
		/* Note: _freea only actually does anything if m_p was heap allocated
		   If m_p was from the stack, it wouldn't be possible to actually free it here
		   [wrong function] unless we got inlined. But really all we do if m_p is
		   stack-based is ignore it and let its alloca storage disappear at the end
		   of the outer function.
		*/
		_freea(m_p);
		m_p = NULL;
	}

public:
	T* m_p;
};

// static_cast_auto template functions.  Used like static_cast, only they work on CAutoPtr objects
template< class Dest, class Src >
Dest* static_cast_auto(_In_ const CAutoPtr< Src >& pSrc) throw()
{
	return( static_cast< Dest* >( static_cast< Src* >( pSrc ) ) );
}


class CComAllocator
{
public:
	static void* Reallocate(
		_In_opt_ void* p,
		_In_ size_t nBytes) throw()
	{
		if( nBytes > INT_MAX )
		{
			return( NULL );
		}
		return ::CoTaskMemRealloc(p, ULONG(nBytes));
	}
	static void* Allocate(_In_ size_t nBytes) throw()
	{
		if( nBytes > INT_MAX )
		{
			return( NULL );
		}
		return ::CoTaskMemAlloc(ULONG(nBytes));
	}
	static void Free(_In_opt_ void* p) throw()
	{
		::CoTaskMemFree(p);
	}
};

template <typename T>
class CComHeapPtr :
	public CHeapPtr<T, CComAllocator>
{
public:
	CComHeapPtr() throw()
	{
	}

	explicit CComHeapPtr(_In_ T* pData) throw() :
		CHeapPtr<T, CComAllocator>(pData)
	{
	}
};

template <class T, class Reallocator>
_Ret_writes_maybenull_(cEls) T* AtlSafeRealloc(
	_In_opt_ T* pT,
	_In_ size_t cEls) throw()
{
	size_t nBytes=0;
	if(FAILED(::ATL::AtlMultiply(&nBytes, cEls, sizeof(T))))
	{
		Reallocator::Free(pT);
		return NULL;
	}
	
	T *pTemp = static_cast<T*>(Reallocator::Reallocate(pT, nBytes));
	if (pTemp == NULL)
	{
		Reallocator::Free(pT);
		return NULL;
	}

	return pTemp;
}

// TODO: Remove this type.
class CHandle
{
public:
	CHandle() throw();
	CHandle(_Inout_ CHandle& h) throw();
	explicit CHandle(_In_ HANDLE h) throw();
	~CHandle() throw();

	CHandle& operator=(_Inout_ CHandle& h) throw();

	operator HANDLE() const throw();

	// Attach to an existing handle (takes ownership).
	void Attach(_In_ HANDLE h) throw();
	// Detach the handle from the object (releases ownership).
	HANDLE Detach() throw();

	// Close the handle.
	void Close() throw();

public:
	HANDLE m_h;
};

inline CHandle::CHandle() throw() :
	m_h( NULL )
{
}

inline CHandle::CHandle(_Inout_ CHandle& h) throw() :
	m_h( NULL )
{
	Attach( h.Detach() );
}

inline CHandle::CHandle(_In_ HANDLE h) throw() :
	m_h( h )
{
}

inline CHandle::~CHandle() throw()
{
	if( m_h != NULL )
	{
		Close();
	}
}

inline CHandle& CHandle::operator=(_Inout_ CHandle& h) throw()
{
	if( this != &h )
	{
		if( m_h != NULL )
		{
			Close();
		}
		Attach( h.Detach() );
	}

	return( *this );
}

inline CHandle::operator HANDLE() const throw()
{
	return( m_h );
}

inline void CHandle::Attach(_In_ HANDLE h) throw()
{
	ATLASSUME( m_h == NULL );
	m_h = h;  // Take ownership
}

inline HANDLE CHandle::Detach() throw()
{
	HANDLE h;

	h = m_h;  // Release ownership
	m_h = NULL;

	return( h );
}

inline void CHandle::Close() throw()
{
	if( m_h != NULL )
	{
		::CloseHandle( m_h );
		m_h = NULL;
	}
}

class CCritSecLock
{
public:
	_Post_same_lock_(cs, this->m_cs)
	_When_(bInitialLock != 0, _Acquires_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked != 0))
	_When_(bInitialLock == 0, _Post_satisfies_(this->m_bLocked == 0))
	CCritSecLock(
		_Inout_ CRITICAL_SECTION& cs,
		_In_ bool bInitialLock = true) : m_cs(cs), m_bLocked(false)
	{
		if( bInitialLock )
		{
			Lock();
		}		
	}
		
	_When_(this->m_bLocked != 0, _Requires_lock_held_(this->m_cs) _Releases_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked == 0))
	~CCritSecLock() throw()
	{
		if( m_bLocked )
		{
			Unlock();
		}
	}

	_Acquires_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked != 0)
	void Lock()
	{
		ATLASSERT( !m_bLocked );

		::EnterCriticalSection( &m_cs );
		m_bLocked = true;
	}

	_Releases_lock_(this->m_cs) _Post_satisfies_(this->m_bLocked == 0)
	void Unlock() throw()
	{
		ATLASSUME( m_bLocked );
		::LeaveCriticalSection( &m_cs );
		m_bLocked = false;	
	}

// Implementation
private:
	CRITICAL_SECTION& m_cs;
	bool m_bLocked;

// Private to avoid accidental use
	CCritSecLock(_In_ const CCritSecLock&) throw();
	CCritSecLock& operator=(_In_ const CCritSecLock&) throw();
};


}	// namespace ATL

#ifdef _ATL_ATTRIBUTES
#include <atlplus.h>
#endif


#pragma pack(pop)
#ifdef _ATL_ALL_WARNINGS
#pragma warning( pop )  // disable 4668/4820/4917/4127/4097/4786/4291/4201/4103/4268
#endif

#ifdef _ATL_ALL_USER_WARNINGS
#pragma warning( pop )  // disable 4505/4710/4514/4511/4512/4355
#endif

/////////////////////////////////////////////////////////////////////////////

#endif // __ATLBASE_H__
