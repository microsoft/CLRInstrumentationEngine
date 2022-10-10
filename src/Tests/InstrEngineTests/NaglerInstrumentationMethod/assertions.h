#pragma once


// Note, we use const char* because it is easier for cross-plat
inline void AssertFailed(_In_ const char* message)
{
#ifdef DEBUG

#ifdef PLATFORM_UNIX
    // Just write to standard error
    fprintf(stderr, "%s", message);
#else
    tstring wmessage;
    CommonLib::SystemString::Convert(message, wmessage);
    _ASSERT_EXPR(0, wmessage.c_str());
#endif

#endif // DEBUG
}