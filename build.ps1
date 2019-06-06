# Copyright (c) Microsoft Corporation. All rights reserved.
#

param(
    [switch] $IncludeTests,
    [switch] $SkipBuild,
    [switch] $SkipPackaging,
    [switch] $SkipCleanAndRestore,
    [switch] $Release,
    [switch] $Verbose
)

$ErrorActionPreference = 'Stop'
$VerbosePreference = 'Continue'

function Invoke-ExpressionHelper
{
    [CmdletBinding()]
    param(
        [string] $Executable,
        [string[]] $Arguments,
        [string] $Activity
    )

    Write-Host -Verbose "Running '$Activity'..." -ForegroundColor Green -BackgroundColor Black

    $progress = 1.0

    foreach ($arg in $Arguments)
    {
        Write-Progress -Activity $Activity -Status "Percent Complete: $([Math]::Round($progress/$Arguments.Count, 2) * 100)%" -PercentComplete ($progress/$Arguments.Count * 100)
        Write-Verbose -Verbose "[ $Executable $arg ]"

        Invoke-Expression "& $Executable $arg"

        if ($LASTEXITCODE -ne 0)
        {
            Write-Error "Terminating..."
        }
        $progress++
    }
}

function Verify-DotnetExists
{
    $dotnetCommand = Get-Command 'dotnet' -ErrorAction SilentlyContinue
    if (!$dotnetCommand)
    {
        Write-Error "No .NET Core CLI exists. Please go to https://aka.ms/dotnet-download and install the latest SDK."
    }
    else
    {
        Write-Verbose "Verified .NET Core CLI exists."
        & 'dotnet' --info
    }
}

###
# Set variables
###
if ($Verbose)
{
    $clParams = "Verbosity=d"
}
else
{
    $clParams = "ErrorsOnly"
}

if ($Release)
{
    $configuration = "Release"
}
else
{
    $configuration = "Debug"
}

$BuildVersion = "$([System.DateTime]::Now.ToString('yyyyMMddhhmm'))" # Set the build number so it's constant for the entirety of this build

$SignType = 'None' # Used for internal testing of signing.

###
# Attempt to find CLR Instrumentation Engine repo
###
try { $repoRoot = (Get-Item $PSCommandPath).Directory.FullName } catch { } # ignore terminating errors
if ($repoRoot)
{
    Write-Verbose -Verbose "Checking if [$repoRoot] is the CLR Instrumentation Engine repo"
    $repoMarker = Join-Path $repoRoot 'EnlistmentRoot.marker'
    if (Test-Path $repoMarker -ErrorAction SilentlyContinue)
    {
        $repoPath = $repoRoot
    }
}

if (-not ($repoPath))
{
    $repoPath = Read-Host "Please enter the full path to the local repository"
}

###
# Picks up msbuild from vs2017 installation
###
$VsRequirements = @(
    'Microsoft.Component.MSBuild'
    'Microsoft.VisualStudio.Workload.NativeDesktop'
    'Microsoft.VisualStudio.Component.VC.ATL.Spectre'
    'Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre'
)

Write-Verbose -Verbose "Checking for VS installation with these installed components: `n`n$($VsRequirements | Out-String)`n"
$vswhere = "`"${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe`""
$filterArgs = "-latest -prerelease -requires $($VsRequirements -join ' ') -property installationPath"

# Restrict the VS version if running from a context that has the VisualStudioVersion env variable (e.g. Developer Command Prompt).
# This will make sure that VisualStudioVersion matches the VS version of MSBuild in order to avoid mismatches (e.g. using a Dev15
# Developer Command Prompt but invoking Dev16's MSBuild).
if ($env:VisualStudioVersion)
{
    $vsversion = [version]$($env:VisualStudioVersion)
    # This is a version range that looks like "[15.0,16.0)"
    $filterArgs = "$filterArgs -version `"[$($vsversion.Major).0,$($vsversion.Major + 1).0)`""
}

$installationPath = Invoke-Expression "& $vswhere $filterArgs"
$msbuild = Join-Path $installationPath 'MSBuild\15.0\bin\MSBuild.exe'
if (-not (Test-Path $msbuild))
{
    $msbuild = Join-Path $installationPath 'MSBuild\Current\bin\MSBuild.exe'
}

if (-not (Test-Path $msbuild))
{
    $msbuild = Join-Path (Read-Host "Please enter the full path to your VS installation (eg. 'C:\Program Files (x86)\Microsoft Visual Studio\Preview\Enterprise)'`r`n") 'MSBuild\15.0\Bin\MSBuild.exe'
}

if (-not (Test-Path $msbuild))
{
    Write-Error 'Cannot find msbuild.exe. Please check your VS 2017 installation.'
}

