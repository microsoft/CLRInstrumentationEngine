#pragma once

// This file is a workaround: vsassert has __in macros that conflicts in C++.

#ifndef __UNIX_VSASSERT_H__
#define __UNIX_VSASSERT_H__

#ifdef DEBUG

// TODO: Use PAL MessageBox
#define VSASSERT(fTest, szMsg)                                           \
    do {                                                                 \
        if (!(fTest)) {   \
            fprintf(stderr,                                              \
                  "ASSERT FAILED: %s\n"                                  \
                  "\tExpression: %s\n"                                   \
                  "\tLocation:   line %d in %s\n"                        \
                  "\tFunction:   %s\n",                                  \
                  (#szMsg), #fTest, __LINE__, __FILE__, __FUNCTION__);   \
            if (IsDebuggerPresent())                                     \
                DebugBreak();                                            \
        }                                                                \
  } while (false)                                                        \
  
#define VSFAIL(szMsg) VSASSERT(0, #szMsg)
#define ASSERT(x) VSASSERT(x, "")
#define ASSERT_MSG(x, msg) VSASSERT(x, msg)

void __cdecl DisplayTraceW(LPCWSTR pszFormat, ...);
void __cdecl IndentTrace(int indent);

#define TRACE DisplayTraceW

extern bool g_fDisplayTrace;
extern int  g_TraceIndent;

#else  // DEBUG

#define VSASSERT(cond, msg)
#define VSFAIL(szMsg)
#define ASSERT(x)
#define ASSERT_MSG(x, msg)
#define TRACE 

#endif // DEBUG

inline void __cdecl _DebugNop(...) {}

#define DEFINE_ASSERT_TITLE(title)
#define DEFINE_ASSERT_MSG(msg)
#define VSDEBUGPRINTF 1 ? (void)0 : _DebugNop
#define VSDEBUGPRINTIF VSDEBUGPRINTF

#endif // __UNIX_VSASSERT_H__
