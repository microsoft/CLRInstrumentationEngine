// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#ifndef __REFCOUNT_H__
#define __REFCOUNT_H__

namespace ExtensionsHostCrossPlat
{
    // ----------------------------------------------------------------------------
    // CRefCount macros

#ifdef DEBUG

#define INITIALIZE_REF_RECORDER(dwOptions) MicrosoftInstrumentationEngine::CRefCount::InitRecorder((dwOptions))
#define TERMINATE_REF_RECORDER MicrosoftInstrumentationEngine::CRefCount::TerminateRecorder()

#define DEFINE_REFCOUNT_NAME(cls) m_pszCRefCountClassName = (LPTSTR)_T(#cls)

#define DUMP_REFCOUNT CRefCount::DumpRefCount()

#else // !DEBUG

#define INITIALIZE_REF_RECORDER(fOn)
#define TERMINATE_REF_RECORDER

#define DEFINE_REFCOUNT_NAME(cls)

#define DUMP_REFCOUNT

#endif // DEBUG

    // ----------------------------------------------------------------------------
    // Define generic method delegation from a derived class to a base class.
    //
    // DEFINE_DELEGATED_METHOD(InprocServerClassFactory, ClassFactory, HRESULT, LockServer, (BOOL bLock), (bLock));
    // defines a delegated method call from InprocServerClassFactory::LockServer() to ClassFactory::LockServer().

#define DEFINE_DELEGATED_METHOD(base_cls, result_type, method, derived_paren_args, base_paren_args) \
    result_type STDMETHODCALLTYPE method derived_paren_args \
    { \
    return base_cls::method base_paren_args; \
}

#define DEFINE_DELEGATED_REFCOUNT_ADDREF(cls) \
    DEFINE_DELEGATED_METHOD(CRefCount, ULONG, AddRef, (void), ())
#define DEFINE_DELEGATED_REFCOUNT_RELEASE(cls) \
    DEFINE_DELEGATED_METHOD(CRefCount, ULONG, Release, (void), ())

#define DEFINE_DELEGATED_REFCOUNT_ADDREF_DEBUG(cls) STDMETHOD_(ULONG, AddRef)() { ULONG result = CRefCount::AddRef(); VSASSERT(#cls); return result; }
#define DEFINE_DELEGATED_REFCOUNT_RELEASE_DEBUG(cls) STDMETHOD_(ULONG, Release)() { ULONG result = CRefCount::Release(); VSASSERT(#cls); return result; }

    // ----------------------------------------------------------------------------
    // CRefCount

    class CRefCount
    {
        // ctor/dtor
    public:
        CRefCount(void)
        {
            DEFINE_REFCOUNT_NAME(CRefCount);

            m_ulcRef = 1;

#ifdef DEBUG

            RecordCreation(this);
#endif

        }
        virtual ~CRefCount()
        {
#ifdef DEBUG

            if (m_ulcRef > 0)
            {
                RecordDestruction(this);
            }
#endif // DEBUG

        }

        // IUnknown methods
    public:
        STDMETHOD_(ULONG, AddRef)(void)
        {
            //VSASSERT(m_ulcRef < ULONG_MAX, L"");

            ULONG ulcRef = (ULONG)(InterlockedIncrement((LPLONG)(&m_ulcRef)));

#ifdef DEBUG

            RecordAddRef(this);
#endif // DEBUG

            // Note: the return from InterlockedIncrement may only match the new value
            // in signed-ness and zero-ness, but the return value of AddRef()
            // is not guaranteed in any case.

            return ulcRef;
        }
        STDMETHOD_(ULONG, Release)(void)
        {
            //VSASSERT(m_ulcRef > 0, L"");

            ULONG ulcRef = (ULONG)(InterlockedDecrement((LPLONG)(&m_ulcRef)));

#ifdef DEBUG

            RecordRelease(this);
#endif // DEBUG

            if (ulcRef == 0)
            {
                delete this;
            }

            // Note: the return from InterlockedDecrement may only match the new value
            // in signed-ness and zero-ness, but the return value of Release()
            // is not guaranteed in any case.

            return ulcRef;
        }

        // ref count
    private:
        ULONG m_ulcRef;

#ifdef DEBUG

        // ------------------------------------------------------------
        // CRefCount recording stuff (for finding leaks)
        // ------------------------------------------------------------

    protected:
        LPTSTR m_pszCRefCountClassName;

    private:
        static DWORD g_dwRecorderOptions;     // Recording is optional.
        static bool g_fRecordingInitialized;
        static class CRefRBMap g_map;
        static CRITICAL_SECTION g_crst;

        static void RecordCreation(CRefCount *pCRefCount);
        static void RecordDestruction(CRefCount *pCRefCount);
        static void RecordAddRef(CRefCount *pCRefCount);
        static void RecordRelease(CRefCount *pCRefCount);
    public:
        static void DumpRefCount(void);

    public:
        enum RecorderOptions
        {
            EnableRecorder = 0x01,
        };

        static void InitRecorder(DWORD dwOptions);
        static void TerminateRecorder(void);

#endif // DEBUG
    };

    // ----------------------------------------------------------------------------
    // CModuleRefCount

    class CModuleRefCount : public CRefCount
    {
        // ctor/dtor
    public:
        CModuleRefCount(void)
        {
            IncModuleUsage();
        }
        ~CModuleRefCount(void)
        {
            DecModuleUsage();
        }

        // module ref count support
    public:
        static ULONG IncModuleUsage(void)
        {
            //VSASSERT(s_ulcModuleRef < ULONG_MAX, L"");

            return (ULONG)InterlockedIncrement((PLONG)(&s_ulcModuleRef));
        }
        static ULONG DecModuleUsage(void)
        {
            // This assert isn't thread-safe but its bug-catching ability is valuable
            // enough to risk a few nuisance alerts.
            //VSASSERT(s_ulcModuleRef > 0, L"");
            ULONG ulcRef = InterlockedDecrement((PLONG)(&s_ulcModuleRef));

            return ulcRef;
        }
        static ULONG GetModuleUsage(void);
    private:
        static ULONG s_ulcModuleRef;
    };
}

#endif  // ! __REFCOUNT_H__
