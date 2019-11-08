# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Creates a modified xdt file that overrides the preinstalled site extension.

.EXAMPLE
    .\GeneratePrivateXdt.ps1 -InputXdtFilePath 'C:\in\applicationHost.xdt' -OutputXdtFilePath 'C:\out\applicationHost.xdt'
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [String]
    $OutputXdtFilePath,

    [Parameter(Mandatory=$false)]
    [ValidateNotNullOrEmpty()]
    [ValidateScript({ Test-Path $_ })]
    [String]
    $InputXdtFilePath = "$PSScriptRoot\..\..\..\src\InstrumentationEngine.Preinstall\applicationHost.xdt",

    [Parameter(Mandatory=$false)]
    [Switch]
    $DebugWait,

    [Parameter(Mandatory=$false)]
    [Switch]
    $DisableSignatureValidation
)

Import-Module "$PSScriptRoot\XdtHelper.psm1" -Force
Add-Type -AssemblyName 'System.Xml.Linq'

$ErrorActionPreference = 'Stop'

[xml]$xdtFile = Get-Content $InputXdtFilePath -Raw

Write-Host "Applying changes..."
$params = @{
    DisableSignatureValidation = $DisableSignatureValidation
    DebugWait                  = $DebugWait
}
Edit-XdtContent -XdtFile $xdtFile -XmlEntries (Get-ClrieXmlEntries @params)

Write-Host "Saving changes..."
# This nicely formats the XML
[System.Xml.Linq.XDocument] $doc = [System.Xml.Linq.XDocument]::Parse($xdtFile.OuterXml)
Set-Content -Value $doc.ToString() -Path $OutputXdtFilePath -Force