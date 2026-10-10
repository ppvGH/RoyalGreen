<#
.SYNOPSIS
Extract a verified ZIP and check CLI/failure paths; optionally run real graphics.
.EXAMPLE
.\scripts\check_windows_package.ps1 -Archive build/packages/RoyalGreen-0.2.0-Windows-AMD64.zip -Graphics
#>
[CmdletBinding()]
param([Parameter(Mandatory)][string]$Archive,[switch]$Graphics,[string]$Python='python')
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
$taskRoot=Split-Path -Parent $PSScriptRoot
$taskArchive=(Resolve-Path -LiteralPath $Archive).Path
& $Python (Join-Path $PSScriptRoot 'verify_package.py') $taskArchive
if($LASTEXITCODE -ne 0) { throw 'Package verification failed.' }
$taskDestination=Join-Path $taskRoot ('build/package check '+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $taskDestination | Out-Null
Expand-Archive -LiteralPath $taskArchive -DestinationPath $taskDestination
$taskExe=(Get-ChildItem -LiteralPath $taskDestination -Filter royalgreen.exe -Recurse | Select-Object -First 1).FullName
function Invoke-PackageCheck {
    param([string]$Label,[string[]]$Arguments,[int]$ExpectedExit,[string]$ExpectedText)
    $taskStdout=Join-Path $taskDestination "$Label.stdout.log"
    $taskStderr=Join-Path $taskDestination "$Label.stderr.log"
    $taskProcess=Start-Process -FilePath $taskExe -ArgumentList $Arguments -WorkingDirectory $env:TEMP -WindowStyle Hidden -PassThru -RedirectStandardOutput $taskStdout -RedirectStandardError $taskStderr
    if(!$taskProcess.WaitForExit(30000)) { throw "$Label did not exit within 30 seconds." }
    $taskProcess.Refresh()
    $taskOutput=(Get-Content -LiteralPath $taskStdout -Raw)+(Get-Content -LiteralPath $taskStderr -Raw)
    if($taskProcess.ExitCode -ne $ExpectedExit -or $taskOutput -notlike "*$ExpectedText*") {
        throw "$Label failed (exit $($taskProcess.ExitCode)): $taskOutput"
    }
    Write-Output "PASS: $Label (exit $ExpectedExit)"
}
Invoke-PackageCheck 'help' @('--help') 0 'RoyalGreen'
Invoke-PackageCheck 'missing-assets' @('--assets','RoyalGreen-deliberately-missing-assets') 1 'Asset directory must contain'
if($Graphics) {
    Invoke-PackageCheck 'graphics' @('--graphics-check') 0 'PASS: graphics/resource/lifetime checks'
    Invoke-PackageCheck 'normal-session' @('--width','960','--height','640','--exit-after','2') 0 'Rendered with:'
}
Write-Output "Retained extracted package and logs: $taskDestination"
