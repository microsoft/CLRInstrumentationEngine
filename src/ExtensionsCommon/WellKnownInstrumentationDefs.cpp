// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "stdafx.h"
#include "WellKnownInstrumentationDefs.h"

LPCWSTR WellKnownMscorlibAssemblyName = L"mscorlib";
LPCWSTR WellKnownMscorlibModuleName = L"mscorlib.dll";

LPCWSTR InterceptExtensionAssemblyName = L"Microsoft.AI.Agent.Intercept";
LPCWSTR InterceptExtensionModuleName = L"Microsoft.AI.Agent.Intercept.dll";

LPCWSTR InstrumentationNativeMethodDecorate = L"Microsoft.Diagnostics.Instrumentation.Extensions.Intercept.Internal.NativeMethods.Decorate";
LPCWSTR InstrumentationNativeMethodGetTrace = L"Microsoft.Diagnostics.Instrumentation.Extensions.Intercept.Internal.NativeMethods.GetTrace";
LPCWSTR InstrumentationNativeMethodEnableTrace = L"Microsoft.Diagnostics.Instrumentation.Extensions.Intercept.Internal.NativeMethods.EnableTrace";
LPCWSTR InstrumentationNativeMethodIsAttached = L"Microsoft.Diagnostics.Instrumentation.Extensions.Base.NativeMethods.IsAttached";

LPCWSTR ExtensionsBaseOriginalModuleName = L"Microsoft.Diagnostics.Instrumentation.Extensions.Base.dll";

LPCWSTR ExtensionsBaseAssemblyName = L"mscorlib";
LPCWSTR ExtensionsBaseModuleName = L"mscorlib.dll";
LPCWSTR ExtensionsBasePublicContractType = L"System.Diagnostics.DebuggerHiddenAttribute";
LPCWSTR ExtensionsBaseFacadeTypeOnBeginMethodName = L"ApplicationInsights_OnBegin";
LPCWSTR ExtensionsBaseFacadeTypeOnEndMethodName = L"ApplicationInsights_OnEnd";
LPCWSTR ExtensionsBaseFacadeTypeOnExceptionMethodName = L"ApplicationInsights_OnException";

const DWORD MinimalSigSize = 3;
const DWORD ArgsCountSigIndex = 1;
const DWORD MaxCustomArgsCount = 14;