function Get-RepositoryPaths {
    [CmdletBinding()]
    param()

    $enlistmentRoot = Resolve-Path -Path "$PsScriptRoot\.."

    Write-Output @{
        "EnlistmentRoot"        = $enlistmentRoot
        "BuildFolder"           = Resolve-Path -Path "$enlistmentRoot\build"
        "TemplatesFolder"       = Resolve-Path -Path "$PsScriptRoot\Templates"
        "GuidsFolder"           = Resolve-Path -Path "$enlistmentRoot\src\InstrumentationEngine.Installer\Guids"
        "EngineInstallerFolder" = Resolve-Path -Path "$enlistmentRoot\src\InstrumentationEngine.Installer"
    }
}

function Get-FormattedGuid {
    [CmdletBinding()]
    param()

    Write-Output "{$((New-Guid).Guid.ToString().ToUpper())}"
}

function Get-ReleaseVersion
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true)]
        [ValidateNotNullOrEmpty()]
        [String]
        $VersionPropsPath
    )

    if (!(Test-Path $VersionPropsPath))
    {
        Write-Error "Unable to find Version.Props file at $VersionPropsPath."
    }

    [xml] $versionPropsFile = Get-Content $VersionPropsPath

    $major = "$($versionPropsFile.Project.PropertyGroup.SemanticVersionMajor)".Trim()
    $minor = "$($versionPropsFile.Project.PropertyGroup.SemanticVersionMinor)".Trim()
    $patch = "$($versionPropsFile.Project.PropertyGroup.SemanticVersionPatch)".Trim()

    Write-Output "$major.$minor.$patch"
}

function Set-StringTokens
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true)]
        [ValidateNotNullOrEmpty()]
        [string]
        $InputString,

        [Parameter()]
        [hashtable]
        $TokenDictionary
    )

    $outString = $InputString
    $TokenDictionary.Keys |
        ForEach-Object {
            $outString = $outString.Replace($_, $TokenDictionary[$_])
        }

    Write-Output $outString
}

function Format-XML
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true, ValueFromPipeline)]
        [ValidateNotNullOrEmpty()]
        [xml]
        $Xml,

        [Parameter()]
        [int]
        $Indent=4
    )

    $StringWriter = New-Object System.IO.StringWriter
    $XmlWriter = New-Object System.XMl.XmlTextWriter $StringWriter
    $xmlWriter.Formatting = "indented"
    $xmlWriter.Indentation = $Indent
    $xml.WriteTo($XmlWriter)
    $XmlWriter.Flush()
    $StringWriter.Flush()

    Write-Output $StringWriter.ToString()
}

function Add-FileToCommit
{
    param(
        [Parameter(Mandatory=$true)]
        [ValidateNotNullOrEmpty()]
        [string]
        $path
    )

    & git add $Path
}