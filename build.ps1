# Copyright (c) Microsoft Corporation. All rights reserved.
#

param(
    [switch] $IncludeTests,
    [switch] $SkipBuild,
    [switch] $SkipPackaging,
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

function Get-AuthorizedNuget
{
    $nugetPath = "$repoPath\nuget.exe"
    if (!(Test-Path $nugetPath))
    {
        # Download nuget.exe
        Write-Verbose "Downloading NuGet.exe to $nugetPath"
        $sourceNugetExe = "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe"
        Invoke-WebRequest $sourceNugetExe -OutFile $nugetPath | Out-Null
    }

    # Update to latest version
    Invoke-Expression -Command "$nugetPath update -self" | Out-Null

    return $nugetPath
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
$filterArgs = "-latest -prerelease -version `"[15.0,16.0)`" -requires $($VsRequirements -join ' ') -property installationPath"
$installationPath = Invoke-ExpressionHelper -Executable $vswhere -Arguments $filterArgs -Activity 'Determine MSBuild location'
if (!$installationPath)
{
    Write-Error "Please check the Individual Components installed with your latest VS installation. This project requires version 10493 of the Windows 10 SDK component and both the 'Visual C++ ATL (x86/x64) with Spectre Mitigations' and 'VC++ 2017 version 15.8 v14.15 Libs for Spectre (x86 and x64)'."
}

$msbuild = Join-Path $installationPath 'MSBuild\15.0\bin\MSBuild.exe'
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

# Nuget Restore solutions
$nuget = Get-AuthorizedNuget

if (!$SkipBuild)
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

    $restoreArgs = @(
        "restore $repoPath\InstrumentationEngine.sln -configFile $repoPath\NuGet.config"
    )
    Invoke-ExpressionHelper -Executable $nuget -Arguments $restoreArgs -Activity 'Nuget Restore Solutions'

    # Build InstrumentationEngine.sln
    $buildArgs = @(
        "$repoPath\InstrumentationEngine.sln /p:platform=`"x86`" /p:configuration=`"$configuration`" /clp:$($clParams)"
        "$repoPath\InstrumentationEngine.sln /p:platform=`"x64`" /p:configuration=`"$configuration`" /clp:$($clParams)"
        "$repoPath\InstrumentationEngine.sln /p:platform=`"Any CPU`" /p:configuration=`"$configuration`" /clp:$($clParams) /m"
    )
    Invoke-ExpressionHelper -Executable "$msbuild" -Arguments $buildArgs -Activity 'Build InstrumentationEngine.sln'
}

if (!$SkipPackaging)
{
    $restoreArgs = @(
        "restore $repoPath\src\InstrumentationEngine.Packages.sln -configFile $repoPath\NuGet.config"
    )
    Invoke-ExpressionHelper -Executable $nuget -Arguments $restoreArgs -Activity 'Nuget Restore Solutions'

    # Build InstrumentationEngine.Packages.sln
    $buildArgs = @(
        "$repoPath\src\InstrumentationEngine.Packages.sln /p:platform=`"x86`" /p:configuration=`"$configuration`" /clp:$($clParams) /m"
        "$repoPath\src\InstrumentationEngine.Packages.sln /p:platform=`"x64`" /p:configuration=`"$configuration`" /clp:$($clParams) /m"
        "$repoPath\src\InstrumentationEngine.Packages.sln /p:platform=`"Any CPU`" /p:configuration=`"$configuration`" /clp:$($clParams) /m"
    )
    Invoke-ExpressionHelper -Executable "$msbuild" -Arguments $buildArgs -Activity 'Build InstrumentaitonEngine.Packages.sln'
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
    $anyCpuTests = (Get-ChildItem -path "$repoPath\bin\$configuration\anycpu" -Filter *Tests*.dll -Recurse -Exclude *TestAdapter.tests.dll | Select-Object -ExpandProperty FullName | ForEach-Object { "`"$_`""})
    $x86Tests = (Get-ChildItem -path "$repoPath\bin\$configuration\x86" -Filter *Tests*.dll -Recurse -Exclude *TestAdapter.tests.dll | Select-Object -ExpandProperty FullName | ForEach-Object { "`"$_`""})
    $x64Tests = (Get-ChildItem -path "$repoPath\bin\$configuration\x64" -Filter *Tests*.dll -Recurse -Exclude *TestAdapter.tests.dll | Select-Object -ExpandProperty FullName | ForEach-Object { "`"$_`""})

    $testBuildArgs = @(
        ($x86Tests -join ' ') + " /Settings:`"$repoPath\tests\TestSettings\x86.runsettings`""
        ($x64Tests -join ' ') + " /Settings:`"$repoPath\tests\TestSettings\x64.runsettings`""
        ($anyCpuTests -join ' ') + " /Settings:`"$repoPath\tests\TestSettings\x86.runsettings`"" # AnyCPU
    )

    Invoke-ExpressionHelper -Executable "$vstest" -Arguments $testBuildArgs -Activity 'Run Tests'
}