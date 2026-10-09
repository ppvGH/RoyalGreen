<#
.SYNOPSIS
Opt-in startup/clean-shutdown smoke test on a desktop with OpenGL 3.3.
.DESCRIPTION
Checks renderer initialization and diagnostics. It does not simulate gameplay or
assert visual correctness. Do not run this on a runner without a graphics driver.
#>
[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [ValidateRange(2, 60)][int]$DurationSeconds = 8
)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$taskRoot = Split-Path -Parent $PSScriptRoot
$taskExecutable = Join-Path $taskRoot "build/app/$Configuration/royalgreen.exe"
if (-not (Test-Path -LiteralPath $taskExecutable)) { throw 'Build the application first: see docs/installation.md.' }
$taskLogDirectory = Join-Path $taskRoot 'build/smoke'
New-Item -ItemType Directory -Force -Path $taskLogDirectory | Out-Null
$taskStdout = Join-Path $taskLogDirectory "$Configuration.stdout.log"
$taskStderr = Join-Path $taskLogDirectory "$Configuration.stderr.log"
$taskProcess = Start-Process -FilePath $taskExecutable -WorkingDirectory $taskRoot -WindowStyle Hidden -RedirectStandardOutput $taskStdout -RedirectStandardError $taskStderr -PassThru
try {
    if ($taskProcess.WaitForExit($DurationSeconds * 1000)) {
        throw "Application exited before the smoke interval (exit $($taskProcess.ExitCode)); inspect $taskLogDirectory."
    }
    if (-not $taskProcess.CloseMainWindow()) { throw 'No application window could be closed normally.' }
    if (-not $taskProcess.WaitForExit(5000)) { throw 'Application did not shut down within five seconds.' }
    $taskProcess.Refresh()
    if ($taskProcess.ExitCode -ne 0) { throw "Application shutdown failed (exit $($taskProcess.ExitCode))." }
    $taskOutput = [IO.File]::ReadAllText($taskStdout)
    $taskErrors = [IO.File]::ReadAllText($taskStderr)
    if ($taskOutput -notmatch 'Rendered with: [^\r\n]+') { throw 'Renderer initialization was not reported.' }
    if (($taskOutput + $taskErrors) -match '(?i)ERROR|Failed|ABORT|failure') { throw "Startup reported an error; inspect $taskLogDirectory." }
    Write-Output $taskOutput.Trim()
    Write-Output "PASS: startup and clean shutdown ($Configuration). Visual/gameplay validation remains manual."
} finally {
    if (-not $taskProcess.HasExited) { Stop-Process -Id $taskProcess.Id }
    $taskProcess.Dispose()
}