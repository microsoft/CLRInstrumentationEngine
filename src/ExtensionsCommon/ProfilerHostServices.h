// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "InstrumentationEngineInfraPointerDefs.h"
#include "AgentValidation.h"
#include "Exception.h"

class CProfilerHostServices
{
    CProfilerHostServices(const CProfilerHostServices&) = delete;
public:
    virtual ~CProfilerHostServices()
    {
        //_ASSERT(m_sptrLogger != nullptr);
    }

    template<
        typename T,
        typename... Args
    >
    HRESULT Create(_Out_ T** ppResult, Args&&... args)
    {
        IfTrueRet(nullptr == ppResult, E_INVALIDARG);

        try
        {
            *ppResult = new T(std::forward<Args>(args)...);
        }
        catch (const Agent::Diagnostics::CException& exc)
        {
            return exc.GetCode();
        }

        IfTrueRet(nullptr == *ppResult, E_OUTOFMEMORY);

        return S_OK;
    }

    template<
        typename TPointer,
        typename... Args
    >
    HRESULT Create(_Out_ TPointer& spResult, Args&&... args)
    {
        TPointer::element_type* pResult = nullptr;
        IfFailRet(Create(&pResult, std::forward<Args>(args)...));
        spResult.reset(pResult);

        IfTrueRet(nullptr == spResult, E_OUTOFMEMORY);

        return S_OK;
    }

    virtual HRESULT BuildUpMembers()
    {
        return S_OK;
    };

    template<typename T> HRESULT BuildUpObjPtr(T& spObject)
    {
        spObject->SetLogger(m_sptrLogger);
        return spObject->BuildUpMembers();
    }

    template<typename T> HRESULT BuildUpObjRef(T& obj)
    {
        obj.SetLogger(m_sptrLogger);
        return obj.BuildUpMembers();
    }

    template<
        typename TPointer,
        typename... Args
    >
    HRESULT CreateAndBuildUp(_Out_ TPointer& spResult, Args&&... args)
    {
        IfFailRet(Create(spResult, std::forward<Args>(args)...));
        IfFailRet(BuildUpObjPtr(spResult));

        return S_OK;
    }
protected:
    CProfilerHostServices() noexcept {}

	void SetLogger(const IProfilerManagerLoggingSptr& spLogger)
    {
        //_ASSERT(spLogger != nullptr);
        m_sptrLogger = spLogger;
    }

    const IProfilerManagerLoggingSptr& GetLogger() const
    {
        return m_sptrLogger;
    }

private:
    IProfilerManagerLoggingSptr		m_sptrLogger;
};