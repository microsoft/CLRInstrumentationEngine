# Change Log

Please format the changes as follows:

## Version Number
+ New:
+ BugFixes:
+ Updates:

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