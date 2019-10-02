# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

$VerbosePreference = 'Continue'
$ErrorActionPreference = 'Stop'

$copyrightHeader  = 'Copyright (c) Microsoft Corporation. All rights reserved.'
$mitLicenseHeader = 'Licensed under the MIT License.'
$xmlHeader        = '<?xml'
$shellHeader        = '#!/bin/bash'

$excludedDirs =  @(
    # generated
    '.vs'
    '.vscode'
    'bin'
    'Debug'
    'obj'
    'package'
    'Release'
    'out'

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
    'ExtensionsHost_i.c'
    'ExtensionsHost_i.h'
    'ExtensionsHost_p.c'
    'HostExtension_i.c'
    'HostExtension_i.h'
    'InstrumentationEngine.h'
    'InstrumentationEngine_api.cpp'
    'RawProfilerHook_i.c'
    'RawProfilerHook_i.h'
    'dlldata.c'
    'build.sem'

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
    Where-Object { -not $excludedFiles.Contains($_.Name) } |
    Where-Object { -not $excludedExtensions.Contains($_.Extension) } |
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
                Write-Error "$($_.FullName) is missing OSS header."
            }
        }
        else
        {
            Write-Error "$($_.FullName) is empty."
        }
    }

Write-Verbose "Done."