// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

using namespace ATL;

namespace MicrosoftInstrumentationEngine
{
    // for non-rejit cases, the clr has a fairly broken model for
    // allocating function bodies. Rather than pass a buffer which is then
    // copied, it hands out a raw buffer to the clr's method heap.
    // However, since the profiler manager aggregates results of instrumentation,
    // it waits to do that allocation until everyone is finished with a method.
    // This class implements IMethodMalloc to hand to raw profiler callbacks
    // but cleans up the memory when its refcount hits 0
    class __declspec(uuid("4C4DCEB9-028F-4C25-A148-CEFFB9C21BE5"))
    CCorMethodMalloc : public IMethodMalloc, public CModuleRefCount
    {
    private:
        CAutoVectorPtr<BYTE> m_pBuffer;
        ULONG m_cbBufferLen;

    public:
        DEFINE_DELEGATED_REFCOUNT_ADDREF(CCorMethodMalloc);
        DEFINE_DELEGATED_REFCOUNT_RELEASE(CCorMethodMalloc);
        STDMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ void **ppvObject) override
        {
            return ImplQueryInterface(
                static_cast<IMethodMalloc*>(this),
                riid,
                ppvObject
                );
        }

    public:
        CCorMethodMalloc();

    public:
        virtual PVOID __stdcall Alloc(_In_ ULONG cb) override;

    public:
        // Called by CCorProfilerInfoWrapper to obtain the last allocation and validate the buffer
        // NOTE: Relies on the serialization of jit callbacks for thread safety.
        HRESULT GetCurrentBufferAndLen(_Out_ BYTE** ppBuffer, _Out_ ULONG* pcbBufferLen);

        // This is used for scenarios where a raw profiler hook uses Alloc to create memory
        // for a method, but does so earlier than expected (such as ModuleLoad).
        // We cannot cleanup this buffer since the runtime will use it for the final method body.
        HRESULT DetachCurrentBuffer();
    };
}