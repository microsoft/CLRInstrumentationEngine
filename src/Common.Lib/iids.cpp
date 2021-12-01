// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Defines IID_IUnknown within this library on Linux. We need to do this so that ImplQueryInterface can have a
// valid reference to IID_IUknown. In libraries that depend on the PAL, IID_IUnknown is redefined as clrie_IID_IUnknown.
// Both have the same value, but in order for ImplQueryInterface to correctly do the comparison, it needs its own
// copy in this library.
extern "C"
{
    __attribute__ ((visibility("default"))) typedef struct _IID
    {
        unsigned int x;
        unsigned short s1;
        unsigned short s2;
        unsigned char  c[8];
    } IID;

    __attribute__ ((visibility("default"))) typedef IID CLSID;

#define _DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        __attribute__ ((visibility ("default"))) extern const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

    _DEFINE_GUID(IID, IID_IUnknown, 0x00000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
}