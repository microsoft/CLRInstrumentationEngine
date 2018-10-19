# CLR Instrumentation Engine

Develop Branch: [![Build Status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/GitHub/%5BGitHub%5D%20ClrInstrumentationEngine-develop-CI-QE)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=10093)

Master Branch: [![Build Status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/GitHub/%5BGitHub%5D%20ClrInstrumentationEngine-master-signed)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=10055)

## Overview

The CLR Instrumentation Engine is a cooperation profiler that allows running multiple profiling extensions in the same process. It was built to satisfy Application Insights, IntelliTrace, and Production Breakpoints scenarios.

Currently the CLR Instrumentation Engine is installed by:

* Latest versions of Microsoft Monitoring Agent:
  - [64-bit windows](https://go.microsoft.com/fwlink/?LinkID=517476)
  - [32-bit windows](https://go.microsoft.com/fwlink/?LinkID=615592)
* [Application Insights](docs/scenarios/applicationinsights.md)
  - [Status Monitor](http://go.microsoft.com/fwlink/?linkid=506648&clcid=0x409)
  - Shipped in the [Application Insights Private Site Extension](https://www.nuget.org/packages/Microsoft.ApplicationInsights.AzureWebSites/) for [Azure App Services](docs/scenarios/azureappservice.md)
* [Snapshot Debugger](docs/scenarios/snapshotdebugger.md)
  - Shipped as a [Preinstalled Site Extension](https://github.com/projectkudu/kudu/wiki/Azure-Site-Extensions) for [Azure App Services](docs/scenarios/azureappservice.md)
* You can also use the following NuGet packages:
  - [Microsoft.ApplicationInsights.Agent_x64](http://www.nuget.org/packages/Microsoft.ApplicationInsights.Agent_x64)
  - [Microsoft.ApplicationInsights.Agent_x86](http://www.nuget.org/packages/Microsoft.ApplicationInsights.Agent_x86)

## Getting Started

The CLR Instrumentation Engine is a profiler implementation and is enabled and configured via environment variables for the running process. Please see [Getting Started](docs/getting_started.md) for more details.

See the [Design Notes](DESIGN-NOTES.md) for in-depth details of the CLR Instrumentation Engine.

## Building

Please refer to [Build](docs/build.md) for details on how to run local builds.

## Running Tests

Please refer to [Test](docs/test.md) for details on how to run tests.

## Contributing

Please read [Contributing](CONTRIBUTING.md) for details on the Contributor License Agreement (CLA) and the process for submitting pull requests to us.

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
