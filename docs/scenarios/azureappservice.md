# Azure App Service

Azure App Service is a PaaS offering in Azure and provides hosting of web applications and sites as well as a Site Control Manager (SCM, also known as [Kudu](https://github.com/projectkudu/kudu)) that provides administrative and management capabilities.

In addition, there exists a notion of [Site Extensions](https://github.com/projectkudu/kudu/wiki/Azure-Site-Extensions) which provide powerful customizable behaviors for the web app. This customization is done through a mechanism in IIS called [xdt transformations](https://msdn.microsoft.com/en-us/library/dd465326.aspx).

## CLR Instrumentation Engine in an App Service

The CLR Instrumentation Engine is currently shipped as part of the [Application Insights](https://www.nuget.org/packages/Microsoft.ApplicationInsights.AzureWebSites/) which is a Private Site Extension as well as in the form of a Preinstalled Site Extension on every Azure App Service instance.

A private site extension is a nuget package that can be manually installed by the site owner from the site extension gallery in Kudu into the %HOME%\SiteExtensions folder. Once installed, the binaries and files of the private site extension can be manipulated and modified easily.

A preinstalled site extension is a folder that is shipped by the Kudu team into the %ProgramFiles(x86)%\SiteExtensions folder on every Azure App Service machine and is enabled through the Azure App Service Application Settings. These files are restricted to read-only access and cannot be modified.

The CLR Instrumentation Engine [Environment Variables](../environment_variables.md) are enabled through an applicationHost.xdt file. When a private site extension is installed or a preinstalled site extension is enabled, the applicationHost.xdt file in that site extension is consumed by the web app process during startup. Some site extensions also provide an scmApplicationHost.xdt file which is consumed by the Kudu process.

## Enabling the CLR Instrumentation Engine

### As a Preinstalled Site Extension

In order to enable the CLR Instrumentation Engine preinstalled site extension, go to your Azure App Service resource in the Azure Portal and add the below setting to the Application Settings:

|Key|Value|
|-|-|
`INSTRUMENTATIONENGINE_EXTENSION_VERSION`|`~1`

This will enable the latest version 1.x of the CLR Instrumentation Engine preinstalled site extension. Please see [Kudu Site Extensions](https://github.com/projectkudu/kudu/wiki/Azure-Site-Extensions) for details on how the version values work.

Please note that applicationHost.xdt files for enabled preinstalled site extension are processed before any private site extension.

### As a Private Site Extension

The CLR Instrumentation Engine is not shipped as a standalone private site extension. Instead it is available as part of the Application Insights private site extension and will be enabled as part of the Application Insights applicationHost.xdt file.

### Configuring your Instrumentation Method

In order for the Instrumentation Engine to pick up your instrumentation method, you will need to declare environment variables that point to a *.config file which references your instrumentation method assembly.

Please see [Configuration](../configuration.md) for details about the config file and environment variables.

## Patching or debugging the CLR Instrumentation Engine

Please see [Build](../build.md) to build the CLR Instrumentation Engine binaries locally.

If you would like to test your changes in your Azure App Service, it is recommended that you either install the Application Insights private site extension and patch or create a custom private site extension with an applicationHost.xdt file that enables the required environment variables. You will not be able to patch the preinstalled site extension.

Ensure that the CLR Instrumentation Engine preinstalled site extension is not enabled when working with private site extensions.