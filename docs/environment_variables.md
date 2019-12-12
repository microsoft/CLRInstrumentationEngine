# The CLR Instrumentation Engine Environment Variables

See [Configuration](configuration.md) for details on how to setup your Instrumentation Method.

## Required Variables

These are the environment variables and values that are required to enable the CLR Instrumentation Engine:

| Variable | Value | Description |
|-|-|-|
COR_ENABLE_PROFILING|1|The CLR will only connect to a profiler if this is set to 1 (true).|
COR_PROFILER|"{324F817A-7420-4E6D-B3C1-143FBED6D855}"|The CLR will search for the CLSID or ProgID specified.|
COR_PROFILER_PATH_32 (see below)|"[FULL PATH TO MicrosoftInstrumentationEngine_x86.dll or InstrumentationEngine.ProfilerProxy_x86.dll]"|Skips the registry lookup, uses the 32bit dll from the path.
COR_PROFILER_PATH_64 (see below)|"[FULL PATH TO MicrosoftInstrumentationEngine_x64.dll or InstrumentationEngine.ProfilerProxy_x64.dll]"|Skips the registry lookup, uses the 64bit dll from the path.

Alternatively, for CoreCLR:

| Variable | Value | Description |
|-|-|-|
CORECLR_ENABLE_PROFILING|1|The CoreCLR will only connect to a profiler if this is set to 1 (true).
CORECLR_PROFILER|"{324F817A-7420-4E6D-B3C1-143FBED6D855}"|The CoreCLR will search for the CLSID or ProgID specified.
CORECLR_PROFILER_PATH_32 (see below)|"[FULL PATH TO MicrosoftInstrumentationEngine_x86.dll or InstrumentationEngine.ProfilerProxy_x86.dll]"|Skips the registry lookup, uses the 32bit dll from the path.
CORECLR_PROFILER_PATH_64 (see below)|"[FULL PATH TO MicrosoftInstrumentationEngine_x64.dll or InstrumentationEngine.ProfilerProxy_x64.dll]"|Skips the registry lookup, uses the 64bit dll from the path.

> Please see [Profiler Proxy](profilerproxy.md) for details on using the Profiler Proxy dll. We recommend setting the COR_PROFILER variables to
the proxy whenever applicable since it redirects to the latest CLRIE installed on every process re/start.

Additional variables needed to make the CLR Instrumentation Engine work:

| Variable | Value | Description |
|-|-|-|
MicrosoftInstrumentationEngine_LogLevel|"Errors\|Messages\|Dumps\|All"|Filters logs. Currently only "Errors" is allowed for EventLogging to prevent verbose logging.
MicrosoftInstrumentationEngine_LogLevel_GUID|"Errors\|Messages\|Dumps\|All"|Filters logs for InstrumentationMethods. Currently only "Errors" is allowed to prevent verbose logging.


## Optional and Miscellaneous Variables

These are the environment variables and values that aid in troubleshooting the CLR Instrumentation Engine:

| Variable | Value | Description |
|-|-|-|
MicrosoftInstrumentationEngine_DebugWait|1|Suspends the process until the debugger is attached.
MicrosoftInstrumentationEngine_FileLogPath|"[FULL PATH TO LOGGING FILE]"|File to host the event logs. This requires LogLevel to be set.
MicrosoftInstrumentationEngine_DisableCodeSignatureValidation|1|Disables signature validation
MicrosoftInstrumentationEngine_IsPreinstalled|1|The preinstalled site extension for CLRIE sets this to help users know that the applicationHost.xdt file for the preinstalled extension was applied. The Application Insights private site extension won't set this.

## Raw Profiler Hook

The RawProfilerHook allows one additional profiler that has not yet on-boarded to the ClrInstrumentationEngine to work with the Instrumentation Methods.

| Variable | Value | Description |
|-|-|-|
MicrosoftInstrumentationEngine_RawProfilerHook|"{GUID}"|This would be the value set to CORECLR/COR_PROFILER.
MicrosoftInstrumentationEngine_RawProfilerHookPath_32/64|"[FULL PATH TO raw profiler dll]"|This would be the value set to CORECLR/COR_PROFILER_PATH_32/64.

## Deprecated as of Version 1.0.22
The following variables allowed custom ExtensionHosts for the InstrumentationEngine. The responsibility of the ExtensionsHost involves setting
logging flags and loading Instrumentation Methods.

These variables are deprecated in favor of the default ExtensionsHost implementation to promote collaboration around one protocol for
onboarding Instrumentation Methods.

| Variable | Value | Description |
|-|-|-|
MicrosoftInstrumentationEngine_Host|"{CA487940-57D2-10BF-11B2-A3AD5A13CBC0}"|CLSID or ProgID of the IProfileManagerHost implementation which registers for raw COR profiler callbacks and handles configuration information about instrumentation methods that is passed back.
MicrosoftInstrumentationEngine_HostPath_32|"[FULL PATH TO Microsoft.InstrumentationEngine.ExtensionsHost_x86.dll]"|The 32bit dll hosting the implementation.
MicrosoftInstrumentationEngine_HostPath_64|"[FULL PATH TO Microsoft.InstrumentationEngine.ExtensionsHost_x64.dll]"|The 64bit dll hosting the implementation.