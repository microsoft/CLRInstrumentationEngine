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

.PARAMETER DisableSignatureValidation
    Optional, sets MicrosoftInstrumentationEngine_DisableCodeSignatureValidation which allows CLRIE to use unsigned Instrumentation Methods.

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

# CLRIE configuration variables
@(
    # COR_PROFILER
    @{
        name   = 'COR_ENABLE_PROFILING'
        value  = 1
        enable = $true
    }
    @{
        name   = 'COR_PROFILER'
        value  = '{324F817A-7420-4E6D-B3C1-143FBED6D855}'
        enable = $true
    }
    @{
        name   = 'COR_PROFILER_PATH_32'
        value  = "${env:ProgramFiles(x86)}\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x86.dll"
        enable = $true
    }
    @{
        name   = 'COR_PROFILER_PATH_64'
        value  = "$env:ProgramFiles\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x64.dll"
        enable = $true
    }
    @{
        name   = 'CORECLR_ENABLE_PROFILING'
        value  = 1
        enable = $true
    }
    @{
        name   = 'CORECLR_PROFILER'
        value  = '{324F817A-7420-4E6D-B3C1-143FBED6D855}'
        enable = $true
    }
    @{
        name   = 'CORECLR_PROFILER_PATH_32'
        value  = "${env:ProgramFiles(x86)}\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x86.dll"
        enable = $true
    }
    @{
        name   = 'CORECLR_PROFILER_PATH_64'
        value  = "$env:ProgramFiles\Microsoft CLR Instrumentation Engine\Proxy\v1\InstrumentationEngine.ProfilerProxy_x64.dll"
        enable = $true
    }
    # CLRIE-specific
    @{
        name   = "MicrosoftInstrumentationEngine_DebugWait"
        value  = 1
        enable = $DebugWait
    }
    @{
        name   = 'MicrosoftInstrumentationEngine_DisableCodeSignatureValidation'
        value  = 1
        enable = $DisableSignatureValidation
    }
    # Proxy-specific
    @{
        name   = 'InstrumentationEngineProxy_UseDebug'
        value  = 1
        enable = $ProxyUseDebug
    }
    @{
        name   = 'InstrumentationEngineProxy_UsePreview'
        value  = 1
        enable = $ProxyUsePreview
    }
    # Instrumentation Method
    @{
        name   = 'MicrosoftInstrumentationEngine_ConfigPath32_TestMethod'
        value  = $InstrumentationMethodConfigPath
        enable = ($PSCmdlet.ParameterSetName -ieq 'InstrumentationMethod')
    }
    @{
        name   = 'MicrosoftInstrumentationEngine_ConfigPath64_TestMethod'
        value  = $InstrumentationMethodConfigPath
        enable = ($PSCmdlet.ParameterSetName -ieq 'InstrumentationMethod')
    }
    # Raw Profiler Hook
    @{
        name   = 'MicrosoftInstrumentationEngine_RawProfilerHook'
        value  = $RawProfilerHookGuid
        enable = ($PSCmdlet.ParameterSetName -ieq 'InstrumentationMethod' -or $PSCmdlet.ParameterSetName -ieq 'RawProfilerHook')
    }
    @{
        name   = 'MicrosoftInstrumentationEngine_RawProfilerHookPath_32'
        value  = $RawProfilerHookPath
        enable = ($PSCmdlet.ParameterSetName -ieq 'InstrumentationMethod' -or $PSCmdlet.ParameterSetName -ieq 'RawProfilerHook')
    }
    @{
        name   = 'MicrosoftInstrumentationEngine_RawProfilerHookPath_64'
        value  = $RawProfilerHookPath
        enable = ($PSCmdlet.ParameterSetName -ieq 'InstrumentationMethod' -or $PSCmdlet.ParameterSetName -ieq 'RawProfilerHook')
    }
) | ForEach-Object {
    $envVarPath = "Env:\$($_.name)"
    if ($_.enable) {
        Write-Verbose "Set $($_.name) = $($_.value)"
        New-Item -Path $envVarPath -Value $_.value -Force | Out-Null
    } elseif (Test-Path $envVarPath) {
        Write-Verbose "Removed $($_.name)"
        Remove-Item -Path $envVarPath
    }
}

Start-Process $ApplicationPath