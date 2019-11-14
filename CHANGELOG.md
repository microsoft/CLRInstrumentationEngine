# Change Log

Please format the changes as follows:

## Version Number
+ New:
+ BugFixes:
+ Updates:

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
