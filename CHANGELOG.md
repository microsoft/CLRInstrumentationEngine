Change Log

# Next
- The method `MicrosoftInstrumentationEngine::CModuleInfo::ImportModule` no longer imports custom attributes.
- Do not inject custom attributes into mscorlib.
- Wrapper of `ICorProfiler` object updated to `ICorProfilerInfo7`.

# ???
- feature 1
- feature 2

# 0.1.20 

- Updated to the Dev14 compiler
- Code Signing validation for all instrumentation methods on load
- Set environment variable `MicrosoftInstrumentationEngine_DisableCodeSignatureValidation` to disable code signing validation
- Support for `FooAsync` methods instrumentation 
- Allow to redirect traces to the file or directory set by environment variable `MicrosoftInstrumentationEngine_FileLogPath`
- Instrumentation Engine detect platform automatically and uses `InstrumentationMethod32` or `InstrumentationMethod64` in configuration file and `MicrosoftInstrumentationEngine_HostPath_32` or `MicrosoftInstrumentationEngine_HostPath_64` to load appropriate host.

# 0.1.19

- Moved to this repository