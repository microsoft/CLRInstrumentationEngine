# Testing the CLR Instrumentation Engine.

## Testing locally

### Windows

The [WindowsLocalTest.ps1](../src/Scripts/WindowsLocalTest.ps1) script requires installing the ClrInstrumentationEngine msi or msm and launches a user-specified application (or another PowerShell process) with the COR_PROFILER environment variables set. The script takes switch parameters to configure several environment variables (like DebugWait) as well as one InstrumentationMethod config path and RawProfilerHook Guid and path.

## Running unit tests locally

CoreCLR tests and `TestAppRunner` target only .NET 10 and .NET 11. Desktop .NET Framework
coverage is retained, including the `net462` instrumentation tests. Install the .NET 11 SDK
to build all test targets, using a prerelease SDK while .NET 11 is in preview.
On Windows, install both the x64 and x86 runtimes for .NET 10 and .NET 11 because the
tests launch applications of both bitnesses. Test binaries are written to the `net10.0`
and `net11.0` output folders, respectively.
On Linux, use a supported distribution such as Ubuntu 22.04 or later.

`InstrEngineTests` runs from portable build artifacts and does not generate a
`runtimeconfig.dev.json` with build-machine NuGet probing settings. Exception stack
baselines exclude the test runner and the .NET 11 `System.Environment.CallEntryPoint`
bootstrap frame; application frames and exception callback ordering remain checked.

### Windows

There are two ways to run the CLR Instrumentation Engine unit tests - Visual Studio Test Explorer and `build.ps1` script.

#### Via Visual Studio Test Explorer

1. Open InstrumentationEngine.sln
2. Set the build configuration (`Debug` or `Release`) and architecture (`x86`, `x64`, `AnyCPU`) and build locally.
3. Go to `Test > Test Settings > Select Test Settings File > [CLRInstrumentationEngine repo]\tests\TestSettings\`.
    - Select the `x86.runsettings` for `x86` and `AnyCPU` architecture
    - Select the `x64.runsettings` for `x64` architecture.
4. Click `Run All` in the Test Explorer.


#### Via Script

Run the script `[CLRInstrumentationEngine repo]\build.ps1` with IncludeTests. This will invoke the unit tests for x86, x64, and AnyCPU against
either `Debug` or `Release` configuration post-build.
|Flag|Description|
|-|-|
IncludeTests|Runs unit tests after build.
SkipBuild|Skips building the solutions. Use this with `-IncludeTests` flag to only run tests.
Release|Cause build to run with `Release` configuration. By default, build uses `Debug` configuration.
Verbose|Sets msbuild verbosity to `normal`. By default, verbosity is set to `ErrorsOnly`.

### Linux

#### Prerequisites

The following technologies/tools are required in order to run Linux tests on Windows.

* WSL (Windows Subsystem for Linux) 2.0 with Ubuntu 22.04 LTS or later, the .NET 11 SDK, and the .NET 10 and .NET 11 runtimes
  - WSL is available on Windows 10 Build 19041+ and Windows 11.
  - Run this in a command prompt `wsl --install -d Ubuntu-22.04` or download Ubuntu 22.04 from the Microsoft Store. Restart as needed and setup a user/pass for your account.
* Docker Desktop for Linux (Ubuntu)
  - Follow the instructions from the official docs https://docs.docker.com/engine/install/ubuntu/

#### Steps

1. On Windows, build InstrumentationEngine for Debug AnyCPU (which builds managed projects and dlls).
2. Run `src\scripts\DockerLocalBuild.ps1 -Wsl -BuildDockerImage -Interactive -RebuildImage`. This will build a docker container and drop you in an interactive session. By default builds for Ubuntu/Debian; set `-CLib musl` if you want to build for Alpine Linux instead.
3. Run `./src/build.sh` within the interactive session. This builds the native shared object files (.so).
4. Exit the container and, from the repository directory in WSL, use the .NET 11 SDK to run both `dotnet test bin/Debug/AnyCPU/net10.0/InstrEngineTests.dll` and `dotnet test bin/Debug/AnyCPU/net11.0/InstrEngineTests.dll`.
   The native build containers retain their older distribution baseline and are not the CoreCLR test execution environment.

Please contact clrieowners@microsoft.com for any details or questions.