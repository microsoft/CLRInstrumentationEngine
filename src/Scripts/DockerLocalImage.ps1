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
    [Switch] $Rebuild,

    #Indicates that WSL should be used instead of docker desktop. 
    [Parameter()]
    [Switch] $Wsl,

    # The name of the distribution of wsl to use. Ignored if $Wsl is false
    [Parameter()]
    [String] $WslDistro
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

$WslCommand = ""

if ($Wsl)
{
    $WslCommand = "wsl"
    if ($WslDistro)
    {
        $WslCommand = "$wslCommand -d $WslDistro"
    }
}
elseif ($IsMac -or $IsLinux)
{
    $sudo = "sudo"
}

$imageName = "clrielocal:$CLib"
# Check to see if the image already esists.
$existenceCheck = "$sudo docker images $imageName --format 'yes'"
if ($Wsl)
{
    $existenceCheck = "$wslCommand bash -c `"sudo $existenceCheck`""
}
Write-Host $existenceCheck
$exists = Invoke-Expression $existenceCheck

if (-not $?) {
    Write-Error "Could not execute docker command".
    exit 1
}

Write-Host $exists

if ($exists -eq "yes") {
    Write-Host "Image '$imageName' already exists."
    if (-not $Rebuild) {
        Write-Output $imageName
        exit 0
    }
}

$dockerDir = Join-Path -Path "$EnlistmentRoot" -Child "src\unix\docker\dockerfiles\build\$linux"
$dockerContext = Join-Path -Path "$EnlistmentRoot" -Child "src\unix\docker\context"
$dockerFile = Join-Path -Path "$dockerDir" -Child "DockerFile"

if (-not (Test-Path "$dockerFile" -PathType Leaf)) {
    Write-Error "Could not find path to required DockerFile in '$dockerDir"
    exit 1
}

if (-not (Test-Path "$dockerContext" -PathType Container)) {
    Write-Error "Could not find Docker context path '$dockerContext'"
    exit 1
}

if ($Wsl)
{
    $pathInfo = resolve-path $dockerDir
    $drive = $pathInfo.Drive.ToString().ToLowerInvariant()
    $dockerDir = $pathInfo.Path.Substring(3).Replace('\','/') 
    $dockerDir = "/mnt/$drive/$dockerDir"
    $pathInfo = resolve-path $dockerContext
    $drive = $pathInfo.Drive.ToString().ToLowerInvariant()
    $dockerContext = $pathInfo.Path.Substring(3).Replace('\','/') 
    $dockerContext = "/mnt/$drive/$dockerContext"
    Invoke-Expression "$wslCommand bash -c `"cat $dockerDir/DockerFile | sudo docker build -t '$imageName' -f - '$dockerContext'`"" | Write-Host
}
else
{
    $Expression = "Get-Content '$dockerFile' | $sudo docker build -t '$imageName' -f - '$dockerContext'"
    Write-Host $Expression
    Invoke-Expression $Expression | Write-Host
}

if (-not $?) {
    Write-Error "Error building image '$imageName'"
    exit 1
}

Write-Host "Created '$imageName'"
Write-Output $imageName

