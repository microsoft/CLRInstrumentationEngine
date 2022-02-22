# Getting Started with the CLR Instrumentation Engine

1. [Introduction](#introduction)
2. [Using CLRIE](#using-clrie)

## Supported Domains

See [CLRIE in Azure](scenarios/azure.md)

## Introduction <a name="introduction" />

### What is the CLR?

The CLR stands for the Common Language Runtime. It is the runtime for Microsoft's .NET framework and provides execution support for programs
written in managed .NET languages such as C#, C++ CLI, F#, and VB.Net. Compiling code written in a managed language results in bytecode called
the Common Intermediate Language (CIL). The CIL is platform and processor independent and it is up to the CLR to Just-In-Time (JIT) compile the
CIL into machine code in order to be executed. The CIL and the CLR together make up the Common Language Infrastructure (CLI).

In addition to the CLR which supports the full .NET Framework, Microsoft has also released CoreCLR to support .NET Core libraries.

### What is the CLR Instrumentation Engine?

The CLR exposes a profiling API which reports events and messages of what is happening during the execution of a managed process or
application (such as what methods are JIT compiled or if a class is loaded). The problem is that only one profiler can interact with the CLR
Profiler APIs at a time. The CLR Instrumentation Engine was written to address this by acting as a host and inverse multiplexes the APIs for
multiple profiler clients.

See the [MSDN Profiling Overview](https://docs.microsoft.com/dotnet/framework/unmanaged-api/profiling/profiling-overview) for more details
about profilers in general.

See the [Design Notes](../DESIGN-NOTES.md) for in-depth details of the CLR Instrumentation Engine.

## Using the CLR Instrumentation Engine <a name="using-clrie" />

### Cooperative Model

Since the CLR Instrumentation Engine is meant to be shared, it is available as an x86 & x64 MSI/MSM installer on Windows desktop. The installer
sets up Instrumentation Engine with the following folder structure:

```
%ProgramFiles%|%ProgramFiles(x86)%
    Microsoft Instrumentation Engine
        <CLRIE Version>
            Instrumentation64|32
                <dlls>
        Proxy
            <Proxy Version>
                proxy.dll
```

We recommend setting the COR_PROFILER variables to the profiler proxy whenever applicable.

### How do I enable the CLR Instrumentation Engine?

When a managed process starts execution, it must first load the CLR. Once the CLR is initialized, it then checks environment variables to see
if a Profiler should be loaded.

See [Environment Variables](environment_variables.md) for more details on configuring your environment.

#### Required Dlls
The Instrumentation Engine supports both x86 and x64 configurations.

* MicrosoftInstrumentationEngine_x86.dll
* MicrosoftInstrumentationEngine_x64.dll

See [CLRIE in Azure](scenarios/azure.md) for details about leveraging CLRIE on supported Azure platforms and products.

### How do I write an Instrumentation Method?

A simple example of an InstrumentationMethod can be found in
[NaglerInstrumentationMethod.cpp](../src/Tests/InstrEngineTests/NaglerInstrumentationMethod/NaglerInstrumentationMethod.cpp).

### How do I provide my Instrumentation Method to the CLR Instrumentation Engine?

See [Configuration](configuration.md) for more details

### How do I use the Raw Profiler Hook?

The RawProfilerHook allows one additional profiler that has not yet on-boarded to the ClrInstrumentationEngine to work with the Instrumentation Methods.

The profiler callbacks will first iterate through each Instrumentation Method in priority order. Then the resultant IL is given to the ICorProfiler profiler listening on the RawProfilerHook.

In order to use the RawProfilerHook, you must modify these environment variables for the ICorProfiler profiler:

Variable Name|RawProfilerHook Variable Name 
:-|:-
CORECLR/COR_PROFILER|MicrosoftInstrumentationEngine_RawProfilerHook
CORECLR/COR_PROFILER_PATH_32/64|MicrosoftInstrumentationEngine_RawProfilerHookPath_32/64

See [Environment Variables](environment_variables.md) for more details.
