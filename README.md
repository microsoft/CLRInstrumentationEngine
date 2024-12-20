# CLR Instrumentation Engine

Main Branch: [![Build Status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/ClrInstrumentationEngine-CI-Yaml?branchName=main)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=11310&branchName=main)

Release Branch: [![Build Status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/GitHub/ClrInstrumentationEngine-Signed-Yaml?branchName=release)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=11311&branchName=release)

## Overview

The CLR Instrumentation Engine (CLRIE) is a cooperation profiler that allows running multiple profiling extensions in the same process. It was built to satisfy Application Insights, IntelliTrace, and Production Breakpoints scenarios.

The CLRIE's goal is to create a cooperative environment for different profilers to work together on various platforms. In order to further this goal, we plan to open source this repo to the community by the end June 2019.

For more information about our current and future project scope and to track our progress, please see the [CLRIE Road Map](ROADMAP.md).

## Getting Started

The CLR Instrumentation Engine is a profiler implementation and is enabled and configured via environment variables for the running process.

-   [Getting Started](docs/getting_started.md) for details on how use the CLR Instrumentation Engine.
-   [Environment Variables](docs/environment_variables.md)
-   [Configure Instrumentation Methods](docs/configuration.md)
-   [CLRIE Releases](docs/releases.md)

## Contributing

Please read [Contributing](CONTRIBUTING.md) for details on the Contributor License Agreement (CLA) and the process for submitting pull requests to us.

This repo builds using Visual Studio 2022 and requires the following components:

| Component Id                                                 | Component Friendly Name                                                  |
| :----------------------------------------------------------- | :----------------------------------------------------------------------- |
| Microsoft.Component.MSBuild                                  | MSBuild                                                                  |
| Microsoft.VisualStudio.Workload.NativeDesktop                | Desktop development with C++ (Workload)                                  |
| Microsoft.VisualStudio.Component.VC.ATL.Spectre              | C++ ATL for latest v143 build tools with Spectre Mitigations (x86 & x64) |
| Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre | MSVC v143 - VS 2022 C++ x64/x86 Spectre-mitigated libs (Latest)          |

Optionally, in order to develop Wixproj files in Visual Studio, you will need to install the [Wix Toolset Visual 2022 Extension](https://marketplace.visualstudio.com/items?itemName=WixToolset.WixToolsetVisualStudio2022Extension), also known as the "Votive" extension.

-   [Design Notes](DESIGN-NOTES.md) - the overall design for CLR Instrumentation Engine.
-   [Build](docs/build.md) - how to run local builds.
-   [Test](docs/test.md) - how to run tests.
-   [Release Process](docs/release_process.md) - how to release CLRIE to various platforms.

## Versioning

The CLR Instrumentation Engine follows the [Semantic Versioning](https://semver.org/) scheme.

## Encountering and reporting issues

See [Troubleshooting](docs/troubleshooting.md) for common pitfalls and remediation.

If you're still encountering a non-critical issues, please contact clrieowners@microsoft.com.

Critical security issues and bugs should be reported privately, via email, to the Microsoft Security Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). See [Security](SECURITY.md) for more details.

## License

Usage governance of the Instrumentation Engine is detailed in the [License](LICENSE).

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information, see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact opencode@microsoft.com with any additional questions or comments.

## Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft trademarks or logos is subject to and must follow [Microsoft’s Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general). Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship. Any use of third-party trademarks or logos are subject to those third-party’s policies.
