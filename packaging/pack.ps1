<#
Packaging script for HuyEngine (PowerShell)
This script builds Release, installs into a staging folder and then prunes all source files
(.cpp, .h, etc.) so the final zip contains only binaries (bin/, lib/) and README.
Usage (from repo root):
    pwsh -ExecutionPolicy Bypass -File .\packaging\pack.ps1
#>
[CmdletBinding()]
param(
    [string]$Configuration = "Release",
    [string]$Version = $null,
    [string]$BuildDir = ".\cmake-build-release",
    [string]$StagingDir = ".\package\staging",
    [string]$OutputDir = ".\package\output"
)

# If version not provided, try to extract from build/version.txt first
if ( [string]::IsNullOrEmpty($Version))
{
    $possibleVersionFile = Join-Path $root "build\version.txt"
    if (Test-Path $possibleVersionFile)
    {
        Write-Host "Reading version from $possibleVersionFile"
        $Version = (Get-Content -Path $possibleVersionFile -Raw).Trim()
    }
}

# If still not provided, try to extract from top-level CMakeLists.txt (project(... VERSION x.y.z))
if ( [string]::IsNullOrEmpty($Version))
{
    try
    {
        $cmakePath = Join-Path $root "CMakeLists.txt"
        if (Test-Path $cmakePath)
        {
            $cmContent = Get-Content -Path $cmakePath -Raw
            $m = [regex]::Match($cmContent, 'project\s*\(\s*\w+\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)')
            if ($m.Success)
            {
                $Version = $m.Groups[1].Value
                Write-Host "Detected version from CMakeLists.txt: $Version"
            }
        }
    }
    catch
    {
        Write-Host "Warning: failed to detect version from CMakeLists.txt: $_"
    }
}

# Fallback version if still not found
if ( [string]::IsNullOrEmpty($Version))
{
    $Version = "0.0.0"; Write-Host "Using fallback version: $Version"
}

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path ".").Path
$BuildDir = Join-Path $root $BuildDir
$Staging = Join-Path $root $StagingDir
$Output = Join-Path $root $OutputDir

Write-Host "Root: $root"
Write-Host "BuildDir: $BuildDir"
Write-Host "Staging: $Staging"
Write-Host "Output: $Output"
Write-Host "Configuration: $Configuration"

# Prepare directories
if (Test-Path $Staging)
{
    Write-Host "Removing existing staging: $Staging"
    Remove-Item -Recurse -Force $Staging -ErrorAction SilentlyContinue
}
New-Item -ItemType Directory -Path $Staging -Force | Out-Null

if (-not (Test-Path $BuildDir))
{
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
}

if (-not (Test-Path $Output))
{
    New-Item -ItemType Directory -Path $Output -Force | Out-Null
}

# Configure
Write-Host "Configuring CMake..."
& cmake -S $root -B $BuildDir -DCMAKE_BUILD_TYPE=$Configuration -DCMAKE_INSTALL_PREFIX=$Staging

