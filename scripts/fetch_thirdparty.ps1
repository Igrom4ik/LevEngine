<#
PowerShell script to download and extract GLFW 3.4 and GLEW 2.2.0 releases from GitHub
Usage (from repository root):
  .\scripts\fetch_thirdparty.ps1          # downloads into thirdparty/ (skips if exists)
  .\scripts\fetch_thirdparty.ps1 -Force  # force re-download and overwrite
#>
param(
    [switch]$Force
)

$repoRoot = Resolve-Path -Path (Join-Path $PSScriptRoot "..")
$thirdDir = Join-Path $repoRoot "thirdparty"
if (-not (Test-Path $thirdDir)) { New-Item -ItemType Directory -Path $thirdDir | Out-Null }

function DownloadAndExtract($url, $destDir) {
    Write-Host "Downloading $url ..."
    $tmpZip = Join-Path $env:TEMP ([IO.Path]::GetRandomFileName() + ".zip")
    try {
        Invoke-WebRequest -Uri $url -OutFile $tmpZip -UseBasicParsing -ErrorAction Stop
    } catch {
        Write-Error "Failed to download $url : $_"
        return $false
    }

    $tmpExtract = Join-Path $env:TEMP ([IO.Path]::GetRandomFileName())
    Expand-Archive -Path $tmpZip -DestinationPath $tmpExtract -Force

    $entries = Get-ChildItem -Path $tmpExtract -Directory
    if ($entries.Count -eq 1) { $src = $entries[0].FullName } else { $src = $tmpExtract }

    if (Test-Path $destDir) {
        if ($Force) {
            Remove-Item -Recurse -Force $destDir
        } else {
            Write-Host "$destDir already exists — skipping (use -Force to overwrite)"
            Remove-Item -Force $tmpZip -ErrorAction SilentlyContinue
            Remove-Item -Recurse -Force $tmpExtract -ErrorAction SilentlyContinue
            return $true
        }
    }

    try {
        Move-Item -Path $src -Destination $destDir
        Write-Host "Extracted to $destDir"
        Remove-Item -Force $tmpZip -ErrorAction SilentlyContinue
        if ($src -ne $tmpExtract) { Remove-Item -Recurse -Force $tmpExtract -ErrorAction SilentlyContinue }
        return $true
    } catch {
        Write-Error "Failed to move extracted files: $_"
        return $false
    }
}

# GitHub archive URLs (tags)
$glewUrl = 'https://github.com/nigels-com/glew/archive/refs/tags/glew-2.2.0.zip'
$glfwUrl  = 'https://github.com/glfw/glfw/archive/refs/tags/3.4.zip'

$ok = $true
$ok = $ok -and (DownloadAndExtract $glewUrl (Join-Path $thirdDir 'glew'))
$ok = $ok -and (DownloadAndExtract $glfwUrl  (Join-Path $thirdDir 'glfw-3.4'))

if ($ok) {
    Write-Host "All done. You can now run:"
    Write-Host "  cmake -S . -B build"
    Write-Host "  cmake --build build --config Debug"
} else {
    Write-Error "One or more downloads failed. See messages above."
    exit 1
}
