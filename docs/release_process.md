# Releasing the CLR Instrumentation Engine

Release occurs from the Master branch. Please notify clrieowners@microsoft.com to initiate the release process.

## Internal Process

### Develop Phase
1.	When changes are merged to develop branch, the [Develop-CI](https://devdiv.visualstudio.com/DevDiv/_build/index?context=allDefinitions&path=%5CClrInstrumentationEngine%5CGitHub&definitionId=10093&_a=completed) build runs and creates artifacts (NuGet, zip, etc.)
2.	Once the build completes, the [Develop-CI Release](https://devdiv.visualstudio.com/DevDiv/_releases2?view=all&definitionId=1116) pipeline automatically publishes artifacts to the internal NuGet feed with a preview version (eg. 1.0.15-build00001)
3.	When a release occurs, a PR to version bump the Instrumentation Engine as well as updating the [CHANGELOG](../CHANGELOG.md) should occur.

### Test Phase
Based on the changes and targeted platform releases, impacted scenarios and partner teams should be involved in testing for regressions.

### Release Phase
1.	Once testing completes, PR to merge develop branch to master branch
2.	Manually run the [Master-Signed](https://devdiv.visualstudio.com/DevDiv/_build/index?context=allDefinitions&path=%5CClrInstrumentationEngine%5CGitHub&definitionId=10055&_a=completed) build to create release artifacts
3.	Manually run the [CLR Instrumentation Engine Release](https://devdiv.visualstudio.com/DevDiv/_releases2?view=all&definitionId=901) pipeline which publishes artifacts with release version (eg. 1.0.15)
	+	Publish NuGets to the internal NuGet feed and MSAzure (for Azure VM/VMSS WAD)
	+	Publish Preinstalled CLR Instrumentation Engine zip file to Azure App Service