<#
.SYNOPSIS
Optional line coverage for the C++ modules linked into the Debug unit test binary.
.DESCRIPTION
Requires a separately installed OpenCppCoverage 0.9.9.0 x64. This legacy tool is
archived upstream; it is optional and not installed by the build or this script.
Coverage excludes unlinked production modules and is not whole-application coverage.
#>
[CmdletBinding()]
param([string]$OpenCppCoverage = 'OpenCppCoverage.exe')
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$taskRoot = Split-Path -Parent $PSScriptRoot
$taskTool = (Get-Command $OpenCppCoverage -ErrorAction Stop).Source
$taskBinary = Join-Path $taskRoot 'build/tests/Debug/royalgreen_tests.exe'
if (-not (Test-Path -LiteralPath $taskBinary)) { throw 'Run scripts/run_checks.ps1 -Configuration Debug first.' }
$taskCoverage = Join-Path $taskRoot 'coverage'
New-Item -ItemType Directory -Force -Path $taskCoverage | Out-Null
$taskSources = Join-Path $taskRoot 'src'
$taskHtml = Join-Path $taskCoverage 'cpp-html'
$taskXml = Join-Path $taskCoverage 'cpp-cobertura.xml'
& $taskTool '--modules' 'royalgreen_tests.exe' '--sources' $taskSources '--export_type' "html:$taskHtml" '--export_type' "cobertura:$taskXml" '--' $taskBinary
if ($LASTEXITCODE -ne 0) { throw "Coverage run failed with exit code $LASTEXITCODE" }
Write-Output "Reports: $taskHtml and $taskXml. Interpret these as coverage of linked modules only."