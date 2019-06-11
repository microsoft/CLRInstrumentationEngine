# CLR Instrumentation Engine in Azure

[Platforms](#platforms)
  1. [Azure App Service](#app-service)
  2. [Azure Virtual Machine and Virtual Machine Scale Sets](#vm-and-vmss)
  3. [Azure Kubernetes Service](#kubernetes)

[Products](#products)
  1. [Application Insights](#application-insights)
  2. [Visual Studio Snapshot Debugger](#snapshot-debugger)


# <a name="platforms">Platforms</a>

## <a name="app-service">Azure App Service</a>

Microsoft Docs: [App Service Overview](https://docs.microsoft.com/azure/app-service)

Azure App Service is a PaaS offering in Azure and provides hosting of web applications and sites as well as a
Site Control Manager (SCM, also known as [Kudu](https://github.com/projectkudu/kudu) that provides administrative
and management capabilities.

In addition, there exists a notion of [Site Extensions](https://github.com/projectkudu/kudu/wiki/Azure-Site-Extensions) which provide powerful
customizable behaviors for the web app. This customization is done through a mechanism in IIS called
[xdt transformations](https://msdn.microsoft.com/library/dd465326.aspx).

App Services are hosted by Azure VMs with both Windows and Linux flavors. Currently, only App Service on Windows supports Site Extensions.

### CLRIE on App Service

The CLR Instrumentation Engine is currently shipped in the form of a Preinstalled Site Extension on every Azure App Service instance.

A **private site extension** is a nuget package that can be manually installed by the site owner from the site extension gallery in Kudu into
the %HOME%\SiteExtensions folder. Once installed, the binaries and files of the private site extension can be manipulated and modified easily.

A **preinstalled site extension** is a folder that is shipped directly by the Kudu team into the %ProgramFiles(x86)%\SiteExtensions folder on
every Azure App Service machine and is enabled through the Azure App Service Application Settings. These files are restricted to read-only
access and cannot be modified.

When a private site extension is installed or a preinstalled site extension is enabled, the **applicationHost.xdt** file in that site extension
is consumed by the web app process during startup. This xdt file transforms the application's web.config like providing environment variables.
Some site extensions also provide an scmApplicationHost.xdt file which is consumed by and transforms the Kudu process.

Details for CLR Instrumentation Engine environment variables can be found at [Environment Variables](../environment_variables.md).

### Enabling the CLR Instrumentation Engine

#### As a Preinstalled Site Extension

In order to enable the CLR Instrumentation Engine preinstalled site extension, go to your Azure App Service resource in the Azure Portal and
add the below setting to the Application Settings (Currently there's a bug where different components set the key with different casings and
producing 409 conflict errors):

|Key|Value|
|-|-|
`InstrumentationEngine_EXTENSION_VERSION`|`~1`

This will enable the latest version 1.x of the CLR Instrumentation Engine preinstalled site extension.
Please see [Kudu Site Extensions](https://github.com/projectkudu/kudu/wiki/Azure-Site-Extensions) for details on how the version values work.

Please note that applicationHost.xdt files for enabled preinstalled site extension are processed before any private site extension.

#### As a Private Site Extension

The CLR Instrumentation Engine is not shipped as a standalone private site extension. Instead it is available as part of the Application
Insights private site extension and will be enabled as part of the Application Insights applicationHost.xdt file.

#### Configuring your Instrumentation Method

In order for the Instrumentation Engine to pick up your instrumentation method, you will need to declare environment variables that point to a
*.config file which references your instrumentation method assembly.

Please see [Configuration](../configuration.md) for details about the config file and environment variables.

### Patching or debugging the CLR Instrumentation Engine

Please see [Build](../build.md) to build the CLR Instrumentation Engine binaries locally.

If you would like to test your changes in your Azure App Service, it is recommended that you either install the Application Insights private
site extension and patch or create a custom private site extension with an applicationHost.xdt file that enables the required environment
variables. You will not be able to patch the preinstalled site extension.

Ensure that the CLR Instrumentation Engine preinstalled site extension is not enabled when working with private site extensions.

## <a name="vm-and-vmss">Azure Virtual Machine and Virtual Machine Scale Sets</a>

Microsoft Docs:
* [VM Overview](https://docs.microsoft.com/azure/virtual-machines)
* [VMSS Overview](https://docs.microsoft.com/azure/virtual-machine-scale-sets)

Azure Virtual Machine (VM) and Virtual Machine Scale Sets (VMSS) are IaaS offerings in Azure that provide users with a virtualized computer
running in Microsoft's Azure datacenter. The level of customization and control for VM/VMSS is much greater than App Service, meaning a wide
variety of applications and types of apps can be hosted.

Azure VM and VMSS support a notion of extensions, such as the
[Remote Debugging Extension](https://docs.microsoft.com/visualstudio/azure/vs-azure-tools-debug-cloud-services-virtual-machines#debugging-azure-virtual-machines)
and the
[Azure Diagnostics Extension](https://docs.microsoft.com/azure/azure-monitor/platform/diagnostics-extension-overview)

### CLRIE on VM/VMSS

Although Azure VM and VMSS host both Windows and Linux OS images, CLRIE distributions currently only target Windows images.

CLRIE is deployed in the Azure Diagnostics Extension (commonly referred to as the Windows Azure Diagnostics or "WAD" extension). The Visual
Studio Snapshot Debugger and Time Travel Debugging features leverage CLRIE by deploying and configuring WAD.

Since CLRIE is also packaged in an MSI/MSM, there are tasks to move WAD to install the MSI/MSM rather than shipping CLRIE dlls directly, which
will help further the cooperation model by allowing users to use Microsoft products and features alongside 3rd party profiling tools and
extensions.

## <a name="kubernetes">Azure Kubernetes Service</a>

Microsoft Docs: [AKS Overview](https://docs.microsoft.com/azure/aks/)

AKS provides a managed Kuberentes cluster in Azure. Kubernetes is a container-based distributed platform for providing reliable and
fault-tolerant infrastructure.

Currently, CLRIE is leveraged by [Visual Studio Snapshot Debugger](#snapshot-debugger) on AKS Linux (Alpine & Ubuntu) Docker Containers and
requires manual configuration by users. More details can be found at
[vssnapshotdebugger-docker repo](https://github.com/Microsoft/vssnapshotdebugger-docker) on GitHub.

# <a name="products">Products</a>

## <a name="application-insights">Application Insights</a>

Application Insights is supported in numerous configurations across a multitude of Azure platforms with various features, some of which
leverage CLRIE and some do not.

See [Application Insights Overview](https://docs.microsoft.com/azure/azure-monitor/app/app-insights-overview)

Also refer to Application Insights extension
[design specs](https://github.com/Microsoft/InstrumentationEngine-Intercept/blob/develop/design-principles.md)


## <a name="snapshot-debugger">Visual Studio Snapshot Debugger</a>

### Overview

The Visual Studio Snapshot Debugger allows users to set snappoints and logpoints (similar to breakpoints) in order to generate snapshots or
on-demand logging against their production applications with minimal impact.

See [Debug a live Azure app](https://aka.ms/snappoint) for more details.