# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# The XPath expressions below effectively returns a node if the conditional expression in '[]' is true and null if the conditional expression is false.
# The conditional expression checks whether the environment variable is set and/or if the variable value equals 'disabled'
# The conditional expression will be transformed by:
# 1. Expand any environment variable (ie. resolve %variable% to their values). If the variable is not set, then no changes occur.
# 2. Resolve character codes (ie. decode 0x25 to '%' character). This avoids expanding environment variables on both sides of the equality check.
# ------------------------------------------------------------------------------------------------------------------------------------------------------------
# Assuming %InstrumentationEngine_EXTENSION_VERSION% is set to ~1, the expression will evaluate to true:
#       '~1' != '%InstrumentationEngine_EXTENSION_VERSION%' and '~1' != 'disabled'
# Assuming %InstrumentationEngine_EXTENSION_VERSION% is not set, the expression will evaluate to false:
#       '%InstrumentationEngine_EXTENSION_VERSION%' != '%InstrumentationEngine_EXTENSION_VERSION%' and '%InstrumentationEngine_EXTENSION_VERSION%' != 'disabled' ### false
# ------------------------------------------------------------------------------------------------------------------------------------------------------------
# The "script" scope ensures that all functions in this module will have access to the variable.
# Defining the variable here allows it to be set when this module is imported.
$script:XPathExpression_IsAppSettingEnabled = "//*['%InstrumentationEngine_EXTENSION_VERSION%'!='0x25InstrumentationEngine_EXTENSION_VERSION0x25' and '%InstrumentationEngine_EXTENSION_VERSION%'!='disabled']"

function Get-EnvironmentVariableAddInfo {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true)]
        [ValidateNotNullOrEmpty()]
        [string]
        $EnvironmentVariable,

        [Parameter(Mandatory=$true)]
        [ValidateSet('prepend', 'append')]
        [string]
        $Operation,

        [Parameter(Mandatory=$true)]
        [ValidateNotNullOrEmpty()]
        [string]
        $Transform,

        [Parameter(Mandatory=$false)]
        [ValidateNotNullOrEmpty()]
        [string]
        $Value
    )

    # Operation affects the placement of this element.
    # <add name="$EnvironmentVariable" xdt:Locator="Match(name)" xdt:Transform="$Transform" />
    # or
    # <add name="$EnvironmentVariable" value=$Value xdt:Locator="Match(name)" xdt:Transform="$Transform" />
    $node = @{
        destination = '/configuration/system.webServer/runtime/environmentVariables'
        operation = $Operation
        element = 'add'
        attributes = [ordered] @{
            name = $EnvironmentVariable
            'xdt:Locator' = 'Match(name)'
            'xdt:Transform' = $Transform
        }
    }

    if ($Value) {
        $node.attributes.value = $Value
    }

    Write-Output $node
}

function Get-EnvironmentVariableUpdateInfo {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true)]
        [ValidateNotNullOrEmpty()]
        [string]
        $EnvironmentVariable
    )

    $formattableXPath = '/configuration/system.webServer/runtime/environmentVariables/add[@name="{0}" and @xdt:Transform="InsertIfMissing"]'

    # Update Nodes
    # Change xdt:Transform="InsertIfMissing" to xdt:Transform="InsertIfMissingAndXPathAny(EXPRESSION)"
    Write-Output @{
        destination = $formattableXPath -f $EnvironmentVariable
        operation = 'update'
        attributes = [ordered] @{
            'xdt:Transform' = "InsertIfMissingAndXPathAny($XPathExpression_IsAppSettingEnabled)"
        }
    }
}

