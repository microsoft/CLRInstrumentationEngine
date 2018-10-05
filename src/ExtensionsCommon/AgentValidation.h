// Copyright (c) Microsoft Corporation. All rights reserved.
// 

#pragma once

#include "FormatMessage.h"

#ifndef TraceCommonTo
#define TraceCommonTo(RESOLVE_LOGGER_EXPRESSION, MSG, SEVERITY_FLAG, METHOD_TO_CALL, ...) \
	do { auto pLogger = RESOLVE_LOGGER_EXPRESSION; if (nullptr != pLogger) { LoggingFlags flags = LoggingFlags_None; pLogger->GetLoggingFlags(&flags); if (flags & SEVERITY_FLAG) { std::wostringstream ssFmt; ssFmt << L"[msg=" << MSG << L"];"; Agent::Diagnostics::Format(ssFmt, __VA_ARGS__); ssFmt << L"[func=" << __FILEW__ << L':' << __LINE__ << L':' << __FUNCTIONW__ << L"];";  pLogger->METHOD_TO_CALL(ssFmt.str().c_str()); } } } while (false)
#endif

#ifndef TraceCommon
#define TraceCommon(MSG, SEVERITY_FLAG, METHOD_TO_CALL, ...) TraceCommonTo(this->GetLogger(), MSG, SEVERITY_FLAG, METHOD_TO_CALL, __VA_ARGS__)
#endif

#ifndef TraceMsg
#define TraceMsg(MSG, ...) do { TraceCommon(MSG, LoggingFlags_Trace, LogMessage, __VA_ARGS__); } while (false)
#endif

#ifndef TraceMsgTo
#define TraceMsgTo(RESOLVE_LOGGER_EXPRESSION, MSG, ...) do { TraceCommonTo(RESOLVE_LOGGER_EXPRESSION, MSG, LoggingFlags_Trace, LogMessage, __VA_ARGS__); } while (false)
#endif

#ifndef TraceError
#define TraceError(MSG, ...) do { TraceCommon(MSG, LoggingFlags_Errors, LogError, __VA_ARGS__); } while (false)
#endif

#ifndef TraceErrorTo
#define TraceErrorTo(RESOLVE_LOGGER_EXPRESSION, MSG) do { TraceCommonTo(RESOLVE_LOGGER_EXPRESSION, MSG, LoggingFlags_Errors, LogError); } while (false)
#endif

#ifndef IfFailRetHresult
#define IfFailRetHresult(EXPR, RETVAL) \
	do { if (FAILED(RETVAL = (EXPR))) { TraceError(L#EXPR); return RETVAL; } } while (false)
#endif

#ifndef IfFailRet
#define IfFailRet(EXPR) \
	do { auto __RETVAL = S_OK; if (FAILED(__RETVAL = (EXPR))) { TraceError(L#EXPR); return __RETVAL; } } while (false)
#endif

#ifndef IfFailToRet
#define IfFailToRet(RESOLVE_LOGGER_EXPRESSION, EXPR) \
	do { auto __RETVAL = S_OK; if (FAILED(__RETVAL = (EXPR))) { TraceErrorTo(RESOLVE_LOGGER_EXPRESSION, L#EXPR); return __RETVAL; } } while (false)
#endif

#ifndef IfFailThrow
#define IfFailThrow(EXPR) \
	do { auto __RETVAL = S_OK; if (FAILED(__RETVAL = (EXPR))) { TraceError(L#EXPR); throw Agent::Diagnostics::CException(__RETVAL); } } while (false)
#endif

#ifndef IfFailRetVoid
#define IfFailRetVoid(EXPR) \
	do { if (FAILED(hr = (EXPR))) { TraceError(L#EXPR); return; } } while (false)
#endif

#ifndef IfTrueRet
#define IfTrueRet(EXPR, result) \
	do { if (EXPR) { auto __RETVAL = result; TraceError(L#EXPR); return __RETVAL; } } while (false)
#endif

#ifndef IfTrueThrow
#define IfTrueThrow(EXPR, code) \
	do { if (EXPR) { auto __RETVAL = code; TraceError(L#EXPR); throw Agent::Diagnostics::CException(__RETVAL); } } while (false)
#endif

#ifndef IfFalseRet
#define IfFalseRet(EXPR, result) \
	do { if (!(EXPR)) { auto __RETVAL = result; TraceError(L#EXPR); return __RETVAL; } } while (false)
#endif

#ifndef IfTrueRetVoid
#define IfTrueRetVoid(EXPR) \
	do { if (EXPR) { TraceError(L#EXPR); return; } } while (false)
#endif

#ifndef IfFalseRetVoid
#define IfFalseRetVoid(EXPR) \
	do { if (!(EXPR)) { TraceError(L#EXPR); return; } } while (false)
#endif

#ifndef AGENT_TRY
#define AGENT_TRY try
#endif

#ifndef AGENT_CATCH_LOG_TO
#define AGENT_CATCH_LOG_TO(RESOLVE_LOGGER_EXPRESSION) catch(...) { TraceErrorTo(RESOLVE_LOGGER_EXPRESSION, L"Exception handled"); }
#endif

#ifndef AGENT_CATCH
#define AGENT_CATCH catch(...) { TraceError(L"Exception handled"); }
#endif

#ifndef DECLARE_NO_LOGGING
#define  AGENT_DECLARE_NO_LOGGING IProfilerManagerLoggingSptr m_spLogger; \
	const IProfilerManagerLoggingSptr& GetLogger() const \
	{ \
		return m_spLogger; \
	}
#endif