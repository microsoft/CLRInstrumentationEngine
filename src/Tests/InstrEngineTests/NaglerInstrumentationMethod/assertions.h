#pragma once


// Note, we use const char* because it is easier for cross-plat
inline void AssertFailed(_In_ const char* message)
{{
    #ifdef PLATFORM_UNIX
    // Just write to standard error
    fprintf(stderr, "%s", message);
    #else
        _ASSERT_EXPR(0, message);
    #endif
}}