// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "unix.h"
//#include "vsassert.h"
//#include "atlstr.h"
#include "atlcoll.h"

#ifndef PLATFORM_UNIX
/* static*/
IAtlStringMgr* ATL::CAtlStringMgr::GetInstance()
{
    static CWin32Heap strHeap(::GetProcessHeap());
    static CAtlStringMgr strMgr(&strHeap);

    return &strMgr;
}

bool ATL::CAtlStringMgr::isInitialized = CAtlStringMgr::StaticInitialize();
#endif

// List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
static const UINT s_anPrimes[] =
{
    17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821,
    1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331,
    13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033,
    131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563,
    832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319,
    5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
};

UINT ATL::PickAtlHashMapSize(UINT nBinsEstimate)
{
    // Find the smallest prime greater than our estimate
    int iPrime = 0;
    while (nBinsEstimate > s_anPrimes[iPrime])
    {
        iPrime++;
    }

    if (s_anPrimes[iPrime] == UINT_MAX)
    {
        return(nBinsEstimate);
    }
    else
    {
        return(s_anPrimes[iPrime]);
    }
}