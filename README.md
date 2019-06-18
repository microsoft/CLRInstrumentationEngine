# CLR Instrumentation Engine

Develop Branch: [![Build Status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/GitHub/ClrInstrumentationEngine-CI-Yaml?branchName=develop)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=11310&branchName=develop)

Master Branch: [![Build Status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/GitHub/ClrInstrumentationEngine-Signed-Yaml?branchName=master)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=11311&branchName=master)

## Overview

The CLR Instrumentation Engine (CLRIE) is a cooperation profiler that allows running multiple profiling extensions in the same process. It was built to satisfy Application Insights, IntelliTrace, and Production Breakpoints scenarios.

The CLRIE's goal is to create a cooperative environment for different profilers to work together on various platforms. In order to further this goal, we plan to open source this repo to the community by the end June 2019.

For more information about our current and future project scope and to track our progress, please see the [CLRIE Road Map](roadmap.md).

## Getting Started

The CLR Instrumentation Engine is a profiler implementation and is enabled and configured via environment variables for the running process.

* [Getting Started](docs/getting_started.md) for details on how use the CLR Instrumentation Engine.
* [Environment Variables](docs/environment_variables.md)
* [Configure Instrumentation Methods](docs/configuration.md)

## Contributing

Please read [Contributing](CONTRIBUTING.md) for details on the Contributor License Agreement (CLA) and the process for submitting pull requests to us.

This repo builds using Visual Studio 2017 and requires the following components:

|Component Id|Component Friendly Name|
|:--|:--
Microsoft.Component.MSBuild|MSBuild
Microsoft.VisualStudio.Workload.NativeDesktop|Desktop development with C++ (Workload)
Microsoft.VisualStudio.Component.VC.ATL.Spectre|Visual C++ ATL (x86/x64) with Spectre Mitigations
Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre|VC++ 2017 version 15.9 v14.16 libs for spectre (x86 and x64)

Optionally, in order to develop Wixproj files in Visual Studio, you will need to install the [Wix Toolset Visual 2017 Extension](https://marketplace.visualstudio.com/items?itemName=RobMensching.WixToolsetVisualStudio2017Extension), also known as the "Votive" extension.

* [Design Notes](DESIGN-NOTES.md) - the overall design for CLR Instrumentation Engine.
* [Build](docs/build.md) - how to run local builds.
* [Test](docs/test.md) - how to run tests.
* [Release Process](docs/release_process.md) - how to release CLRIE to various platforms.

## Versioning

The CLR Instrumentation Engine follows the [Semantic Versioning](https://semver.org/) scheme.


## Encountering and reporting issues

See [Troubleshooting](docs/troubleshooting.md) for common pitfalls and remediation.

If you're still encountering a non-critical issues, please contact clrieowners@microsoft.com.

Critical security issues and bugs should be reported privately, via email, to the Microsoft Security
Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). You should
receive a response within 24 hours. If for some reason you do not, please follow up via
email to ensure we received your original message. Further information, including the
[MSRC PGP](https://technet.microsoft.com/security/dn606155) key, can be found in
the [Security TechCenter](https://technet.microsoft.com/security/default).

## License

Usage governance of the Instrumentation Engine is detailed in the [End User License Agreement](Instrumentation%20Engine%20SDK%20EULA.rtf) and [License](LICENSE).

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact opencode@microsoft.com with any additional questions or comments.
