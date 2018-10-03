#pragma once

// For Linux builds, we need to define the structures necessary to generate IIDs
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    __attribute__ ((visibility("default"))) typedef struct _IID
    {
        unsigned long x;
        unsigned short s1;
        unsigned short s2;
        unsigned char  c[8];
    } IID;

    #define DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        __attribute__ ((visibility("default"))) extern const IID name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#ifdef __cplusplus
}
#endif