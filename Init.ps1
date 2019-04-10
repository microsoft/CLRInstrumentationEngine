<#
.Synopsis
    Create a pre-commit hook if one does not already exist.
    This hook is generic and it invokes the real hook script at scripts\hooks\pre-commit.ps1
#>

$preCommitPath = "$PSScriptRoot\.git\hooks\pre-commit"

if (!(Test-Path "$preCommitPath")) {
    Copy-Item -Path "$PSScriptRoot\Scripts\hooks\pre-commit" -Destination "$preCommitPath"
}