$msbuild = "`"$msbuild`""

###
# Local Build
###
Verify-DotnetExists

if (!$SkipBuild)
{
    if (!$SkipCleanAndRestore)
    {
        # Clean up bin & obj folder if exists
        if (Test-Path "$repoPath\bin\$configuration")
        {
            Remove-Item -Force -Recurse "$repoPath\bin\$configuration"
        }

        if (Test-Path "$repoPath\obj\")
        {
            Remove-Item -Force -Recurse "$repoPath\obj\"
        }

        # NuGet restore disregards platform/configuration
        # dotnet restore defaults to Debug|Any CPU, which requires the /p:platform specification in order to replicate NuGet restore behavior.
        $restoreArgsInit = "restore $repoPath\InstrumentationEngine.sln --configfile $repoPath\NuGet.config"
        $restoreArgs = @(
            "$restoreArgsInit /p:platform=`"x86`""
            "$restoreArgsInit /p:platform=`"x64`""
            "$restoreArgsInit /p:platform=`"Any CPU`""
        )
        Invoke-ExpressionHelper -Executable "dotnet" -Arguments $restoreArgs -Activity 'dotnet Restore Solutions'
    }

    # Build InstrumentationEngine.sln
    $buildArgsInit = "$repoPath\InstrumentationEngine.sln /p:configuration=`"$configuration`" /p:SignType=$SignType /p:BuildVersion=$BuildVersion /clp:$($clParams)"
    $buildArgs = @(
        "$buildArgsInit /p:platform=`"x86`""
        "$buildArgsInit /p:platform=`"x64`""
        "$buildArgsInit /p:platform=`"Any CPU`" /m"
    )
    Invoke-ExpressionHelper -Executable "$msbuild" -Arguments $buildArgs -Activity 'Build InstrumentationEngine.sln'
}

if (!$SkipPackaging)
{
    # Remove files from obj folders as they can become stale
    Remove-Item "$repoPath\obj\InstrumentationEngine.Fragment" -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item "$repoPath\obj\InstrumentationEngine.Module" -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item "$repoPath\obj\InstrumentationEngine.Installer" -Recurse -Force -ErrorAction SilentlyContinue

    # The SkipBuild switch prevents bin/obj cleanup and also means we're testing packaging. This will cleanup old/stale packages
    if ($SkipBuild)
    {
        # Remove package artifacts from bin folders
        Get-ChildItem "$repoPath\bin\$configuration" -Include *.msi,*.msm,*.wixlib,*.nupkg -Recurse | Remove-Item -Force
    }

    # NuGet restore disregards platform/configuration
    # dotnet restore defaults to Debug|Any CPU, which requires the /p:platform specification in order to replicate NuGet restore behavior.
    $restoreArgsInit = "restore $repoPath\src\InstrumentationEngine.Packages.sln --configfile $repoPath\NuGet.config"
    $restoreArgs = @(
        "$restoreArgsInit /p:platform=`"x86`""
        "$restoreArgsInit /p:platform=`"x64`""
        "$restoreArgsInit /p:platform=`"Any CPU`""
    )
    Invoke-ExpressionHelper -Executable "dotnet" -Arguments $restoreArgs -Activity 'dotnet Restore Solutions'

    # Build InstrumentationEngine.Packages.sln
    $buildArgsInit = "$repoPath\src\InstrumentationEngine.Packages.sln /p:configuration=`"$configuration`" /p:SignType=$SignType /p:BuildVersion=$BuildVersion /clp:$($clParams) /m"
    $buildArgs = @(
        "$buildArgsInit /p:platform=`"x86`""
        "$buildArgsInit /p:platform=`"x64`""
        "$buildArgsInit /p:platform=`"Any CPU`""
    )
    Invoke-ExpressionHelper -Executable "$msbuild" -Arguments $buildArgs -Activity 'Build InstrumentationEngine.Packages.sln'
}

# Build Tests
if ($IncludeTests)
{
    $vstest = Join-Path $installationPath 'Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe'
    if (-not (Test-Path $vstest))
    {
        Write-Error "Cannot find $vstest. Please check your VS 2017 installation."
    }

    $vstest = "`"$vstest`""

    # Release builds contain the RawProfilerHook.Tests_AnyCPU.dll which is not tested in debug configuration.
    $x86Tests = (Get-ChildItem -path "$repoPath\bin\$configuration\x86" -Filter *Tests*.dll -Recurse -Exclude *TestAdapter.tests.dll | Select-Object -ExpandProperty FullName | ForEach-Object { "`"$_`""})
    $x64Tests = (Get-ChildItem -path "$repoPath\bin\$configuration\x64" -Filter *Tests*.dll -Recurse -Exclude *TestAdapter.tests.dll | Select-Object -ExpandProperty FullName | ForEach-Object { "`"$_`""})
    $anyCpuTests = (Get-ChildItem -path "$repoPath\bin\$configuration\anycpu" -Filter *Tests*.dll -Recurse -Exclude *TestAdapter.tests.dll | Select-Object -ExpandProperty FullName | ForEach-Object { "`"$_`""})

    $testBuildArgs = @(
        ($x86Tests -join ' ') + " /Settings:`"$repoPath\tests\TestSettings\x86.runsettings`""
        ($x64Tests -join ' ') + " /Settings:`"$repoPath\tests\TestSettings\x64.runsettings`""
        ($anyCpuTests -join ' ') + " /Settings:`"$repoPath\tests\TestSettings\x86.runsettings`"" # AnyCPU
    )

    Invoke-ExpressionHelper -Executable "$vstest" -Arguments $testBuildArgs -Activity 'Run Tests'
}