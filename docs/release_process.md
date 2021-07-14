# Releasing the CLR Instrumentation Engine

Release occurs from the `release` branch. Please notify clrieowners@microsoft.com to initiate the release process.

## Microsoft Internal Process

### Develop Phase
1.  When changes are merged to `main` branch, the
[Main-CI](https://devdiv.visualstudio.com/DevDiv/_build/index?context=allDefinitions&path=%5CClrInstrumentationEngine%5CGitHub&definitionId=10093&_a=completed)
build runs and creates artifacts (NuGet, zip, etc.)
2.  Once the build completes, the [Main-CI Release](https://devdiv.visualstudio.com/DevDiv/_releases2?view=all&definitionId=1116) pipeline
automatically publishes artifacts to the internal NuGet feed with a preview version (eg. 1.0.15-build00001)
3.  When a release occurs, a PR to version bump the Instrumentation Engine as well as updating the [CHANGELOG](../CHANGELOG.md) should occur.

### Test Phase
Based on the changes and targeted platform releases, impacted scenarios and partner teams should be involved in testing for regressions.

### Release Phase
1.  Once testing completes, PR to merge `main` branch to `release` branch
2.  Manually run the [Signed](https://devdiv.visualstudio.com/DevDiv/_build?definitionId=11311) build to create release artifacts
3.  Manually run the
[CLR Instrumentation Engine Release](https://devdiv.visualstudio.com/DevDiv/_releases2?view=all&definitionId=901)
pipeline which publishes artifacts with release version (eg. 1.0.15)
    +  Publish NuGets to the internal NuGet feed and MSAzure (for Azure VM/VMSS WAD)
    +  Publish Preinstalled CLR Instrumentation Engine zip file to Azure App Service
    +  Publish msi/msm files to CDN and expose aka.ms links on this repo.