# Releasing the CLR Instrumentation Engine

Release occurs from the Master branch. Please notify clrieowners@microsoft.com to initiate the release process.

1. A signed build runs against the master branch that generates private nuget packages.
2. Once these nuget packages are tested to be functionally correct, a release definition runs against the signed build to publish artifacts to the necessary drops and endpoints.