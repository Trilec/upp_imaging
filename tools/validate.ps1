param(
 [string]$Umk = 'E:/upp-18468/umk.exe',
 [string]$Assembly = 'GitHubOut',
 [string]$Method = 'CLANGx64',
 [string[]]$Configuration = @('debug','release'),
 [string[]]$Package = @(),
 [switch]$Rebuild
)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot
Set-Location $root
$out=Join-Path $root 'out/validation'
New-Item -ItemType Directory -Path $out -Force | Out-Null
if(!$Package.Count) { $Package=Get-Content (Join-Path $root 'tests/acceptance.txt') | Where-Object {$_ -and !$_.StartsWith('#')} }
$results=@()
foreach($cfg in $Configuration) {
 foreach($t in $Package) {
  $flags=if($cfg -eq 'release') {'-rH8'} else {'-H8'}
  if($Rebuild -and $t -eq $Package[0]) { $flags=$flags.Replace('-', '-a') }
  $base=Join-Path $out "$t-$cfg"
  & $Umk $Assembly $t $Method $flags "$base.exe" > "$base-build.log" 2>&1
  if($LASTEXITCODE -ne 0) { Get-Content "$base-build.log" -Tail 25; throw "BUILD FAILED: $t $cfg" }
  $proc=Start-Process -FilePath "$base.exe" -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$base-run.log" -RedirectStandardError "$base-stderr.log"
  if(!$proc.WaitForExit(120000)) { $proc.Kill(); throw "TIMEOUT: $t $cfg" }
  $proc.Refresh()
  $summary=(Select-String -Path "$base-run.log" -Pattern '^SUMMARY passed=\d+ failed=\d+' | ForEach-Object {$_.Line}) -join ' '
  $line="$t $cfg exit=$($proc.ExitCode) $summary"
  Write-Output $line
  $results+=$line
  $results | Set-Content (Join-Path $out 'results.txt')
  if($proc.ExitCode -ne 0 -or !$summary -or $summary -notmatch 'failed=0') {throw "TEST FAILED: $t $cfg"}
 }
}