# Build & install
Write-Host "Building and installing (this may take a while)..."
try
{
    & cmake --build $BuildDir --config $Configuration --target install
    $buildExit = $LASTEXITCODE
}
catch
{
    Write-Host "Build command threw an exception: $_"
    $buildExit = 1
}
if ($buildExit -ne 0)
{
    Write-Host "Build failed with exit code $buildExit. Will try to collect existing binaries from known locations."

    # Typical candidate locations to search for binaries
    $candidates = @(
        (Join-Path $root "build"),
        (Join-Path $root "build/App/bin"),
        (Join-Path $root "build/bin"),
        (Join-Path $root "cmake-build-release/bin"),
        (Join-Path $root "cmake-build-release"),
        # allow wildcard matching of test packages rather than hardcoding the version
        (Join-Path $root "Test\HuyEngine-*-Windows"),
        (Join-Path $root "Test\HuyEngine-*-Windows\bin"),
        (Join-Path $root "Test\HuyEngine-*-Windows\lib")
    )

    $found = $false
    foreach ($c in $candidates)
    {
        if (Test-Path $c)
        {
            Write-Host "Inspecting candidate: $c"
            # Copy bin/ and lib/ if present
            $binSrc = Join-Path $c "bin"
            if (Test-Path $binSrc)
            {
                $binDst = Join-Path $Staging "bin"
                New-Item -ItemType Directory -Path $binDst -Force | Out-Null
                Copy-Item -Path (Join-Path $binSrc '*') -Destination $binDst -Recurse -Force -ErrorAction SilentlyContinue
                Write-Host "Copied binaries from $binSrc -> $binDst"
                $found = $true
            }
            $libSrc = Join-Path $c "lib"
            if (Test-Path $libSrc)
            {
                $libDst = Join-Path $Staging "lib"
                New-Item -ItemType Directory -Path $libDst -Force | Out-Null
                Copy-Item -Path (Join-Path $libSrc '*') -Destination $libDst -Recurse -Force -ErrorAction SilentlyContinue
                Write-Host "Copied libs from $libSrc -> $libDst"
                $found = $true
            }
            # also copy top-level executables if present
            Get-ChildItem -Path $c -Filter "*.exe" -File -Recurse -ErrorAction SilentlyContinue | ForEach-Object {
                $binDst = Join-Path $Staging "bin"
                New-Item -ItemType Directory -Path $binDst -Force | Out-Null
                Copy-Item -Path $_.FullName -Destination $binDst -Force -ErrorAction SilentlyContinue
                Write-Host "Copied exe $( $_.FullName ) -> $binDst"
                $found = $true
            }
        }
    }

    if (-not $found)
    {
        Write-Host "ERROR: No existing binaries found in known locations. Packaging will continue but the archive may be empty of binaries."
    }
}

# Prune source files from staging: remove headers and source files
Write-Host "Pruning source files from staging..."
$sourcePatterns = @('*.c', '*.cc', '*.cxx', '*.cpp', '*.h', '*.hpp')
foreach ($pat in $sourcePatterns)
{
    Get-ChildItem -Path $Staging -Recurse -Include $pat -File -ErrorAction SilentlyContinue | ForEach-Object {
        Write-Host "Removing source file: $( $_.FullName )"
        Remove-Item -Force $_.FullName -ErrorAction SilentlyContinue
    }
}

# Remove common source/include directories if mistakenly installed
$removeDirs = @('src', 'Source', 'source', 'include', 'engine', 'Engine')
foreach ($d in $removeDirs)
{
    $p = Join-Path $Staging $d
    if (Test-Path $p)
    {
        Write-Host "Removing directory from staging: $p"
        Remove-Item -Recurse -Force $p -ErrorAction SilentlyContinue
    }
}

# Ensure README is present in the package root
$readmeCandidates = @('PACKAGE_README.md', 'README.md')
$copiedReadme = $false
foreach ($r in $readmeCandidates)
{
    $src = Join-Path $root $r
    if (Test-Path $src)
    {
        Copy-Item -Path $src -Destination (Join-Path $Staging 'README.md') -Force
        Write-Host "Copied README: $r to staging"
        $copiedReadme = $true
        break
    }
}
if (-not $copiedReadme)
{
    Write-Host "WARNING: No README found (PACKAGE_README.md or README.md)"
}

# Create final zip
$zipName = "HuyEngine-$Version-Windows-$Configuration.zip"
$zipPath = Join-Path $Output $zipName
if (Test-Path $zipPath)
{
    Remove-Item $zipPath -Force -ErrorAction SilentlyContinue
}
Write-Host "Creating zip: $zipPath"
# Compress-Archive requires items, include all content of staging
Compress-Archive -Path (Join-Path $Staging '*') -DestinationPath $zipPath -Force

Write-Host "Packaging complete. Output: $zipPath"
Write-Host "Contents of package (top-level):"
Get-ChildItem -Path $Staging -Force | ForEach-Object { Write-Host " - $( $_.Name )" }

Write-Host "Done."
