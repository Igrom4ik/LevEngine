param(
    [switch]$Force
)

# Safe cleanup script — removes known build artifacts after confirmation
$scriptDir = $PSScriptRoot
$projectRoot = Split-Path -Parent $scriptDir
$targets = @(
    "build",
    "build-msvc",
    "cmake-build-debug",
    "cmake-build-release",
    "cmake-build-release-event-trace",
    "bin",
    "x64",
    "Engine/Engine.dir",
    "Engine/CMakeFiles",
    "CMakeFiles",
    "CMakeCache.txt",
    "cmake_install.cmake",
    "InstallScripts.json",
    "TargetDirectories.txt"
)

Write-Host "Cleanup script. The following targets will be removed:"
$targets | ForEach-Object { Write-Host " - $_" }

if (-not $Force)
{
    $confirm = Read-Host "Continue? (y/N)"
    if ($confirm -notlike 'y*')
    {
        Write-Host "Cancelled. Nothing removed."; exit 0
    }
}

foreach ($t in $targets)
{
    $path = Join-Path $projectRoot $t
    if (Test-Path $path)
    {
        try
        {
            Remove-Item -LiteralPath $path -Recurse -Force -ErrorAction Stop
            Write-Host "Removed: $path"
        }
        catch
        {
            Write-Warning "Failed to remove: $path - $( $_ )"
        }
    }
}
Write-Host "Done."
