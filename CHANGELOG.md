# Change Log

Please format the changes as follows:

## Version Number
+ New:
+ BugFixes:
+ Updates:

## 1.0.43
+ BugFixes:
  + Fix XmlReader.Create to read from stream instead of filepath string
+ New:
  + On Unix, environment is now scanned for instrumentation methods instead of looking for hardcoded ProductionBreakpoints.

## 1.0.42
+ BugFixes:
  + Fix Linux build

## 1.0.41
+ Updates:
  + Remove PathCch functions in order to support Win7

## 1.0.40
+ No new changes.

## 1.0.39
+ BugFixes:
  + Remove MethodInfo creation from JitComplete events. This fixes a race condition when accessing method info map.
  + Fix CFileLoggerSink to flush writes before closing the file.

## 1.0.38
+ New:
  + Add build ID to binaries in linux

## 1.0.37
+ BugFixes:
  + Fix RPH Bug - Support raw profiler QI that only return the highest ICorProfilerCallback

## 1.0.36
+ BugFixes:
  + Make configuration environment variables case-insensitive on Windows.
+ Perf:
  + Use interlockedexchange instead of critical sections around rawprofiler handle to address bottleneck objectalloc callbacks.

## 1.0.35
+ New:
  + Allow unit tests to test single-return instrumentation methods and created unit tests for this.
+ BugFixes:
  + Fix GetSignatureInfoFromCallToken to use parent token if current token is methodSpec.
  + Fix static code analysis builds.
  + Address static analysis errors.
  + Add HResult, PID, and method name in logging for InstrumentationMethod.
  + Normalize file line endings
+ Perf:
  + Fix stack overflow when destructing graphs.
  + Reduce overhead of retargeting branches
  + Optimize addref/release.
  + Defer instruction offset calculation.
+ Updates:
  + Reenable ref counting in debug
  + Change log for missing method from error to message level
  + Update PR/CI stage dependencies
  + Use NuGet 5.8 when restoring
  + Use IfFailLog when rawprofiler fails to initialize.

## 1.0.34
+ BugFixes:
  + Fix GetAssemblyInfosByName to return out param

## 1.0.33
+ BugFixes:
  + Fix GetILFunctionBody call for RawProfilerHook during OnModuleLoadFinished
  + Fix building with MSVC v14.28 by moving .h headers from ClCompile to ClInclude
  + Fix RawProfilerHook QI to match CLR behavior (cast interfaces downward)

# 1.0.32
+ New:
  + Support loading and initializing Instrumentation Methods on profiler attach.
+ Updates:
  + Deprecate IProfilerManager::GetProfilerHost(IProfilerManagerHost**) method.
+ BugFixes:
  + Fix RPH bug - Allow SetEventMask post initialization if not affecting immutable flags
  + Fix RPH bug - SetRejitMethodInfo with nullptr did not erase MethodInfo, causing AV
  + Fix RPH bug - GetILFunctionBody allows NULL for out params if more than one
  + Fix incorrect naming of nested types

# 1.0.31
+ BugFixes:
  + Avoid deadlock within ProxyLogging when terminating logging.
  + Fixed CLRIE to correctly emit ldloca instead of ldloc.

# 1.0.30
+ New:
  + Add configuration for InstrumentationMethod loglevels
+ Updates:
  + Added documentation around the logging infrastructure
+ BugFixes:
  + Fix codesigning ProfilerProxy

# 1.0.29
+ BugFixes:
  + Fix runtime error for Alpine

# 1.0.28
+ New:
  + Add support for profiling all CLRs within the same process
  + Add ProfilerManagerWrapper to host Instrumentation Methods
  + Add local .ps1 testing script
+ BugFixes:
  + Fix index & publish symbols (Windows)
+ Updates:
  + Add bitness to RawProfilerHook environment variables
  + Update CLRIE to build with VS 2019

# 1.0.27
+ New:
  + Added support for ICorProfilerInfo10

# 1.0.26
Same functionality as 1.0.25.

# 1.0.25
This version did not get published due to issues encountered in App Service. Mitigation was taken by repackaging the previously released version (1.0.21) as 1.0.25.
+ Bugfixes:
  + Build header nupkg for AnyCPU
  + Fix unicode character in bldver.rc
  + Remove nobuild property for package projects to unblock dotnet core 3
  + Handle null methodNames from dll obfuscation
+ Updates:
  + Change EULA to MIT license for OSS

# 1.0.24
+ BugFixes:
  + Avoid deadlock within CEventLoggingBase when terminating logging.

# 1.0.23
+ New:
  + Added instrumentation method registration query method via new IProfilerManager5 API.

# 1.0.22
+ New:
  + Added generating MSI and MSM packages
  + Added yaml builds
  + Added ProfilerProxy
  + Added export for global logger with no association with any IProfilerManager instances.
  + Added global logger support to IProfilerManager via new IProfilerManager4 API.
+ BugFixes:
  + Revert License.md to LICENSE and fix NuGet pack. This also fixes doc links.
+ Updates:
  + Removed ExtensionsHost variables in favor of default host.
  + Use public MicroBuild package

# 1.0.21
+ BugFixes:
  + Fixed xdt files to not contain '--' (via %XDT_SITENAME% & %XDT_SCMSITENAME%)

# 1.0.20
+ New:
  + Added support for ICorProfilerInfo8 and ICorProfilerInfo9
  + Added IMethodJitInfo2
+ BugFixes:
  + Work-around for issue in the CLR runtime which causes il/instrumentation maps and il/native maps to be cached incorrectly.
  + Added NuGet signing to headers and engine package

# 1.0.19
+ New:
  + Added IModuleInfo3 interface
  + Changed semantics of GetIsDynamic to match the CLR understanding of dynamic code. I.e. a module is dynamic if and only if its base address is 0.

# 1.0.18
+ Update
  + Fixed bug that causes an AV when jit compiling dynamic methods.

# 1.0.17
+ New
  + Added API version levels to IProfilerManager via new IProfilerManager3 api
  + Added IMethodJitInfo api
  + Added IInstrumentationMethodJitEvents2 api
+ Update
  + Added more dump logs around OriginalIL, Pid, and Module/Method names during Instrumentation

# 1.0.16

+ Updates:
    + Always create new methodinfos (and log existing)

# 1.0.15

+ New:
    + Migrated repo from the internal Microsoft DevDiv account in Azure DevOps.
