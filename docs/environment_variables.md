# The CLR Instrumentation Engine Environment Variables

## Required Variables

These are the environment variables and values that are required to enable the CLR Instrumentation Engine:

| Variable | Value | Description |
|-|-|-|
COR_ENABLE_PROFILING|1|The CLR will only connect to a profiler if this is set to 1 (true).|
COR_PROFILER|"{324F817A-7420-4E6D-B3C1-143FBED6D855}"|The CLR will search for the CLSID or ProgID specified.|
COR_PROFILER_PATH_32|"[FULL PATH TO MicrosoftInstrumentationEngine_x86.dll]"|Skips the registry lookup, uses the 32bit dll from the path.
COR_PROFILER_PATH_64|"[FULL PATH TO MicrosoftInstrumentationEngine_x64.dll]"|Skips the registry lookup, uses the 64bit dll from the path.

Alternatively, for CoreCLR:

| Variable | Value | Description |
|-|-|-|
CORECLR_ENABLE_PROFILING|1|The CoreCLR will only connect to a profiler if this is set to 1 (true).
CORECLR_PROFILER|"{324F817A-7420-4E6D-B3C1-143FBED6D855}"|The CoreCLR will search for the CLSID or ProgID specified.
CORECLR_PROFILER_PATH_32|"[FULL PATH TO MicrosoftInstrumentationEngine_x86.dll]"|Skips the registry lookup, uses the 32bit dll from the path.
CORECLR_PROFILER_PATH_64|"[FULL PATH TO MicrosoftInstrumentationEngine_x64.dll]"|Skips the registry lookup, uses the 64bit dll from the path.

Additional variables needed to make the CLR Instrumentation Engine work:

| Variable | Value | Description |
|-|-|-|
MicrosoftInstrumentationEngine_Host|"{CA487940-57D2-10BF-11B2-A3AD5A13CBC0}"|CLSID or ProgID of the IProfileManagerHost implementation which registers for raw COR profiler callbacks and handles configuration information about instrumentation methods that is passed back.
MicrosoftInstrumentationEngine_HostPath_32|"[FULL PATH TO Microsoft.InstrumentationEngine.ExtensionsHost_x86.dll]"|The 32bit dll hosting the implementation.
MicrosoftInstrumentationEngine_HostPath_64|"[FULL PATH TO Microsoft.InstrumentationEngine.ExtensionsHost_x64.dll]"|The 64bit dll hosting the implementation.
MicrosoftInstrumentationEngine_LogLevel|"Errors"|Filters event logs. Currently only "Errors" is allowed to prevent verbose logging.

## Optional and Miscellaneous Variables

These are the environment variables and values that aid in troubleshooting the CLR Instrumentation Engine:
| Variable | Value | Description |
|-|-|-|
MicrosoftInstrumentationEngine_DebugWait|1|Suspends the process until the debugger is attached.
MicrosoftInstrumentationEngine_FileLogPath|"[FULL PATH TO LOGGING FILE]"|File to host the event logs. This requires LogLevel to be set.
MicrosoftInstrumentationEngine_DisableCodeSignatureValidation|1|Disables signature validation
MicrosoftInstrumentationEngine_IsPreinstalled|1|The preinstalled site extension for CLRIE sets this to help users know that the applicationHost.xdt file for the preinstalled extension was applied. The Application Insights private site extension won't set this.