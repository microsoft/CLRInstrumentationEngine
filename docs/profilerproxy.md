# CLRIE Profiler Design Doc

1. [Overview](#overview)
2. [Goals](#goals)
3. [Benefits](#benefits)
4. [Background](#background)
5. [Architecture and Design](#design)

## <a name="overview>Overview</a>
The CLR Instrumentation Engine (CLRIE) MSI/MSM installs the dlls into a versioned folder under Program Files. Since COR_PROFILER environment variables require the dll’s full path, the version must be known up front. Conflicts can occur when different products deploy different versions of the CLRIE.

## <a name="goals">Goals</a>
- Profilers can cooperatively run on the latest version of CLRIE.
- Security updates are provided with newer CLRIE versions and automatically used on next process restart.
- Easy cleanup due to filelocking proxy and CLRIE being used
- Proxy can be updated separately from CLRIE
- Design should work with Azure VM/VMSS (WAD extension) and Azure App Service since SnapshotDebugger requires supporting these platforms

## <a name="benefits">Proxy Benefits</a>
From a profiler author’s standpoint, having a proxy pointing to the latest version removes the burden of determining whether CLRIE is already installed and which version of CLRIE is the latest. Profiler authors can ship with a compatible CLRIE version, point their environment variables to the CLRIE proxy, and run with the guarantee that the version of CLRIE being used is at least the version they ship with (an invariant of CLRIE is that it’s backwards compatible).

Since the proxy points to the latest version, security updates and bugfixes are easily deployed out-of-band by installing newer MSIs. When the process eventually restarts, the proxy logic will automatically pick up a newer CLRIE if it exists.

From a CLRIE developer’s perspective, the proxy simplifies maintainability by mirroring the behavior used by the App Service preinstalled site extension where configuring the application settings using [tilda semantics](https://github.com/projectkudu/kudu/wiki/Azure-Site-Extensions) ~X automatically enables the latest major version X of the site extension (eg. ~1 ensures the latest version 1.x.x is used). The proxy will not be used by the App Service preinstalled site extension since it points to the latest CLRIE version already.

## <a name="background">Background</a>
### CLRIE Installation Design
The CLRIE builds an x86 and x64 platform MSI and MSM. The x64 installers contain both x86 and x64 bits while the x86 installers contain only x86 bits.

The installers do not modify registry keys nor environment variables; they are designed to simply drop files into a versioned folder. The x64 and x86 binaries are located in %ProgramFiles% (64bit) and %ProgramFiles(x86)% (32bit) as described below:

#### x64 MSI on 64bit Windows
Contains both x86 and x64 dlls
```
%ProgramFiles%
    Microsoft CLR Instrumentation Engine
        <CLRIE Version>
            Instrumentation64
                <x64 dlls>

%ProgramFiles(x86)%
    Microsoft CLR Instrumentation Engine
        <CLRIE Version>
            Instrumentation32
                <x86 dlls>
```

#### x86 MSI on 64bit Windows
Contains only x86 dlls in %ProgramFiles(x86)%
```
%ProgramFiles(x86)%
    Microsoft CLR Instrumentation Engine
        <CLRIE Version>
            Instrumentation32
                <x86 dlls>
```

#### x86 MSI on 32bit Windows
Contains only x86 dlls but in **%ProgramFiles%** since %ProgramFiles(x86)% doesn't exist.
```
%ProgramFiles%
    Microsoft CLR Instrumentation Engine
        <CLRIE Version>
            Instrumentation64
                <x86 dlls>
```

This allows side-by-side versions of the CLRIE to coexist and respects the installer [component rules](http://robmensching.com/blog/posts/2003/10/18/component-rules-101/) between the CLRIE MSI and external profiler MSIs that consume CLRIE MSM modules.

#### CLRIE Versions
CLRIE is built with both Debug and Release configuration as well as supports the notion of preview and public-release versions.
The build context modifies the version as follows:
- The base version is defined with three numbers separated by periods (eg. `1.0.0`).
- Preview builds append `–build#####` to the base version; public-release builds do not.
- Debug builds append `_debug` to the version; Release builds do not.

Different configurations (debug and release) still represent the same functional code and have “equal” versions. However, preview builds only contain a subset of features or functionality that will be available in the public-release (ie. public release builds are “newer” than preview builds with the same version). This becomes relevant in version comparisons.

To summarize, there are 4 variations of the CLRIE version:

|| Release | Debug |
|-|-|-|
|**Public Release**|1.0.0|1.0.0_debug|
|**Preview**|1.0.0-build1|1.0.0-build1_debug|

#### Version Comparisons
Preview versions are considered previews of the base version they’re associated with. This means 1.0.0-build1 is a preview of 1.0.0. By design, preview versions are “older” than the public-release version. Here is an example of version comparison:

`1.0.0-build1 < 1.0.0-build2 < 1.0.0 < 1.0.1-build1 < 1.0.1`

Since debug and release configurations of a specific version represent the same code and share the same functionality, they are considered separate but equal versions. The default proxy behavior is to ignore debug versions but can be configured by the environment variable to only use debug versions.

`InstrumentationEngineProxy_UseDebug = 1`

If no debug builds are available when the **UseDebug** flag is set, or if no release builds are available and UseDebug flag is not set, the proxy will return the HRESULT 0x80070002 (ERROR_FILE_NOT_FOUND errno).

`InstrumentationEngineProxy_UsePreview = 1`

Users can also set **UsePreview**. Like **UseDebug** except includes preview versions. This is an optional flag for test environments to pick up preview versions of CLRIE. Production would by default not use preview bits.

`InstrumentationEngineProxy_UseSpecificVersion = 1.0.0-build00001_Debug`

Another flag to configure would be **UseSpecificVersion** to allow the proxy to pick a single version available to it. If this version doesn’t exist, the proxy will error out with ERROR_FILE_NOT_FOUND. The goal of this flag would be for testing and flexibility, although it goes against a cooperative design. The better workaround to this is to just forward COR_PROFILER to the specific CLRIE version.

## <a name="design">Proxy Architecture and Design</a>
The proxy itself should be supported on all OS platforms, however its implementation is platform dependent due to its dependency on a “common folder” location. For the first iteration and scope of this design document, only Windows OS is discussed.

The folder structure of the proxy is described below:
```
%ProgramFiles%|%ProgramFiles(x86)%
    Microsoft CLR Instrumentation Engine
        <CLRIE Version>
            Instrumentation64|32
                <dlls>
        Proxy
            <Proxy Version>
                proxy_x86|x64.dll
```

The profiler proxy will be implemented as a standalone dll and hosts an IClassFactory implementation that will forward the DllGetClassObject call to the latest version of CLRIE as previously discussed. There will be an x86 and x64 bit proxy, and the proxy will search in the folder structure corresponding to its own bitness.

### The Proxy Version
The proxy is meant to be a simple and shallow implementation that determines the latest CLRIE version. This means updates to the proxy should be infrequent and small. In addition, we want to mirror the behavior of Azure App Service preinstalled site extension. The proposed proxy versioning scheme is simple:
- Version 1 proxy will point to the latest major version 1 of CLRIE. This means all major version 1 CLRIE will carry the version 1 proxy.
- The proxy.dll file version can be changed to ensure [MSI file versioning rules](https://docs.microsoft.com/en-us/windows/desktop/Msi/file-versioning-rules) allow patching proxies from newer builds.

### Using the Proxy
The COR_PROFILER environment variables should point to the proxy dll rather than the CLRIE dll.

`COR_PROFILER_PATH_64 = "%ProgramFiles%\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x64.dll"`
`COR_PROFILER_PATH_32 = "%ProgramFiles(x86)%\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x86.dll"`

The `v1` folder indicates this proxy will point to the latest major version 1 of CLRIE.

This enables the proxy to conduct CLRIE versioning checks. However, the CLRIE dll should still be its own profiler and can also be pointed to by COR_PROFILER for testing purposes. This also maintains the current behavior of the App Service preinstalled site extension where specific versions can be configured rather than “latest”.

### Redistributing the Proxy

The point of the proxy is to look for CLRIE in a common folder location and load the latest. This design works well for redistributions where CLRIE can be installed to the common folder (ie. deploy the proxy along with MSI/MSM).

It is possible to redistribute the proxy alongside the user's application as long as CLRIE is installed in the common folder described below. However by moving the proxy from the common location folder, it is effectively removed from the in-place upgrade/patch scenarios provided by MSI/MSM and so is discouraged unless there is a guarantee that the application is standalone. Please post a GitHub issue or reach out to clrieowners@microsoft.com if you require a scenario to be supported.

For Azure-related scenarios, please see [CLRIE in Azure](scenarios/azure.md).

### Considerations
Since the CLRIE proxy only updates to new versions on process restart, multi-instance environments may run the risk of having conflicting CLRIE versions running at the same time. This may not be an issue since new profilers would update the environment variables and restart the process to get picked up.

Linux could use /urs/lib/ as its “common location” folder like %ProgramFiles% on Windows. For Snapshot Debugger AKS scenarios (on Linux docker containers), CLRIE is in /diag/Instrumentation64 and is currently not versioned.