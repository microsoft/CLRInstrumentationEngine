# Usage:
#   .\DockerLocalBuild.ps1 D:\ClrInstrumentationEngine alpine Debug
#   Will create an alpine docker container and build ClrInstrumentationEngine repo with Debug configuration in the container.
#   The build output will be immediately available on the host machine at D:\ClrInstrumentationEngine\out\Linux.
#   Please note that this command builds clean and deliberately wipes out anything under D:\ClrInstrumentationEngine\out\Linux.
#
# NOTE: Currently this script supports building with glibc in Ubuntu 16.04 and musl-libc in Alpine 3.6.
#       If you are using the sample Dockerfiles then you want "ubuntu" BaseImage for the Debian 9 sample and
#       "alpine" BaseImage for the Alpine 3.7 sample. More generally, use "ubuntu" build output for glibc-
#       based distros and "alpine" build output for musl-libc-based distros.

param
(
    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({$_ -and (Test-Path $_)})]
    [String] $EnlistmentRoot,

    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('alpine', 'ubuntu')]
    [String] $LinuxDistro,

    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('Debug', 'Release')]
    [String] $Type="Debug",

    [Parameter()]
    [Switch] $CreateSemaphore
)

$ErrorActionPreference = "Stop"

if ($LinuxDistro -eq "ubuntu")
{
    $BaseImage = "proddiagbuild.azurecr.io/clrie-build-ubuntu:latest"
}
elseif ($LinuxDistro -eq "alpine")
{
    $BaseImage = "proddiagbuild.azurecr.io/clrie-build-alpine:latest"
}
else
{
    write-error "Unrecognized distro: $LinuxDistro."
}

$containerName = "clrinstrumentationengine-build-$(New-Guid)"

$pathInfo = resolve-path $EnlistmentRoot
# This ToLowerInvariant is a workaround to a problem Wiktor has where Docker gets confused
# about drive letter 'd' and drive letter 'D'. If the mount step in 'docker run' fails for
# you then you may want to comment this line out and email wiktork and nbilling to figure
# out a permanent fix.
$driveLetter = $pathInfo.Drive.ToString().ToLowerInvariant()
$relativePath = $pathInfo.Path.Substring(3).Replace('\', '/')
$EnlistmentMountPath = "//$driveLetter/$relativePath"

# The build definition contains the keyvault secret. We will pull this in manually for local builds
$secretPat = (Get-AzureKeyVaultSecret -VaultName tsdtusr -Name DevDiv-VisualStudio-Com-PackageRead).SecretValueText
$nugetTemplate = Get-Content -Path "$pathInfo\src\unix\dependencies\nuget.template.config"
Set-Content -Path "$pathInfo\src\unix\dependencies\nuget.config" $nugetTemplate.Replace("{PersonalAuthenticationToken}", $secretPat) -Force

if (-not $CreateSemaphore)
{
    docker run --rm --name $containerName -v $EnlistmentMountPath`:/root/ClrInstrumentationEngine --net=host $BaseImage /root/ClrInstrumentationEngine/src/build.sh $Type clean
}
else
{
    docker run --rm --name $containerName -v $EnlistmentMountPath`:/root/ClrInstrumentationEngine --net=host $BaseImage /bin/bash -c "/root/ClrInstrumentationEngine/src/build.sh $Type clean && touch /root/ClrInstrumentationEngine/build.sem"
}
