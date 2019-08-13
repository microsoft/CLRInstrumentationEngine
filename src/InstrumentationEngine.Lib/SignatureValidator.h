// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <windows.h>

class CSignatureValidator
{
    class WinTrustDataHolder
    {
    private:
        GUID& m_WVTPolicyGUID;
        WINTRUST_DATA& m_WinTrustData;

    public:
        WinTrustDataHolder(GUID& WVTPolicyGUID, WINTRUST_DATA& WinTrustData)
            : m_WVTPolicyGUID(WVTPolicyGUID)
            , m_WinTrustData(WinTrustData)
        {
        }

        ~WinTrustDataHolder()
        {
            // Any hWVTStateData must be released by a call with close.
            m_WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;

            LONG lStatus = WinVerifyTrust(
                NULL,
                &m_WVTPolicyGUID,
                &m_WinTrustData);
        }
    };

public:
    bool VerifyEmbeddedSignature(_In_ LPCWSTR pwszSourceFile);
};