param(
  [Parameter(Mandatory = $true)][string]$Binary
)

$ErrorActionPreference = "Stop"
if (-not (Test-Path -LiteralPath $Binary -PathType Leaf)) {
  throw "binary not found: $Binary"
}
$dumpbin = Get-Command dumpbin.exe -ErrorAction SilentlyContinue
$dumpbinPath = if ($dumpbin) { $dumpbin.Path } else { $null }
if (-not $dumpbinPath -and $dumpbin) { $dumpbinPath = $dumpbin.Source }

if (-not $dumpbinPath) {
  $programFilesRoots = @($env:ProgramFiles, ${env:ProgramFiles(x86)}) |
    Where-Object { $_ }
  $vswherePath = $programFilesRoots |
    ForEach-Object { Join-Path $_ "Microsoft Visual Studio\Installer\vswhere.exe" } |
    Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } |
    Select-Object -First 1

  if ($vswherePath) {
    $dumpbinPath = & $vswherePath `
      -latest `
      -products * `
      -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
      -find "VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe" 2>$null |
      Where-Object { $_ -and (Test-Path -LiteralPath $_ -PathType Leaf) } |
      Select-Object -First 1
  }
}

if (-not $dumpbinPath) {
  throw "dumpbin.exe was not found. Install the Visual Studio C++ tools (Microsoft.VisualStudio.Component.VC.Tools.x86.x64)."
}
Write-Output "Using dumpbin: $dumpbinPath"
$output = & $dumpbinPath /DEPENDENTS $Binary
if ($LASTEXITCODE -ne 0) { throw "dumpbin failed with exit code $LASTEXITCODE" }
$forbidden = $output | Select-String -Pattern '(?i)\b(libcurl|libxml2|pugixml|libssl|libcrypto|zlib|zstd|brotli|libidn|libpsl|libssh|vcruntime|msvcp)[^\s]*\.dll\b'
if ($forbidden) {
  $forbidden | ForEach-Object { Write-Error $_.Line }
  throw "third-party or dynamic MSVC runtime dependency detected"
}
Write-Output $output
