# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

$VerbosePreference = 'Continue'
$ErrorActionPreference = 'Stop'

$copyrightHeader  = 'Copyright (c) Microsoft Corporation. All rights reserved.'
$mitLicenseHeader = 'Licensed under the MIT License.'
$xmlHeader        = '<?xml'
$shellHeader        = '#!/bin/bash'

$resultFile = "verifyCopyrightHeadersResult.log"
Write-Verbose "Generating $resultFile" 
New-Item $resultFile -Force | Out-Null

$excludedDirs =  @(
    # generated
    '.vs'
    '.vscode'
    'bin'
    'Debug'
    'obj'
    'package'
    'Release'

    # external
    'atl'
    'empty'
    'winsdk'

    # test-related
    'InstrEngineTests\\Baselines'
    'InstrEngineTests\\TestScripts'
)

$excludedExtensions = @(
    '.filters'
    '.htm'
    '.log'
    '.md'
    '.rgs'
    '.sln'
    '.snk'
    '.txt'
    '.user'
)

$excludedFiles = @(
    # external
    'corprof.h'
    'dbgmsg.h'

    # used by preinstalled site extension on Azure App Service
    'extension.xml'

    # generated
    'HostExtension_i.c'
    'HostExtension_i.h'
    'InstrumentationEngine.h'
    'InstrumentationEngine_api.cpp'
    'RawProfilerHook_i.c'
    'RawProfilerHook_i.h'

    # the license itself
    'LICENSE'
)

Write-Verbose "OSS Headers require the following lines near the top of the file:"
Write-Verbose ""
Write-Verbose "    $copyrightHeader"
Write-Verbose "    $mitLicenseHeader"
Write-Verbose ""

Write-Verbose "Checking files under directory '$PSScriptRoot' ..."
Write-Verbose "Non-compliant files will be stored in $resultFile"

Get-ChildItem -Path $PSScriptRoot -Recurse -File | 
    Where-Object { $_.PSParentPath -notmatch "\\$([String]::Join('|', $excludedDirs))" } | 
    Where-Object { !$excludedFiles.Contains($_.Name) } |
    Where-Object { !$excludedExtensions.Contains($_.Extension) } | 
    ForEach-Object { 
        $lines = Get-Content $_.FullName -First 3
        $copyrightIndex = 0
        $mitLicenseIndex = 1
        if ($lines)
        {
            if ($lines[0].ToString().Contains($xmlHeader) -or 
                $lines[0].ToString().Contains($shellHeader))
            {
                $copyrightIndex = 1
                $mitLicenseIndex = 2
            }

            if (!$lines[$copyrightIndex].ToString().Contains($copyrightHeader) -or
                !$lines[$mitLicenseIndex].ToString().Contains($mitLicenseHeader))
            {
                "Could not find OSS header in $($_.FullName)" > $resultFile
            }
        }
        else
        {
            $msg = "$($_.FullName) is empty. Consider modifying this script to exclude the file."
            Write-Warning $msg > $resultFile
        }
    }

if (Get-Content $resultFile)
{
    Write-Error "See '$resultFile' for the list of files to fix."
}
else
{
    Write-Verbose "Done."
}