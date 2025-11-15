param(
    [string]$preset = "otladka"
)

Write-Host "Using CMake preset: $preset"

cmake --preset $preset
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configure failed (exit code $LASTEXITCODE)"
    exit $LASTEXITCODE
}

cmake --build --preset $preset
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake build failed (exit code $LASTEXITCODE)"
    exit $LASTEXITCODE
}

Write-Host "Build succeeded"

