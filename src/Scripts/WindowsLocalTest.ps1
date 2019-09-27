# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

<#
.SYNOPSIS
    This script spawns a process with the COR_PROFILER environment variables set. This script assumes the ClrInstrumentationEngine msi is installed.
    NOTE: The bitness of the application will dictate whether to use 32-bit or 64-bit for the profiler, instrumentation methods, and raw profiler hook.

.PARAMETER ApplicationPath
    Optional, the application to run with the profiler set. By default, will spawn another PowerShell process.

.PARAMETER InstrumentationMethodConfigPath
    Required for ParameterSet 'InstrumentationMethod'
    Optional for ParameterSet 'RawProfilerHook'
    Path to the config file defining the InstrumentationMethod to load.
    NOTE: This config file will be set for both the 32-bit and 64-bit environment variable.

.PARAMETER RawProfilerHookGuid
    Required for ParameterSet 'RawProfilerHook'
    Optional for ParameterSet 'InstrumentationMethod'
    The COR_PROFILER guid of a raw profiler.

.PARAMETER RawProfilerHookPath
    Required for ParameterSet 'RawProfilerHook'
    Optional for ParameterSet 'InstrumentationMethod'
    The COR_PROFILER_PATH string for the raw profiler dll.
    NOTE: This config file will be set for both the 32-bit and 64-bit environment variable.

.PARAMETER DebugWait
    Optional, sets MicrosoftInstrumentationEngine_DebugWait which causes the ClrInstrumentationEngine to wait for a debugger to attach.

.PARAMETER ProxyUseDebug
    Optional, forces the CLRIE ProfilerProxy to look at CLRIE versions with "_Debug" suffix.

.PARAMETER ProxyUsePreview
    Optional, allows the CLRIE ProfilerProxy to look at both release and preview (with "-build#####" suffix) versions.
#>

[CmdletBinding(DefaultParameterSetName='None')]
param(
    [Parameter(Mandatory = $false)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({ Test-Path $_ })]
    [string]
    $ApplicationPath = 'powershell',

    [Parameter(ParameterSetName='InstrumentationMethod', Mandatory = $true)]
    [Parameter(ParameterSetName='RawProfilerHook', Mandatory = $false)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({ Test-Path $_ })]
    [string]
    $InstrumentationMethodConfigPath,

    [Parameter(ParameterSetName='InstrumentationMethod', Mandatory = $false)]
    [Parameter(ParameterSetName='RawProfilerHook', Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [string]
    $RawProfilerHookGuid,

    [Parameter(ParameterSetName='InstrumentationMethod', Mandatory = $false)]
    [Parameter(ParameterSetName='RawProfilerHook', Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({ Test-Path $_ })]
    [string]
    $RawProfilerHookPath,

    [Parameter(Mandatory = $false)]
    [switch]
    $DebugWait,

    [Parameter(Mandatory = $false)]
    [switch]
    $DisableSignatureValidation,

    [Parameter(Mandatory = $false)]
    [switch]
    $ProxyUseDebug,

    [Parameter(Mandatory = $false)]
    [switch]
    $ProxyUsePreview
)

# COR_PROFILER variables
$env:COR_ENABLE_PROFILING = 1
$env:COR_PROFILER = "{324F817A-7420-4E6D-B3C1-143FBED6D855}"
$env:COR_PROFILER_PATH_32 = "C:\Program Files (x86)\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x86.dll"
$env:COR_PROFILER_PATH_64 = "C:\Program Files\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x64.dll"
$env:CORECLR_ENABLE_PROFILING = 1
$env:CORECLR_PROFILER = "{324F817A-7420-4E6D-B3C1-143FBED6D855}"
$env:CORECLR_PROFILER_PATH_32 = "C:\Program Files (x86)\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x86.dll"
$env:CORECLR_PROFILER_PATH_64 = "C:\Program Files\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x64.dll"

# CLRIE variables
if ($DebugWait) {
    $env:MicrosoftInstrumentationEngine_DebugWait = 1
} else {
    $env:MicrosoftInstrumentationEngine_DebugWait = 0
}

if ($DisableSignatureValidation) {
    $env:MicrosoftInstrumentationEngine_DisableCodeSignatureValidation = 1
} else {
    $env:MicrosoftInstrumentationEngine_DisableCodeSignatureValidation = 0
}

# ProfilerProxy variables
if ($ProxyUseDebug) {
    $env:InstrumentationEngineProxy_UseDebug = 1
} else {
    $env:InstrumentationEngineProxy_UseDebug = 0
}

if ($ProxyUsePreview) {
    $env:InstrumentationEngineProxy_UsePreview = 1
} else {
    $env:InstrumentationEngineProxy_UsePreview = 0
}

# Instrumentation Method variables
if ($PSCmdlet.ParameterSetName -ieq 'InstrumentationMethod') {
    $env:MicrosoftInstrumentationEngine_ConfigPath32_TestMethod = $InstrumentationMethodConfigPath
    $env:MicrosoftInstrumentationEngine_ConfigPath64_TestMethod = $InstrumentationMethodConfigPath
}

if ($PSCmdlet.ParameterSetName -ieq 'InstrumentationMethod' -or $PSCmdlet.ParameterSetName -ieq 'RawProfilerHook') {
    $env:MicrosoftInstrumentationEngine_RawProfilerHook = $RawProfilerHookGuid
    $env:MicrosoftInstrumentationEngine_RawProfilerHookPath_32 = $RawProfilerHookPath
    $env:MicrosoftInstrumentationEngine_RawProfilerHookPath_64 = $RawProfilerHookPath
}

Start-Process $ApplicationPath