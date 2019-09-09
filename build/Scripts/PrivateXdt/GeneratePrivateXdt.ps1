<#
    Copyright (c) Microsoft Corporation. All rights reserved.
    Licensed under the MIT License.
#>

<#
.SYNOPSIS
    Creates an xdt file from

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

$ErrorActionPreference = 'Stop'

[xml]$xdtFile = Get-Content $InputXdtFilePath -Raw

Write-Host "Applying changes..."
$params = @{
    DisableSignatureValidation = $DisableSignatureValidation
    DebugWait                  = $DebugWait
}
Edit-XdtContent -XdtFile $xdtFile -XmlEntries (Get-ClrieXmlEntries @params)

Write-Host "Saving changes..."
Set-Content -Value $xdtFile.OuterXml -Path $OutputXdtFilePath -Force