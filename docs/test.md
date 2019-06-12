# Testing the CLR Instrumentation Engine.

## Running unit tests locally

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

Currently there are no unit tests support for Linux. However, Linux builds will run via PR validations. Please contact
clrieowners@microsoft.com for more details.