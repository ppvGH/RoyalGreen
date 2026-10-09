<#
.SYNOPSIS
Build and run the sac4ap checks without changing the application's CMake project.
.EXAMPLE
.\scripts\run_checks.ps1 -Configuration Debug -BuildApplication
#>
[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Debug',
    [switch]$BuildApplication,
    [ValidateRange(1, 64)][int]$Jobs = 4,
    [string]$Python = 'python'
)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Invoke-Checked {
    param([string]$Tool, [string[]]$ToolArguments)
    & $Tool @ToolArguments
    if ($LASTEXITCODE -ne 0) { throw "$Tool failed with exit code $LASTEXITCODE" }
}

$taskRoot = Split-Path -Parent $PSScriptRoot
$taskPythonPath = (Get-Command $Python -ErrorAction Stop).Source
$taskVersionOutput = & cmake --version
if ($LASTEXITCODE -ne 0) { throw 'CMake is required.' }
$taskVersion = [version]([regex]::Match($taskVersionOutput[0], '\d+\.\d+\.\d+').Value)
if ($taskVersion -lt [version]'3.24' -or $taskVersion -ge [version]'4.0') {
    throw 'Use CMake >= 3.24 and < 4.0 for the unchanged application dependencies (verified: 3.31.6).'
}
Push-Location -LiteralPath $taskRoot
try {
    Invoke-Checked 'cmake' @('-S', 'tests', '-B', 'build/tests', '-G', 'Visual Studio 17 2022', '-A', 'x64', "-DPython3_EXECUTABLE=$taskPythonPath")
    Invoke-Checked 'cmake' @('--build', 'build/tests', '--config', $Configuration, '--parallel', "$Jobs")
    $taskResults = Join-Path $taskRoot "build/tests/test-results-$Configuration.xml"
    Invoke-Checked 'ctest' @('--test-dir', 'build/tests', '-C', $Configuration, '--output-on-failure', '--parallel', "$Jobs", '--output-junit', $taskResults)
    if ($BuildApplication) {
        Invoke-Checked 'cmake' @('-S', '.', '-B', 'build/app', '-G', 'Visual Studio 17 2022', '-A', 'x64')
        Invoke-Checked 'cmake' @('--build', 'build/app', '--config', $Configuration, '--parallel', "$Jobs")
    }
    Write-Output "All $Configuration checks passed. See docs/manual-testing.md for the interactive graphics checks."
} finally {
    Pop-Location
}