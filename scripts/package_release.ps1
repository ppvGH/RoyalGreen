<#
.SYNOPSIS
Build and verify a portable Windows x64 ZIP. Does not publish a GitHub release.
.EXAMPLE
.\scripts\package_release.ps1
#>
[CmdletBinding()]
param([ValidateRange(1,64)][int]$Jobs=4,[string]$Python='python')
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
function Invoke-Checked {
    param([string]$Tool,[string[]]$ToolArguments)
    & $Tool @ToolArguments
    if($LASTEXITCODE -ne 0) { throw "$Tool failed with exit code $LASTEXITCODE" }
}
$taskRoot=Split-Path -Parent $PSScriptRoot
$taskPythonPath=(Get-Command $Python -ErrorAction Stop).Source
Push-Location -LiteralPath $taskRoot
try {
    Invoke-Checked 'cmake' @('-S','.','-B','build/app','-G','Visual Studio 17 2022','-A','x64','-DROYALGREEN_STATIC_RUNTIME=ON')
    Invoke-Checked 'cmake' @('--build','build/app','--config','Release','--parallel',"$Jobs")
    Invoke-Checked 'cpack' @('--config','build/app/CPackConfig.cmake','-C','Release','-B','build/packages')
    $taskPackage=Get-ChildItem -LiteralPath 'build/packages' -Filter 'RoyalGreen-*-Windows-*.zip' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if(!$taskPackage) { throw 'No Windows archive produced.' }
    Invoke-Checked $taskPythonPath @('scripts/verify_package.py',$taskPackage.FullName)
    Write-Output "Package: $($taskPackage.FullName)"
} finally { Pop-Location }
