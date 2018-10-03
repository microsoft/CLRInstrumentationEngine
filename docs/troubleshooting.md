# Troubleshooting

## Supported Scenarios

Issues specific to these domains will be listed below.

Issues specific to these domains will be listed in the below links.

* [Azure App Service](scenarios/azureappservice.md)
  * Shipped as part of the [Microsoft Application Insights Private Site Extension](scenarios/applicationinsights.md)
  * Shipped as a preinstalled site extension and enabled via [Microsoft Visual Studio Snapshot Debugger](scenarios/snapshotdebugger.md)

## Diagnosing Issues

### Enable Instrumentation Engine Logging

A quick way to see if any errors are occurring in the CLR Instrumentation Engine or Instrumentation Methods is to enable logging. Please see [Environment Variables](environment_variables.md) for detailed information on the allowed values to set.

#### Event Log

Setting `MicrosoftInstrumentationEngine_LogLevel` will enable logging to the local computer Application log under the source name "Instrumentation Engine". Currently LogLevel only supports `Error` level. `All` will default to `Error`. If this variable is missing or set to `None` then no event logging will occur.

These error event logs can be seen in the eventlog.xml in Azure App Services or in Event Viewer on Windows.

#### File Log

If `MicrosoftInstrumentationEngine_FileLogPath` is set along with `MicrosoftInstrumentationEngine_LogLevel`, then logging to the filepath will occur. If the FilePath is set to a directory, then an automatically generated `%TEMP%\ProfilerLog_[PID].txt` file is used, where [PID] represents the process id and the %TEMP% variable is evaluated against the user account running the process.

### Debugging the Instrumentation Engine

When using a local build of the Instrumentation Engine, you will need to disable signing validations and checks by setting the `MicrosoftInstrumentationEngine_DisableCodeSignatureValidation` to 1.

Since the CLR Instrumentation Engine initializes on process start, in order for breakpoints in the initialization logic to hit, you will need to set the `MicrosoftInstrumentationEngine_DebugWait` environment variable with the value 1. This will cause the Instrumentation Engine to wait on startup for a debugger attach event.