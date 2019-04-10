# CLR Instrumentation Engine

Develop Branch: [![Build Status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/GitHub/%5BGitHub%5D%20ClrInstrumentationEngine-develop-CI-QE)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=10093)

Master Branch: [![Build Status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/GitHub/%5BGitHub%5D%20ClrInstrumentationEngine-master-signed)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=10055)

## Overview

The CLR Instrumentation Engine is a cooperation profiler that allows running multiple profiling extensions in the same process. It was initially built to satisfy Application Insights, IntelliTrace, and Production Breakpoints scenarios.

Currently the CLR Instrumentation Engine is installed by:

* Latest versions of Microsoft Monitoring Agent:
  - [64-bit windows](https://go.microsoft.com/fwlink/?LinkID=517476)
  - [32-bit windows](https://go.microsoft.com/fwlink/?LinkID=615592)
* Shipped as a [Preinstalled Site Extension](https://github.com/projectkudu/kudu/wiki/Azure-Site-Extensions) for [Azure App Services](docs/scenarios/azureappservice.md)
  - Enabled by [Application Insights](docs/scenarios/applicationinsights.md)
  - Enabled by [Visual Studio Snapshot Debugger](docs/scenarios/snapshotdebugger.md)
  - [Status Monitor](http://go.microsoft.com/fwlink/?linkid=506648)

## Getting Started

The CLR Instrumentation Engine is a profiler implementation and is enabled and configured via environment variables for the running process. Please see [Getting Started](docs/getting_started.md) for more details.

See the [Design Notes](DESIGN-NOTES.md) for in-depth details of the CLR Instrumentation Engine.

See [Build](docs/build.md) for details on how to run local builds.

See [Test](docs/test.md) for details on how to run tests.

See [Environment Variables](docs/environment_variables.md) for enabling the Engine.

See [Configuration](docs/configuration.md) for setting up your own Instrumentation Method.

## Contributing

Please read [Contributing](CONTRIBUTING.md) for details on the Contributor License Agreement (CLA) and the process for submitting pull requests to us.

## Developing

This repo builds using Visual Studio 2017 and requires the following components:
|Component Id|Component Friendly Name|
|:--|:--
Microsoft.Component.MSBuild|MSBuild
Microsoft.VisualStudio.Workload.NativeDesktop|Desktop development with C++ (Workload)
Microsoft.VisualStudio.Component.VC.ATL.Spectre|Visual C++ ATL (x86/x64) with Spectre Mitigations
Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre|VC++ 2017 version 15.9 v14.16 libs for spectre (x86 and x64)

Optionally, in order to develop Wixproj files in Visual Studio, you will need to install the [Wix Toolset Visual 2017 Extension](https://marketplace.visualstudio.com/items?itemName=RobMensching.WixToolsetVisualStudio2017Extension), also known as the "Votive" extension.

## Versioning

The CLR Instrumentation Engine follows the [Semantic Versioning](https://semver.org/) scheme.

## Release

Please read [Release](docs/release.md) for details on the release process.

## Encountering and reporting issues

See [Troubleshooting](docs/troubleshooting.md) for common pitfalls and remediation.

If you're still encountering a non-critical issues, please contact clrieowners@microsoft.com.

Critical security issues and bugs should be reported privately, via email, to the Microsoft Security
Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). You should
receive a response within 24 hours. If for some reason you do not, please follow up via
email to ensure we received your original message. Further information, including the
[MSRC PGP](https://technet.microsoft.com/en-us/security/dn606155) key, can be found in
the [Security TechCenter](https://technet.microsoft.com/en-us/security/default).

## License

Usage governance of the Instrumentation Engine is detailed in the [End User License Agreement](Instrumentation%20Engine%20SDK%20EULA.rtf) and [License](LICENSE).

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact opencode@microsoft.com with any additional questions or comments.
