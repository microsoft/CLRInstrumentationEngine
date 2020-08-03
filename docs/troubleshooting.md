# Troubleshooting

## Azure Scenarios

See [CLRIE in Azure](scenarios/azure.md).

## Mitigation

When an issue occurs in production, the first step is to mitigate it so the application continues to run. A common cause of problems are from updates to either CLRIE or to one of the Instrumentation Method client that introduces potential conflicts with another client or incompatiblities with the app.

### Remove CLRIE

The most direct way to stop CLRIE and all Instrumentation Methods is to remove the ICorProfiler environment variables that are being applied to the process (along with a process restart). This may either require updating applicationHost.xdt files, changing the `InstrumentationEngine_EXTENSION_VERSION` Application Setting, or modifying registry keys. Without the ICorProfiler environment variables, the process effectively removes CLRIE & all IMs that are currently running. If the application without CLRIE is still encountering errors, then the issue is something the app owner will need to address.

Alternatively, if the user cannot modify the ICorProfiler environment variable, they may be able to at least remove the dlls which are pointed to by the COR_PROFILER_PATH/CORECLR_PROFILER_PATH environment variables. A process restart will still be required.

### Remove Instrumentation Methods

Currently this feature is not supported. We could potentially create a tool that supports an allow/block list of InstrumentationMethods that CLRIE uses to ignore environment variables on startup. Concerns of this implementation involve security implications of file-on-disk or require privileged user access.

## Investigations and Diagnosing Issues

After mitigation and having the application back up and running, the next step is to figure out who the culprit is that produced the error. We recommend using a test environment rather than testing in production to prvent giving customers a very negative experience.

### Enable CLRIE only

It is recommended to just enable CLRIE without any Instrumentation Methods to ensure there's no regression in CLRIE's steady-state behavior. This is especially important if CLRIE was recently updated because regressions or incomptibilities might be introduced for the current application. We expect these issues to be rare as CLRIE is tested on a variety of platforms and apps and features are added incrementally with emphasis on supporting backwards compatibility.

### Enable Instrumentation Methods (one at a time)

Profilers are the main feature providers and so should be the focal point of investigation. Instrumentation Methods are loaded by CLRIE via environment variables that have the prefix `MicrosoftInstrumentationEngine_ConfigPath64_` or `MicrosoftInstrumentationEngine_ConfigPath32_` so app owners can easily determine what Instrumentation Methods are running in their process.

The user should enable each Instrumentation Method by itself and see if the application runs into the issue. This can confirm directly whether an Instrumentation Method by itself is causing the issue. This does not confirm if the Instrumentation Method is causing the issue or if it relies on behavior from CLRIE that is broken or has bugs.

The more complex scenarios occur when multiple Instrumentation Methods are running as their behaviors may conflict without either parties being aware. For these scenarios it is difficult to find a sole owner. We **recommend** starting a discussion by posting a GitHub issue with tags for each of the involved Instrumentation Methods products. Private discussions or discussions with sensitive information can be done via email or other channels once the involved parties are established.

### Enable Instrumentation Engine Logging

A quick way to see if any errors are occurring in the CLR Instrumentation Engine or Instrumentation Methods is to enable logging. Please see
[Environment Variables](environment_variables.md) for detailed information on the allowed values to set. For Instrumentation Method authors, we recommend enabling the "Dumps" log level which shows the IL transformations.

#### Event Log

Setting `MicrosoftInstrumentationEngine_LogLevel` will enable logging to the local computer Application log under the source name
"Instrumentation Engine". Currently LogLevel only supports `Error` level. `All` will default to `Error`. If this variable is missing or set to
`None` then no event logging will occur.

These error event logs can be seen in the eventlog.xml in Azure App Services or in Event Viewer on Windows.

#### File Log

If `MicrosoftInstrumentationEngine_FileLogPath` is set along with `MicrosoftInstrumentationEngine_LogLevel`, then logging to the filepath will
occur. If the FilePath is set to a directory, then an automatically generated `%TEMP%\ProfilerLog_[PID].txt` file is used, where [PID]
represents the process id and the %TEMP% variable is evaluated against the user account running the process.

#### Instrumentation Method Logging

CLRIE's logging infrastructure is outlined [here](./logging.md). During ICorProfilerCallback::Initialize(), InstrumentationMethods can obtain an InstrumentationMethod-specific logger via `IProfilerManager::GetLoggingInstance()` which will prepend log messages with the `[IM:GUID]`. In addition, IMs can log at a different log level from CLRIE by setting an additional environment variable, allowing IMs to filter out CLRIE or other IM logs. See [ProfilerManagerForInstrumentationMethod](./logging.md#profilermanagerforinstrumentationmethod-hcpp) section for more details.

Log parsing or an event collector can utilize these patterns to track InstrumentationMethod behaviors and potentially help diagnose issues.

### Debugging the Instrumentation Engine

When using a local build of the Instrumentation Engine or testing your InstrumentationMethod, you will need to disable signing validations and checks by setting the
`MicrosoftInstrumentationEngine_DisableCodeSignatureValidation` to 1.

Since the CLR Instrumentation Engine initializes on process start, in order for breakpoints in the initialization logic to hit, you will need to set the `MicrosoftInstrumentationEngine_DebugWait` environment variable with the value of 1. This will cause the Instrumentation Engine to wait on startup for a debugger attach event.

#### Breakpoint Locations

The best place to start debugging is in [ProfilerManager.cpp](../src/InstrumentationEngine/ProfilerManager.cpp). ProfilerManager is CLRIE's ICorProfiler implementation which gets the first-chance callback during profiling. It controls setting up InstrumentationMethods and forwarding ICorProfiler callback events to them.

Setting a breakpoint inside `CProfilerManager::Initialize()` provides the earliest point where CLRIE is beginning to initialize in the process.

`CProfilerManager::AddInstrumentationMethod()` provides the logic for loading each InstrumentationMethod and calling Initialize() on them.

CLRIE's main purpose is to coordinate instrumentation, so callbacks such as `CProfilerManager::JITCompilationStarted()` and `CProfilerManager::GetReJITParameters()` are also good places to inspect.