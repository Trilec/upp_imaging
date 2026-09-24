param(
 [string]$Umk = 'E:/upp-18468/umk.exe',
 [string]$Assembly = 'GitHubOut',
 [string]$Method = 'CLANGx64',
 [string[]]$Configuration = @('debug','release'),
 [string[]]$Package = @(),
 [switch]$Rebuild
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot
Set-Location $root
$sourceSha = (git rev-parse HEAD).Trim()
$out = Join-Path $root 'build/windows-x64/validation'
New-Item -ItemType Directory -Path $out -Force | Out-Null
$env:UPP_IMAGING_TEST_RUNTIME_DIR = Join-Path $root 'build/windows-x64/runtime'
New-Item -ItemType Directory -Path $env:UPP_IMAGING_TEST_RUNTIME_DIR -Force | Out-Null
$resultsPath = Join-Path $out 'results.txt'
$expected = @{}
foreach ($line in Get-Content (Join-Path $root 'tests/expected_counts.txt')) {
 if ($line -match '^\s*(?:#|$)') { continue }
 if ($line -notmatch '^([a-z0-9_]+) ([1-9][0-9]*)$') { throw "Malformed expected count: $line" }
 if ($expected.ContainsKey($Matches[1])) { throw "Duplicate expected count: $($Matches[1])" }
 $expected[$Matches[1]] = [int]$Matches[2]
}
$accepted = @(Get-Content (Join-Path $root 'tests/acceptance.txt') | Where-Object { $_ -and !$_.StartsWith('#') })
if ($accepted.Count -ne $expected.Count) { throw 'Acceptance and expected-count manifests differ' }
foreach ($name in $accepted) {
 if (!$expected.ContainsKey($name)) { throw "Missing expected count: $name" }
}
if (!$Package.Count) { $Package = $accepted }
if (!$Configuration.Count) { throw 'No configurations selected' }
foreach ($cfg in $Configuration) {
 if ($cfg -cnotin @('debug','release')) { throw "Invalid configuration: $cfg" }
}
foreach ($name in $Package) {
 if ($name -cnotin $accepted) { throw "Unknown acceptance package: $name" }
}
@("source=$sourceSha method=$Method assembly=$Assembly umk=$Umk", "configurations=$($Configuration -join ',') packages=$($Package -join ',')") |
 Set-Content $resultsPath
foreach ($cfg in $Configuration) {
 $configOut = Join-Path $out $cfg
 New-Item -ItemType Directory -Path $configOut -Force | Out-Null
 foreach ($name in $Package) {
  $flags = if ($cfg -ceq 'release') { '-rH8' } else { '-H8' }
  if ($Rebuild -and $name -ceq $Package[0]) { $flags = $flags.Replace('-', '-a') }
  $base = Join-Path $configOut $name
  $exe = "$base.exe"
  $buildLog = "$base-build.log"
  $runLog = "$base-run.log"
  $stderrLog = "$base-stderr.log"
  if (Test-Path $exe) { Remove-Item -LiteralPath $exe -Force }
  & $Umk $Assembly $name $Method $flags $exe > $buildLog 2>&1
  $buildExit = $LASTEXITCODE
  if ($buildExit -ne 0 -or !(Test-Path $exe)) {
   "FAIL package=$name configuration=$cfg stage=build exit=$buildExit log=$buildLog" | Add-Content $resultsPath
   Get-Content $buildLog -Tail 25
   throw "BUILD FAILED: $name $cfg"
  }
  $proc = Start-Process -FilePath $exe -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput $runLog -RedirectStandardError $stderrLog
  if (!$proc.WaitForExit(120000)) {
   $proc.Kill()
   $proc.WaitForExit()
   "FAIL package=$name configuration=$cfg stage=timeout runlog=$runLog stderr=$stderrLog" | Add-Content $resultsPath
   throw "TIMEOUT: $name $cfg"
  }
  $proc.Refresh()
  $summaries = @(Get-Content $runLog | Where-Object { $_ -match '^SUMMARY\b' })
  $valid = $summaries.Count -eq 1 -and $summaries[0] -cmatch '^SUMMARY passed=([0-9]+) failed=([0-9]+)$'
  if ($valid) {
   $passed = [int]$Matches[1]
   $failed = [int]$Matches[2]
  }
  if (!$valid -or $proc.ExitCode -ne 0 -or $failed -ne 0 -or $passed -lt $expected[$name]) {
   "FAIL package=$name configuration=$cfg stage=run exit=$($proc.ExitCode) summary=$($summaries -join ';') minimum=$($expected[$name]) runlog=$runLog stderr=$stderrLog" | Add-Content $resultsPath
   throw "TEST FAILED: $name $cfg"
  }
  $line = "PASS package=$name configuration=$cfg passed=$passed failed=$failed exit=$($proc.ExitCode) minimum=$($expected[$name]) buildlog=$buildLog runlog=$runLog stderr=$stderrLog"
  Write-Output $line
  $line | Add-Content $resultsPath
 }
}
