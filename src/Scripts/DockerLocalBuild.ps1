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

param
(
    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({$_ -and (Test-Path $_)})]
    [String] $EnlistmentRoot,

    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('gnu', 'musl')]
    [String] $CLib,

    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('Debug', 'Release')]
    [String] $Type="Debug",

    [Parameter()]
    [Switch] $CreateSemaphore
)

$ErrorActionPreference = "Stop"

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

if (-not $CreateSemaphore)
{
    docker run --rm --name $containerName -v $EnlistmentMountPath`:/root/ClrInstrumentationEngine --net=host $BaseImage /root/ClrInstrumentationEngine/src/build.sh $Type clean
}
else
{
    docker run --rm --name $containerName -v $EnlistmentMountPath`:/root/ClrInstrumentationEngine --net=host $BaseImage /bin/bash -c "/root/ClrInstrumentationEngine/src/build.sh $Type clean && touch /root/ClrInstrumentationEngine/build.sem"
}
