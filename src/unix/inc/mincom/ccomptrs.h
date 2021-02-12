// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// ccomptrs.h base types for managing basic reference counting and 
// casting of IUnknown types useful for when the entire platform
// abstraction layer, or Abstract Template Libraries are not available.

#pragma once

template <class T>
class _NoAddRefReleaseOnCComPtr : 
    public T
{
    private:
    
    ~_NoAddRefReleaseOnCComPtr() {}
    
    private:
        STDMETHOD_(ULONG, AddRef)()=0;
        STDMETHOD_(ULONG, Release)()=0;
};


//CComPtrBase provides the basis for all other smart pointers
//The other smartpointers add their own constructors and operators
template <class T>
class CComPtrBase
{
protected:
	CComPtrBase()
	{
		p = NULL;
	}
	CComPtrBase(int nNull)
	{
		(void)nNull;
		p = NULL;
	}
	CComPtrBase(T* lp)
	{
		p = lp;
		if (p != NULL)
			p->AddRef();
	}
public:
	typedef T _PtrClass;
	~CComPtrBase()
	{
		if (p)
			p->Release();
	}
	operator T*() const
	{
		return p;
	}
	T& operator*() const
	{
		return *p;
	}
	T** operator&()
	{
		return &p;
	}
	_NoAddRefReleaseOnCComPtr<T>* operator->() const
	{
		return (_NoAddRefReleaseOnCComPtr<T>*)p;
	}
	bool operator!() const
	{
		return (p == NULL);
	}
	bool operator<(T* pT) const
	{
		return p < pT;
	}
	bool operator==(T* pT) const
	{
		return p == pT;
	}

	// Release the interface and set to NULL
	void Release()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	}
	// Attach to an existing interface (does not AddRef)
	void Attach(T* p2)
	{
		if (p)
			p->Release();
		p = p2;
	}
	// Detach the interface (does not Release)
	T* Detach()
	{
		T* pt = p;
		p = NULL;
		return pt;
	}
	HRESULT CopyTo(T** ppT)
	{
		if (ppT == NULL)
			return E_POINTER;
		*ppT = p;
		if (p)
			p->AddRef();
		return S_OK;
	}

    T* p;
};

template <class T>
class CComPtr : public CComPtrBase<T>
{
public:
	CComPtr()
	{
	}
	CComPtr(int nNull) :
		CComPtrBase<T>(nNull)
	{
	}
	CComPtr(T* lp) :
		CComPtrBase<T>(lp)

	{
	}
	CComPtr(const CComPtr<T>& lp) :
		CComPtrBase<T>(lp.p)
	{
	}
	T* operator=(T* lp)
	{
		return static_cast<T*>(AtlComPtrAssign((IUnknown**)&this->p, lp));
	}
    T* operator=(const CComPtr<T>& lp)
	{
		return static_cast<T*>(AtlComPtrAssign((IUnknown**)&this->p, lp));
	}
};
