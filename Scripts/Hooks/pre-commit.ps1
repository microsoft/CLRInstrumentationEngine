$VerbosePreference = 'Continue'

$ScriptsRoot = Resolve-Path -Path "$PSScriptRoot\.."

try
{
    # Update guid.[VERSION].json file
    & "$ScriptsRoot\UpdateGuids.ps1" -StageChanges
}
catch
{
    # Catch exception and write to host, but don't fail commit operation
    Write-Host "Exception occured executing pre-commit script UpdateGuids.ps1: $_"
}

try
{
    # Update wxs files
    & "$ScriptsRoot\UpdateWxs.ps1" -StageChanges
}
catch
{
    # Catch exception and write to host, but don't fail commit operation
    Write-Host "Exception occured executing pre-commit script UpdateWxs.ps1: $_"
}

exit 0