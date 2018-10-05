# Contributing to the CLR Instrumentation Engine

This project welcomes contributions and suggestions. Most contributions require you to
agree to a Contributor License Agreement (CLA) declaring that you have the right to,
and actually do, grant us the rights to use your contribution. For details, visit
https://cla.microsoft.com.

When you submit a pull request, a CLA-bot will automatically determine whether you need
to provide a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the
instructions provided by the bot. You will only need to do this once across all repositories using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/)
or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Pull Request Process

1. Ensure the README.md and any documentation are up-to-date with details of changes made. This will help maintain the accuracy of the documentation to the current state of the code.
2. Increment the semantic version number and date in the [Version.props](build/version.props) file. The CLR Instrumentation Engine follows the [semantic versioning](https://semver.org/) scheme.
3. Once at least two approvals are given for the PR, you may merge your changes.

## Branches

Active development is happening in `develop` branch.

Last released version is in `master` branch.

## Submitting PR:

- Build solution in both `Release` and `Debug` configurations (see *Building locally* section)
- Create a PR against the `develop` branch and add clrieowners@microsoft.com as a reviewer

## Instrumentation Engine Api Changes

Apis are contributed via midl, which is not available for Linux, so all midl builds must be executed on Windows only. In order to make changes to InstrumentationEngine Api, do the following:

- Make edits to the .idl file in InstrumentationEngine.Api
- Build the InstrumentationEngine.Api project manually for either x86 or x64. The output is the same either way.
- Stage and commit changes to InstrumentationEngine.idl, InstrumentationEngine.h, and InstrumentationEngine_api.cpp
- Ensure that native projects build correctly in x86 and x64
- Submit a PR as normal.