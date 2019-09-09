<#
    Copyright (c) Microsoft Corporation. All rights reserved.
    Licensed under the MIT License.
#>

function Get-ClrieXmlEntries {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$false)]
        [Switch]
        $DebugWait,

        [Parameter(Mandatory=$false)]
        [Switch]
        $DisableSignatureValidation
    )

    Write-Output @{
        element     = 'add'
        name        = 'CORECLR_ENABLE_PROFILING'
        transform   = 'RemoveAll'
        destination = "configuration.'system.webServer'.runtime.environmentVariables"
    }

    Write-Output @{
        element     = 'add'
        name        = 'CORECLR_PROFILER'
        transform   = 'RemoveAll'
        destination = "configuration.'system.webServer'.runtime.environmentVariables"
    }

    Write-Output @{
        element     = 'add'
        name        = 'CORECLR_PROFILER_PATH_32'
        transform   = 'RemoveAll'
        destination = "configuration.'system.webServer'.runtime.environmentVariables"
    }

    Write-Output @{
        element     = 'add'
        name        = 'CORECLR_PROFILER_PATH_64'
        transform   = 'RemoveAll'
        destination = "configuration.'system.webServer'.runtime.environmentVariables"
    }

    Write-Output @{
        element     = 'add'
        name        = 'COR_ENABLE_PROFILING'
        transform   = 'RemoveAll'
        destination = "configuration.'system.webServer'.runtime.environmentVariables"
    }

    Write-Output @{
        element     = 'add'
        name        = 'COR_PROFILER'
        transform   = 'RemoveAll'
        destination = "configuration.'system.webServer'.runtime.environmentVariables"
    }

    Write-Output @{
        element     = 'add'
        name        = 'COR_PROFILER_PATH_32'
        transform   = 'RemoveAll'
        destination = "configuration.'system.webServer'.runtime.environmentVariables"
    }

    Write-Output @{
        element     = 'add'
        name        = 'COR_PROFILER_PATH_64'
        transform   = 'RemoveAll'
        destination = "configuration.'system.webServer'.runtime.environmentVariables"
    }

    if ($DisableSignatureValidation) {
        Write-Output @{
            element     = 'add'
            name        = 'MicrosoftInstrumentationEngine_DisableCodeSignatureValidation'
            value       = '1'
            transform   = 'InsertIfMissing'
            destination = "configuration.'system.webServer'.runtime.environmentVariables"
        }
    }

    if ($DebugWait) {
        Write-Output @{
            element     = 'add'
            name        = 'MicrosoftInstrumentationEngine_DebugWait'
            value       = '1'
            transform   = 'InsertIfMissing'
            destination = "configuration.'system.webServer'.runtime.environmentVariables"
        }
    }
}

function Edit-XdtContent {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory=$true)]
        [ValidateNotNull()]
        [xml]
        $XdtFile,

        [Parameter(Mandatory=$true)]
        [ValidateNotNull()]
        [Object[]]
        $XmlEntries
    )

    foreach ($entry in $XmlEntries) {
        $elem = $XdtFile.CreateElement($entry.element)
        if ($entry.name) {
            [void]$elem.SetAttribute("name", $entry.name)
            [void]$elem.SetAttribute("Locator", "http://schemas.microsoft.com/XML-Document-Transform", "Match(name)")
        }
        if ($entry.path) {
            [void]$elem.SetAttribute("path", $entry.path)
        }
        if ($entry.transform) {
            [void]$elem.SetAttribute("Transform", "http://schemas.microsoft.com/XML-Document-Transform", $entry.transform)
        }

        if ($entry.value) {
            [void]$elem.SetAttribute("value", $entry.value)
        }

        $null = Invoke-Expression "`$XdtFile.$($entry.destination).PrependChild(`$elem)"
    }
}