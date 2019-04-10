<#
.Synopsis
    Generate a guids.[VERSION].json file if not exist
.DESCRIPTION
    This script should only run when the semanticVersion is updated in build\Version.props file.

    If guids.[VERSION].json does not exist for the new semantic version, this script will read
    in the guidsTemplate.json, string replace tokens, and writes the result to that file.
#>
param(
    [Parameter(Mandatory = $false)]
    [switch] $StageChanges,

    [Parameter(Mandatory = $false)]
    [switch] $ForceUpdate
)

$ErrorActionPreference = 'Stop'
$VerbosePreference = 'Continue'

Import-Module "$PsScriptRoot\Common.psm1" -Force -Verbose:$false

$paths                  = Get-RepositoryPaths
$versionFilePath        = "$($paths.BuildFolder)\Version.Props"
$version                = Get-ReleaseVersion -VersionPropsPath $versionFilePath
$guidFilePath           = "$($paths.GuidsFolder)\guids.$version.json"
$guidsTemplateFilePath  = "$($paths.TemplatesFolder)\guidsTemplate.json"

if ((Test-Path $guidFilePath) -and !$ForceUpdate)
{
    Write-Verbose "$guidFilePath exists. Exiting."
    return;
}

Write-Verbose "Creating $guidFilePath..."
$guidFile = New-Item -Path $guidFilePath -ItemType File -Force
if (!$guidFile -or !$guidFile.Exists)
{
    Write-Error "Unable to create $guidFilePath"
}

Write-Verbose "Populating $guidFilePath..."
Get-Content $guidsTemplateFilePath |
    Foreach-Object {
        $_.Replace('[GUID]', (Get-FormattedGuid)).Replace('[VERSION]', $version) | Out-File $guidFile -Append -Encoding utf8
    }

if ($StageChanges)
{
    Add-FileToCommit $guidFilePath
}