function Get-ClrieXmlEntryOperations {
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
        # Prepend remove preinstalled extension's environment variables
        # <add name="MicrosoftInstrumentationEngine_InstallationRoot" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'MicrosoftInstrumentationEngine_InstallationRoot' -Operation 'prepend' -Transform 'RemoveAll'
    }
    else
    {
        # Prepend remove preinstalled extension's environment variables
        # NOTE: these will be prepended in the reverse order
        <#
            <add name="COR_ENABLE_PROFILING" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="COR_PROFILER" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="COR_PROFILER_PATH_32" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="COR_PROFILER_PATH_64" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="CORECLR_ENABLE_PROFILING" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="CORECLR_PROFILER" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="CORECLR_PROFILER_PATH_32" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="CORECLR_PROFILER_PATH_64" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="MicrosoftInstrumentationEngine_IsPreinstalled" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
            <add name="MicrosoftInstrumentationEngine_LogLevel" xdt:Locator="Match(name)" xdt:Transform="RemoveAll" />
        #>
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'COR_ENABLE_PROFILING' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'COR_PROFILER' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'COR_PROFILER_PATH_32' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'COR_PROFILER_PATH_64' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'CORECLR_ENABLE_PROFILING' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'CORECLR_PROFILER' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'CORECLR_PROFILER_PATH_32' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'CORECLR_PROFILER_PATH_64' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'MicrosoftInstrumentationEngine_IsPreinstalled' -Operation 'prepend' -Transform 'RemoveAll'
        Get-EnvironmentVariableAddInfo -EnvironmentVariable 'MicrosoftInstrumentationEngine_LogLevel' -Operation 'prepend' -Transform 'RemoveAll'

        if ($DebugWait) {
            # Append insert environment variable
            # <add name="MicrosoftInstrumentationEngine_DebugWait" value="1" xdt:Locator="Match(name)" xdt:Transform="InsertIfMissing" />
            Get-EnvironmentVariableAddInfo -EnvironmentVariable 'MicrosoftInstrumentationEngine_DebugWait' -Value '1' -Operation 'append' -Transform 'InsertIfMissing'
        }

        if ($DisableSignatureValidation) {
            # Append insert environment variable
            # <add name="MicrosoftInstrumentationEngine_DisableCodeSignatureValidation" value="1" xdt:Locator="Match(name)" xdt:Transform="InsertIfMissing" />
            Get-EnvironmentVariableAddInfo -EnvironmentVariable 'MicrosoftInstrumentationEngine_DisableCodeSignatureValidation' -Value '1' -Operation 'append' -Transform 'InsertIfMissing'
        }

        # Prepend Import XdtExtensions
        # <xdt:Import path="%XDT_EXTENSIONPATH%\XdtExtensions\XdtExtensions.dll" namespace="Microsoft.InstrumentationEngine.XdtExtensions" />
        Write-Output @{
            destination = '/configuration'; operation = 'prepend'; element = 'xdt:Import'
            attributes = [ordered] @{
                path='%XDT_EXTENSIONPATH%\XdtExtensions\XdtExtensions.dll'; namespace='Microsoft.InstrumentationEngine.XdtExtensions'
            }
        }

        # Update Nodes
        # Change xdt:Transform="InsertIfMissing" to xdt:Transform="InsertIfMissingAndXPathAny(EXPRESSION)"
        <#
            COR_ENABLE_PROFILING
            COR_PROFILER
            COR_PROFILER_PATH_32
            COR_PROFILER_PATH_64
            CORECLR_ENABLE_PROFILING
            CORECLR_PROFILER
            CORECLR_PROFILER_PATH_32
            CORECLR_PROFILER_PATH_64
            MicrosoftInstrumentationEngine_IsPreinstalled
            MicrosoftInstrumentationEngine_LogLevel
        #>
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'COR_ENABLE_PROFILING'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'COR_PROFILER'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'COR_PROFILER_PATH_32'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'COR_PROFILER_PATH_64'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'CORECLR_ENABLE_PROFILING'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'CORECLR_PROFILER'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'CORECLR_PROFILER_PATH_32'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'CORECLR_PROFILER_PATH_64'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'MicrosoftInstrumentationEngine_IsPreinstalled'
        Get-EnvironmentVariableUpdateInfo -EnvironmentVariable 'MicrosoftInstrumentationEngine_LogLevel'
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
        $XmlEntryOperations
    )

    foreach ($entry in $XmlEntryOperations) {
        if (!$entry.operation) {
            Write-Error "$entry is missing an operation."
        }

        if ($entry.operation -ieq 'prepend' -or
            $entry.operation -ieq 'append') {

            # Create element
            if ($entry.element.StartsWith('xdt:', [System.StringComparison]::OrdinalIgnoreCase)) {
                $elem = $XdtFile.CreateElement($entry.element, 'http://schemas.microsoft.com/XML-Document-Transform')
            } else {
                $elem = $XdtFile.CreateElement($entry.element)
            }

            # Add [ordered] attributes
            foreach ($key in $entry.attributes.Keys)
            {
                if ($key.StartsWith('xdt:', [System.StringComparison]::OrdinalIgnoreCase)) {
                    $attribute = $XdtFile.CreateAttribute($key.SubString(4), "http://schemas.microsoft.com/XML-Document-Transform")
                } else {
                    $attribute = $XdtFile.CreateAttribute($key)
                }

                $attribute.Value = $entry.attributes[$key]
                $elem.Attributes.Append($attribute) | Out-Null
            }

            # Insert element
            $parentNode = $xdtFile.SelectSingleNode($entry.destination)
            if ($entry.operation -ieq 'prepend') {
                $parentNode.PrependChild($elem) | Out-Null
            } elseif ($entry.operation -ieq 'append') {
                $parentNode.AppendChild($elem) | Out-Null
            }

        } elseif ($entry.operation -ieq 'update') {

            # Get element to update
            # This is necessary to resolve xdt namespace for attributes in XPath
            $nsmgr = [System.Xml.XmlNamespaceManager]::new($XdtFile.NameTable)
            $nsmgr.AddNamespace("xdt", "http://schemas.microsoft.com/XML-Document-Transform")
            $elem = $XdtFile.DocumentElement.SelectSingleNode($entry.destination, $nsmgr)

            # Update attributes
            foreach ($key in $entry.attributes.Keys)
            {
                if ($key.StartsWith('xdt:', [System.StringComparison]::OrdinalIgnoreCase)) {
                    $elem.SetAttribute($key.SubString(4), "http://schemas.microsoft.com/XML-Document-Transform", $entry.attributes[$key]) | Out-Null
                } else {
                    $elem.SetAttribute($key, $entry.attributes[$key]) | Out-Null
                }
            }

        } else {
            Write-Error "Invalid Entry Operation: $($entry.operation)"
        }
    }
}