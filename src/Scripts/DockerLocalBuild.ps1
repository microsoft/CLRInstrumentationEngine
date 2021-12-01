# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Usage:
#   .\DockerLocalBuild.ps1 D:\ClrInstrumentationEngine musl Debug
#   Will create an alpine docker container and build ClrInstrumentationEngine repo with Debug configuration in the container.
#   The build output will be immediately available on the host machine at D:\ClrInstrumentationEngine\out\Linux.
#   Please note that this command builds clean and deliberately wipes out anything under D:\ClrInstrumentationEngine\out\Linux.
#
# NOTE: Currently this script supports building with glibc in Ubuntu 16.04 and musl-libc in Alpine 3.6.
#       If you are using the sample Dockerfiles then you want "gnu" CLib for the Debian 9 sample and
#       "musl" CLib for the Alpine 3.7 sample. More generally, use "gnu" build output for glibc-
#       based distros and "musl" build output for musl-libc-based distros.

<#
.SYNOPSIS
    Launch a local Linux build using docker.
.EXAMPLE
    .\DockerLocalBuild.ps1 D:\ClrInstrumentationEngine musl Debug
#>
param
(
    # Optionally change the enlistment root
    [Parameter(Mandatory=$false)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({$_ -and (Test-Path $_)})]
    [String] $EnlistmentRoot = $(Resolve-Path -Path "$PSScriptRoot\..\.."),
    
    # Indicates which version of the c runtime will be used for the build.
    # This determines the flavor of Linux that will be used for the build.
    [Parameter(Mandatory=$false)]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('gnu', 'musl')]
    [String] $CLib = 'gnu',

    # Which configuration to build. Debug or release. Default is Debug.
    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('Debug', 'Release')]
    [String] $Type="Debug",

    # Build docker image from local docker files. Docker file used is determined based on the value
    # of CLib. Default is to download the official image that is used for released builds
    [Parameter()]
    [Switch] $BuildDockerImage,

    # When -BuildDockerImage is supplied, this will rebuild images that were already created.
    [Parameter()]
    [Switch] $RebuildImage,

    # Launch with an interactive shell. Does not run the build.
    [Parameter()]
    [Switch] $Interactive,

    [Parameter()]
    [Switch] $CreateSemaphore,

    [Parameter()]
    [Switch] $TranslateHostPathsToTarget,

    # Use Windows Subsystem for Linux instead of Docker Desktop. WSL must have docker installed.
    [Parameter()]
    [Switch] $Wsl,

    # The name of the distribution of wsl to use. Ignored if $Wsl is false. The distro used myst
    # have docker and the docker daemon (dockerd) installed and running.
    [Parameter()]
    [String] $WslDistro

)

$ErrorActionPreference = "Stop"

$BaseImage = "";

if ($BuildDockerImage) {

    $Command = "$PSScriptRoot\DockerLocalImage.ps1 -EnlistmentRoot '$EnlistmentRoot' -CLib $CLib -Rebuild:`$$RebuildImage -Wsl:`$$Wsl -WslDistro:`$$WslDistro"
    $BaseImage = Invoke-Expression $Command
    if (-not $?) {
        write-error "Error creating docker image"
        exit 1
    }
}


if ([string]::IsNullOrEmpty($BaseImage))
{
    if ($CLib -eq "gnu")
    {
        $BaseImage = "proddiagbuild.azurecr.io/clrie-build-ubuntu:latest"
    }
    elseif ($CLib -eq "musl")
    {
        $BaseImage = "proddiagbuild.azurecr.io/clrie-build-alpine:latest"
    }
    else
    {
        write-error "Unrecognized C library: $CLib."
        exit 1
    }
}

$containerName = "clrinstrumentationengine-build-$(New-Guid)"

$pathInfo = resolve-path $EnlistmentRoot
if ($Wsl -or $TranslateHostPathsToTarget)
{
    # This ToLowerInvariant is a workaround to a problem Wiktor has where Docker gets confused
    # about drive letter 'd' and drive letter 'D'. If the mount step in 'docker run' fails for
    # you then you may want to comment this line out and email wiktork and nbilling to figure
    # out a permanent fix.
    $driveLetter = $pathInfo.Drive.ToString().ToLowerInvariant()
    $relativePath = $pathInfo.Path.Substring(3).Replace('\', '/')
    $EnlistmentMountPath = "$driveLetter/$relativePath"
    if ($Wsl)
    {
        $EnlistmentMountPath ="/mnt/$EnlistmentMountPath"
    }
    else
    {
        $EnlistmentMountPath ="//$EnlistmentMountPath"
    }
}
else
{
    $EnlistmentMountPath = $EnlistmentRoot
}

Write-Host "Executing inside container using docker image '$BaseImage'"
$command = ""
if ($Interactive)
{
    $command ="docker run --rm -ti --name $containerName -v ${EnlistmentMountPath}:/root/ClrInstrumentationEngine --net=host -w '/root/ClrInstrumentationEngine' $BaseImage bash"
}
elseif (-not $CreateSemaphore)
{
    $command = "docker run --rm --name $containerName -v ${EnlistmentMountPath}:/root/ClrInstrumentationEngine --net=host $BaseImage /root/ClrInstrumentationEngine/src/build.sh $Type clean"
}
else
{
    $command = "docker run --rm --name $containerName -v ${EnlistmentMountPath}:/root/ClrInstrumentationEngine --net=host $BaseImage /bin/bash -c `"/root/ClrInstrumentationEngine/src/build.sh $Type clean && touch /root/ClrInstrumentationEngine/build.sem`""
}

if ($Wsl)
{
    $wslCommand = "wsl"
    if ($WslDistro)
    {
        $wslCommand = "$wslCommand -d $WslDistro"
    }

    $command = $command.Replace("`"", "\`"")
    $command = "$wslCommand bash -c `"sudo $command`""
}
Write-Host $command
Invoke-Expression $command
