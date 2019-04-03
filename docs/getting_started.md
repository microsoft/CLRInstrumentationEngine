# Getting Started with the CLR Instrumentation Engine

## Supported Domains

Issues specific to these domains will be listed in the below links.

* [Azure App Service](scenarios/azureappservice.md)
  * Shipped as part of the [Microsoft Application Insights Private Site Extension](scenarios/applicationinsights.md)
  * Shipped as a preinstalled site extension and enabled via [Microsoft Visual Studio Snapshot Debugger](scenarios/snapshotdebugger.md)

## Introduction

### What is the CLR?

The CLR stands for the Common Language Runtime. It is the runtime for Microsoft's .NET framework and provides execution support for programs written in managed .NET languages such as C#, C++ CLI, F#, and VB.Net. Compiling code written in a managed language results in bytecode called the Common Intermediate Language (CIL). The CIL is platform and processor independent and it is up to the CLR to Just-In-Time (JIT) compile the CIL into machine code in order to be executed. The CIL and the CLR together make up the Common Language Infrastructure (CLI).

In addition to the CLR which supports the full .NET Framework, Microsoft has also released CoreCLR to support .NET Core libraries.

### What is the CLR Instrumentation Engine?

The CLR exposes a profiling API which reports events and messages of what is happening to during the execution of a managed process or application (such as what methods are JIT compiled or if a class is loaded). The problem is that only one profiler can interact with the CLR Profiler APIs at a time. The CLR Instrumentation Engine was written to address this by acting as a host and inverse multiplexes the APIs for multiple profiler clients.

See the [MSDN Profiling Overview](https://docs.microsoft.com/en-us/dotnet/framework/unmanaged-api/profiling/profiling-overview) for more details about profilers in general.

See the [Design Notes](../DESIGN-NOTES.md) for in-depth details of the CLR Instrumentation Engine.

## Using the CLR Instrumentation Engine

### How do I enable the CLR Instrumentation Engine?

When a managed process starts execution, it must first load the CLR. Once the CLR is initialized, it then checks environment variables to see if a Profiler should be loaded.

#### Required Dlls
The Instrumentation Engine supports both x86 and x64 configurations.

* MicrosoftInstrumentationEngine_x86.dll
* MicrosoftInstrumentationEngine_x64.dll

See [Environment Variables](environment_variables.md) for more details on configuring your environment.

Make sure to include this environment variable if you are testing unsigned debug builds of your Instrumentation Method.

`MicrosoftInstrumentationEngine_DisableCodeSignatureValidation = 1`

### How do I write an Instrumentation Method?

A simple example of an InstrumentationMethod can be found in [InstrumentationMethod.cpp](../tests/InstrEngineTests/NaglerInstrumentationMethod/InstrumentationMethod.cpp).

### How do I provide my Instrumentation Method to the CLR Instrumentation Engine?

See [Configuration](configuration.md) for more details