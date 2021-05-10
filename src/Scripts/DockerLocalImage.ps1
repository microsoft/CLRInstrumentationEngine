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
    Write-Output "Image '$imageName' already exists."
    if (-not $Rebuild) {
        exit 0
    }

    # Delete the current image
    docker image rm $imageName

    if (-not $?) {
        Write-Error "Error removing image '$imageName'"
        exit 1
    }
}

$EnlistmentRoot = $(Resolve-Path -Path "$PSScriptRoot\..\..")
$dockerDir = "$EnlistmentRoot\src\unix\docker\dockerfiles\build\$linux"

if (-not (Test-Path "$dockerDir\DockerFile" -PathType Leaf)) {
    Write-Error "Could not find path to required DockerFile in '$dockerDir"
    exit 1
}

pushd $dockerDir

try {
    docker build -t "$imageName" .
    if (-not $?) {
        Write-Error "Error building image '$imageName'"
        exit 0
    }
} finally {
    popd
}

Write-Output "Created image '$imageName'"

