# Logging in CLRIE

1. [Overview](#overview)
2. [Logging Flags](#loggingflags)
3. [Classes](#loggingclasses)
2. [Consumers](#consumers)

## Overview <a name="overview" />

The logging infrastructure provides both CLRIE and Instrumentation Method clients the capability to log to different mediums such as files, eventsource, and debug port.

The main motivation for the logging infrastructure design is to address the side-by-side CLR's in the same process (for example .NET framework 4.8 and .Net core 2.2 or multiple versions of .Net core). This is a common scenario in Azure App Service due to Asp.NET Core's in-process hosting model. Since multiple CLRs will each initialize a Profiler, there can be multiple instances of CLRIE which will disrupt assumptions made around static state.

## Logging Flags <a name="loggingflags" />

### Classification
Log messages are separated into three main categories defined as a logging flags enum in [InstrumentationEngine.idl](../src/InstrumentationEngine.Api/InstrumentationEngine.idl)

```
// logging levels in the instrumentation engine.
enum LoggingFlags
{
    LoggingFlags_None = 0x0,                      // No logging
    LoggingFlags_Errors = 0x1,                    // Log error messages
    LoggingFlags_Trace = 0x2,                     // Log verbose trace messages
    LoggingFlags_InstrumentationResults = 0x4     // Log full dumping of il results.
};
```

These enum flags are initialized to support bitwise operations so multiple flags can be manipulated via OR/AND operations.

### Configuring the LoggingFlags
Colloquially, the term "Messages" is used for Trace and the term "Dumps" is used for InstrumentationResults. There are several Environment Variables involved for configuring the logging flags.

`MicrosoftInstrumentationEngine_LogLevel` configures the global logging level (ie. the CLogging class)

Instrumentation Methods can specify a `MicrosoftInstrumentationEngine_LogLevel_GUID` environment variable where the GUID corresponds to the method's ClassId.

The GUID format is "00000000-0000-0000-0000-000000000000" without curly brackets { }. If no LogLevel is specified by the Method, it will fallback to the global LogLevel.

The values for these LogLevel environment variables are:
* "Errors"
* "Messages"
* "Dumps"
* "All"

Combination of flags can be set using the `|` character such as `Errors|Messages|Dumps` or a subset such as `Errors|Dumps`.

See FileLoggerSink section for details around `MicrosoftInstrumentationEngine_FileLogPath` and `MicrosoftInstrumentationEngine_FileLog`.


## Logging Classes <a name="loggingclasses" />
There are three key classes that make up the Logging infrastructure:
* [CLogging](#clogging)
* [CLoggerService](#cloggerservice)
* [ILoggerSink](#iloggersink)

Throughout the CLRIE codebase, CLogging is used directly. InstrumentationMethods can query via `IProfilerManager4::GetGlobalLoggingInstance()` and `IProfilerManager::GetLoggingInstance()`.

### CLogging <a name="clogging" /> ([.h](../src/InstrumentationEngine.Lib/Logging.h)|[.cpp](../src/InstrumentationEngine.Lib/Logging.cpp))

CLogging is a static class that essentially forwards calls to its static singleton CLoggerService.

The main functionality exposed by CLogging is the methods `LogMessage()`, `LogError()`, and `LogDumpMessage()` for each of the LoggingFlags mentioned preivously. The messages are passed to CLoggerService which forwards them to the specified logger sinks that are enabled for the specific logging level.

CLogging also contains other functionality:
* `SetLoggingFlags(_In_ LoggingFlags loggingFlags)` - sets the global LoggingFlag (see CLoggerService section)
* `SetLogToDebugPort(_In_ bool enable)` - enables a DebugPort (see ILoggerSink section)
* `SetLoggingHost(_In_ IProfilerManagerLoggingHost* pLoggingHost)` - sets an optional LoggingHost (see ILoggerSink section)
* `UpdateInstrumentationMethodLoggingFlags(_In_ GUID classId, _In_ LoggingFlags loggingFlags)` - updates the LogLevel for a specific Instrumentation Method (See Consumers section)
* `class XmlDumpHelper final` - an Xml writer helper, used by the AssemblyInjector (outside the scope of this doc).

### CLoggerService <a name="cloggerservice" /> ([.h](../src/InstrumentationEngine.Lib/LoggerService.h)|[.cpp](../src/InstrumentationEngine.Lib/LoggerService.cpp))

The CLoggerServices contains three vectors for each logging level:
```
std::vector<std::shared_ptr<ILoggerSink>> m_allSinks;
std::vector<std::shared_ptr<ILoggerSink>> m_errorSinks;
std::vector<std::shared_ptr<ILoggerSink>> m_messageSinks;
std::vector<std::shared_ptr<ILoggerSink>> m_dumpSinks;
```

On initialization, the CLoggerService extracts the `MicrosoftInstrumentationEngine_LogLevel` environment variable to m_defaultFlags (aka the global logging flag), creates & initializes each sink, and recalculates the logging flags based on the sinks' capabilities.

The `CLoggerService::RecalculateLoggingFlags()` iterates through each sink and calls Reset() on it with m_defaultFlags and m_instrumentationMethodFlags (see Consumers section). Each sink will provide a subset of loggingFlags that it supports (its "effective" flag). For each of the logging level, if its effective flag supports it, then the sink will be added to the corresponding sink vector. Once finished, all of the effective flags calculated from the m_defaultFlags (not m_instrumentationMethodFlags) are OR'd to produce the overall m_effectiveFlag (aka the global effective logging flag).

`CLoggerService::GetLoggingFlags()` will return the m_effectiveFlags value so consumers can determine which logging flags are actually supported by sinks and self-filter to reduce the amount of noise in the system.

### ILoggerSink <a name="iloggersink" /> ([.h](../src/InstrumentationEngine.Lib/LoggerSink.h))

The ILoggerSink is an interface surface that is implemented by actual LoggerSinks and consises of the three Log() functions, Initialize(), and Reset().

As mentioned in the CLoggerService section, Reset() is the mechanism where the sink declares what subset of loggingflags it supports.

From `CLoggerService::CreateFlags()`:

```
#ifndef PLATFORM_UNIX
    // Windows-only
    sinks.push_back(make_shared<CDebugLoggerSink>());
    sinks.push_back(make_shared<CEventLoggerSink>());
#endif
    // Both Windows & Linux
    sinks.push_back(make_shared<CFileLoggerSink>());
    sinks.push_back(make_shared<CHostLoggerSink>());
```

#### CDebugLoggerSink ([.h](../src/InstrumentationEngine.Lib/DebugLoggerSink.h)|[.cpp](../src/InstrumentationEngine.Lib/DebugLoggerSink.cpp))

Log() functions call to the Windows API [OutputDebugString](https://docs.microsoft.com/windows/win32/api/debugapi/nf-debugapi-outputdebugstringw).

If there is no logging host or `CLoggerService::GetLogToDebugPort()` returns true, then all logging is enabled.

#### CEventLoggerSink ([.h](../src/InstrumentationEngine.Lib/EventLoggerSink.h)|[.cpp](../src/InstrumentationEngine.Lib/EventLoggerSink.cpp))

Derives from EventLoggingBase ([.h](../src/Common.Lib/EventLoggingBase.h)|[.cpp](../src/Common.Lib/EventLoggingBase.cpp)) which is shared with the ProfilerProxy logging infrastructure, this class creates an "Instrumentation Engine" event source. The base class spins up a separate thread that has a while loop and waits on a queue for messages. Log() functions append messages to this queue.

The separate thread is needed because dequeued messages are forwarded to the [ReportEvent](https://docs.microsoft.com/windows/win32/api/winbase/nf-winbase-reporteventw) Windows API which requires `CoInitialize(NULL)` due to their detouring logic of eventlogging messages and since CLRIE live inside the user's process which is not guaranteed to be a Single-Threaded Apartment (STA).

The EventLoggerSink is restricted to Error messages to reduce noise.

#### CFileLoggerSink ([.h](../src/InstrumentationEngine.Lib/FileLoggerSink.h)|[.cpp](../src/InstrumentationEngine.Lib/FileLoggerSink.cpp))

File logging is configured not only by the LogLevel variable but also the `MicrosoftInstrumentationEngine_FileLogPath` variable to specify where the log file is generated. If the FileLogPath is set to a directory, then the filename defaults to "ProfilerLog_PID.txt" where PID is the process id.

For legacy reasons, CLRIE also supports `MicrosoftInstrumentationEngine_FileLog` variable which supercedes the global log level.

#### CHostLoggerSink ([.h](../src/InstrumentationEngine.Lib/HostLoggerSink.h)|[.cpp](../src/InstrumentationEngine.Lib/HostLoggerSink.cpp))

CLRIE supports the notion of allowing Instrumentation Methods to set a custom LoggingHost which is directly settable via the `IProfilerManager` interface.

The custom host logger just needs to implemented the `IProfilerManagerLoggingHost` which consists of the three Log*() functions only.

Both of these interfaces can be found in [InstrumentationEngine.idl](../src/InstrumentationEngine.Api/InstrumentationEngine.idl)

## Logging Consumers <a name="consumers" />

CLRIE either calls CLogging directly or via the `IfFailRet` macro. Instrumentation Methods are provided two mechanisms for getting loggers: either by `IProfilerManager4::GetGlobalLoggingInstance()` which returns an instance of LoggingWrapper or `IProfilerManager::GetLoggingInstance()` which is hijacked by ProfilerManagerForInstrumentationMethod to return itself. Both functions return the type `IProfilerManagerLogging`, however Instrumentation Methods should query using the GetLoggingInstance() for the majority of use cases.

### LoggingWrapper ([.h](../src/InstrumentationEngine/LoggingWrapper.h))

The LoggingWrapper is a thin static wrapper around the CLogging static class and allows InstrumentationMethods to log independently of any profiler manager's lifetime. This plays an important role in SxS CLR scenarios as InstrumentationMethods can log from a static context if needed.

### ProfilerManagerForInstrumentationMethod ([.h](../src/InstrumentationEngine.Lib/HostLoggerSink.h)|[.cpp](../src/InstrumentationEngine.Lib/HostLoggerSink.cpp))

ProfilerManagerForInstrumentationMethod (PMforIM) is a thin wrapper around the CProfilerManager instance. It contains information about the InstrumentationMethod classId GUID and parses the corresponding environment variable `MicrosoftInstrumentationEngine_LogLevel_GUID`. It hijacks Log*() calls by filtering against the InstrumentationMethod loglevel as well as injecting a prefix `[IM:GUID]` before each Log*() call to differentiate from other InstrumentationMethods.

An instance of PMforIM is created during `CProfilerManager::AddInstrumentationMethod()` and passed to the InstrumentationMethod initialize call. Once initialized, ProfilerManager queries the PMforIM to get the InstrumentationMethod's LogLevel and calls `CLogging::UpdateInstrumentationMethodLoggingFlags()` which updates CLoggerService's m_instrumentationMethodFlags. This allows the necessary sinks to be set up for the Instrumentation Method.