# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Usage:
#   .\DockerLocalImage.ps1 [musl|gnu]
#   Creates a local image based on the DockerFiles in this repository. The local image can be used for 
#   performing local docker builds.

<#
.SYNOPSIS 
    Creates a local docker image that can be used for building CLRIE on Linux.
.EXAMPLE
    .\DockerLocalImage.ps1 gnu
#>
param
(
    # Optionally change the enlistment root.
    [Parameter(Mandatory=$false)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({$_ -and (Test-Path $_)})]
    [String] $EnlistmentRoot = $(Resolve-Path -Path "$PSScriptRoot\..\.."),

    # Indicates what version of the c runtime will be used for the build.
    # This determines the version of Linux that will be used in the image
    [Parameter(Mandatory=$false)]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('gnu', 'musl')]
    [String] $CLib = 'gnu',

    # Indicates that the image should be rebuilt.
    [Parameter()]
    [Switch] $Rebuild
)

$linux = ''
if ($CLib -eq 'musl') {
    $linux = 'alpine'
} elseif ($CLib -eq 'gnu') {
    $linux = 'ubuntu'
} else {
    Write-Error "Unrecognized clib '$CLib"
    exit 1
}

$imageName = "clrielocal:$CLib"
# Check to see if the image already esists.
$exists = docker images $imageName --format 'yes'

if (-not $?) {
    Write-Error "Could not execute docker command".
    exit 1
}

if ($exists -eq "yes") {
    Write-Host "Image '$imageName' already exists."
    if (-not $Rebuild) {
        Write-Output $imageName
        exit 0
    }
}

$dockerDir = "$EnlistmentRoot\src\unix\docker\dockerfiles\build\$linux"

if (-not (Test-Path "$dockerDir\DockerFile" -PathType Leaf)) {
    Write-Error "Could not find path to required DockerFile in '$dockerDir"
    exit 1
}

$Expression = "Get-Content '$dockerDir\DockerFile' | docker build - -t '$imageName'"
Write-Host $Expression
Invoke-Expression $Expression | Write-Host
if (-not $?) {
    Write-Error "Error building image '$imageName'"
    exit 1
}

Write-Host "Created '$imageName'"
Write-Output $imageName

