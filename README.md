# CLR Instrumentation Engine

Develop Branch: [![Build status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/ClrInstrumentationEngine-develop-CI-QE)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=8828)

Master Branch: [![Build status](https://devdiv.visualstudio.com/DevDiv/_apis/build/status/ClrInstrumentationEngine/ClrInstrumentationEngine-master-signed)](https://devdiv.visualstudio.com/DevDiv/_build/latest?definitionId=4192)

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

## Building

Please refer to [Build](docs/build.md) for details on how to run local builds.

## Running Tests

Please refer to [Test](docs/test.md) for details on how to run tests.

## Contributing

Please read [Contributing](CONTRIBUTING.md) for details on the Contributor License Agreement (CLA) and the process for submitting pull requests to us.

## Versioning

The CLR Instrumentation Engine follows the [Semantic Versioning](https://semver.org/) scheme.

## Release

Please read [Release](docs/release.md) for details on how we release new changes.

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

See [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/) for full details.

### Our Pledge

In the interest of fostering an open and welcoming environment, we as
contributors and maintainers pledge to making participation in our project and
our community a harassment-free experience for everyone, regardless of age, body
size, disability, ethnicity, gender identity and expression, level of experience,
nationality, personal appearance, race, religion, or sexual identity and
orientation.

### Our Standards

Examples of behavior that contributes to creating a positive environment
include:

* Using welcoming and inclusive language
* Being respectful of differing viewpoints and experiences
* Gracefully accepting constructive criticism
* Focusing on what is best for the community
* Showing empathy towards other community members

Examples of unacceptable behavior by participants include:

* The use of sexualized language or imagery and unwelcome sexual attention or
advances
* Trolling, insulting/derogatory comments, and personal or political attacks
* Public or private harassment
* Publishing others' private information, such as a physical or electronic
  address, without explicit permission
* Other conduct which could reasonably be considered inappropriate in a
  professional setting

### Our Responsibilities

Project maintainers are responsible for clarifying the standards of acceptable
behavior and are expected to take appropriate and fair corrective action in
response to any instances of unacceptable behavior.

Project maintainers have the right and responsibility to remove, edit, or
reject comments, commits, code, wiki edits, issues, and other contributions
that are not aligned to this Code of Conduct, or to ban temporarily or
permanently any contributor for other behaviors that they deem inappropriate,
threatening, offensive, or harmful.

### Scope

This Code of Conduct applies both within project spaces and in public spaces
when an individual is representing the project or its community. Examples of
representing a project or community include using an official project e-mail
address, posting via an official social media account, or acting as an appointed
representative at an online or offline event. Representation of a project may be
further defined and clarified by project maintainers.

### Enforcement

Instances of abusive, harassing, or otherwise unacceptable behavior may be
reported by contacting the project team at clrieowners@microsoft.com. All
complaints will be reviewed and investigated and will result in a response that
is deemed necessary and appropriate to the circumstances. The project team is
obligated to maintain confidentiality with regard to the reporter of an incident.
Further details of specific enforcement policies may be posted separately.

Project maintainers who do not follow or enforce the Code of Conduct in good
faith may face temporary or permanent repercussions as determined by other
members of the project's leadership.
