param(
    [string]$InstallDir = "${PWD}/cmake-build-release/install",
    [string]$OutDir = "${PWD}/cmake-build-release/package",
    [string]$Name = "LevEngine-1.0.0-Windows.zip"
)

Write-Host "Packing clean archive from: $InstallDir"
if (-Not (Test-Path $InstallDir)) { Write-Error "Install dir not found: $InstallDir"; exit 2 }
if (-Not (Test-Path $OutDir)) { New-Item -ItemType Directory -Path $OutDir | Out-Null }
$zipPath = Join-Path $OutDir $Name
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }

# Create temp staging and copy only desired dirs
$staging = Join-Path $env:TEMP ([System.Guid]::NewGuid().ToString())
New-Item -ItemType Directory -Path $staging | Out-Null
try {
    Copy-Item -Path (Join-Path $InstallDir 'bin') -Destination $staging -Recurse -Force -ErrorAction Stop
    Copy-Item -Path (Join-Path $InstallDir 'lib') -Destination $staging -Recurse -Force -ErrorAction Stop
    if (Test-Path (Join-Path $InstallDir 'README.md')) {
        Copy-Item -Path (Join-Path $InstallDir 'README.md') -Destination $staging -Force -ErrorAction Stop
    }

    Write-Host "Creating zip: $zipPath"
    Compress-Archive -Path (Join-Path $staging '*') -DestinationPath $zipPath -Force
    Write-Host "Created: $zipPath"
}
finally {
    Remove-Item -Recurse -Force $staging
}

Write-Host "Pack clean done."; exit 0

