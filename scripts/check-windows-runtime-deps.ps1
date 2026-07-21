param(
  [Parameter(Mandatory = $true)][string]$Binary
)

$ErrorActionPreference = "Stop"
if (-not (Test-Path -LiteralPath $Binary -PathType Leaf)) {
  throw "binary not found: $Binary"
}
$dumpbin = Get-Command dumpbin.exe -ErrorAction SilentlyContinue
if (-not $dumpbin) {
  $dumpbin = Get-ChildItem "${env:ProgramFiles}\Microsoft Visual Studio\2022" `
    -Filter dumpbin.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
}
if (-not $dumpbin) { throw "dumpbin.exe was not found" }
$dumpbinPath = if ($dumpbin.Source) { $dumpbin.Source } else { $dumpbin.FullName }
$output = & $dumpbinPath /DEPENDENTS $Binary
if ($LASTEXITCODE -ne 0) { throw "dumpbin failed with exit code $LASTEXITCODE" }
$forbidden = $output | Select-String -Pattern '(?i)\b(libcurl|libxml2|pugixml|libssl|libcrypto|zlib|zstd|brotli|libidn|libpsl|libssh|vcruntime|msvcp)[^\s]*\.dll\b'
if ($forbidden) {
  $forbidden | ForEach-Object { Write-Error $_.Line }
  throw "third-party or dynamic MSVC runtime dependency detected"
}
Write-Output $output
