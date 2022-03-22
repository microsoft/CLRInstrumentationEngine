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
    '.png'
    '.json'
    '.filters'
    '.htm'
    '.log'
    '.md'
    '.rgs'
    '.sln'
    '.snk'
    '.txt'
    '.user'
    '.ignore'
)

$excludedFiles = @(
    # external
    'corprof.h'
    'dbgmsg.h'

    # json/txt files
    'baseline.gdnbaselines'
    'csharpsuppressions.txt'
    'suppress.gdnsuppress'

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
    'EngineConfiguration.cs'
    'InstrumentationConfigurationSources.cs'

    # the license itself
    'LICENSE'
)

Write-Verbose "OSS Headers require the following lines near the top of the file:"
Write-Verbose ""
Write-Verbose "    $copyrightHeader"
Write-Verbose "    $mitLicenseHeader"
Write-Verbose ""

Write-Verbose "Checking files under directory '$PSScriptRoot' ..."

Get-ChildItem -Path $PSScriptRoot -Recurse -File |
    Where-Object { $_.PSParentPath -notmatch "\\$([String]::Join('|', $excludedDirs))" } |
    Where-Object { -not $excludedFiles.Contains($_.Name) } |
    Where-Object { -not $excludedExtensions.Contains($_.Extension) } |
    ForEach-Object {
        # Assume the Copyright headers are within 5 lines of the file.
        $lines = Get-Content $_.FullName -First 5
        if ($lines)
        {
            # Skip any whitespace at the beginning of the file.
            $i = 0
            while ([string]::IsNullOrWhiteSpace($lines[$i]) -and ($i -lt $lines.Length - 2)) { $i++ }

            $copyrightIndex = $i
            $mitLicenseIndex = $i + 1
            if ($lines[$i].ToString().Contains($xmlHeader) -or
                $lines[$i].ToString().Contains($shellHeader))
            {
                $copyrightIndex = $i + 1
                $mitLicenseIndex = $i + 2
            }

            if (!$lines[$copyrightIndex].ToString().Contains($copyrightHeader) -or
                !$lines[$mitLicenseIndex].ToString().Contains($mitLicenseHeader))
            {
                Write-Warning "$($_.FullName) is missing OSS header"
            }
        }
        else
        {
            Write-Warning "$($_.FullName) is empty."
        }
    }

Write-Verbose "Done."