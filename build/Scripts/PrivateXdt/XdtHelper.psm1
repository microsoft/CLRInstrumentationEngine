# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

function Get-ClrieXmlEntriesToAdd {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$false)]
        [Switch]
        $DebugWait,

        [Parameter(Mandatory=$false)]
        [Switch]
        $DisableSignatureValidation,

        [Parameter(Mandatory=$false)]
        [Switch]
        $Scm
    )

    if ($Scm)
    {
        Write-Output @{
            element     = 'add'
            name        = 'MicrosoftInstrumentationEngine_InstallationRoot'
            transform   = 'RemoveAll'
            destination = "configuration.'system.webServer'.runtime.environmentVariables"
        }
    }
    else
    {
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
            name        = 'MicrosoftInstrumentationEngine_IsPreinstalled'
            transform   = 'RemoveAll'
            destination = "configuration.'system.webServer'.runtime.environmentVariables"
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

        if ($DisableSignatureValidation) {
            Write-Output @{
                element     = 'add'
                name        = 'MicrosoftInstrumentationEngine_DisableCodeSignatureValidation'
                value       = '1'
                transform   = 'InsertIfMissing'
                destination = "configuration.'system.webServer'.runtime.environmentVariables"
            }
        }
    }

}

function Get-ClrieXmlEntriesToRemove {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory=$false)]
        [Switch]
        $Attach,

        [Parameter(Mandatory=$false)]
        [Switch]
        $Scm
    )

    if ($Attach -and -not $Scm) {
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='CORECLR_ENABLE_PROFILING']"
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='CORECLR_PROFILER']"
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='CORECLR_PROFILER_PATH_32']"
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='CORECLR_PROFILER_PATH_64']"
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='COR_ENABLE_PROFILING']"
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='COR_PROFILER']"
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='COR_PROFILER_PATH_32']"
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='COR_PROFILER_PATH_64']"
        Write-Output "/configuration/system.webServer/runtime/environmentVariables/add[@name='MicrosoftInstrumentationEngine_IsPreinstalled']"
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
        $XmlEntriesToAdd,

        [Parameter(Mandatory=$false)]
        [string[]]
        $XmlEntriesToRemove
    )

    foreach ($entry in $XmlEntriesToRemove) {
        $elem = $XdtFile.SelectSingleNode($entry)
        [void]$elem.ParentNode.RemoveChild($elem)
    }

    # These elements are prepended to the first child element of their destination.
    # Reverse the array in order to maintain the declared order of the elements.
    [array]::Reverse($XmlEntriesToAdd)

    foreach ($entry in $XmlEntriesToAdd) {
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

        # Invoke-Expression treats the input string as PowerShell code, allowing dynamic construction of code.
        # The following code constructs the XPath in the $XdtFile from each $entry's destination
        # and then prepends the $elem node constructed from the $entry's properties.
        $null = Invoke-Expression "`$XdtFile.$($entry.destination).PrependChild(`$elem)"
    }
}