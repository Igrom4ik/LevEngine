param(
    [string]$Preset = "otladka",
    [switch]$SkipConfigure
)

# Запуск конфигурации и сборки проекта из IDE или внешнего инструмента
# Путь проекта = родительская папка скрипта
$projectRoot = Split-Path -Parent $PSScriptRoot
Push-Location -LiteralPath $projectRoot
try
{
    Write-Host "Project root: $projectRoot"
    if (-not $SkipConfigure)
    {
        Write-Host "Running: cmake --preset $Preset"
        $rc = & cmake --preset $Preset
        if ($LASTEXITCODE -ne 0)
        {
            throw "cmake configure failed (exit $LASTEXITCODE)"
        }
    }
    else
    {
        Write-Host "Skipping configure (SkipConfigure set)"
    }

    # Determine binary dir from preset naming (otladka -> build, reliz -> cmake-build-release)
    $binDir = Join-Path $projectRoot "build"
    if (-not (Test-Path $binDir))
    {
        Write-Host "Binary dir $binDir not found, will try cmake to create it."
        & cmake --preset $Preset | Out-Host
        if (-not (Test-Path $binDir))
        {
            throw "Binary dir still not found: $binDir"
        }
    }

    Write-Host "Building in: $binDir"
    & cmake --build $binDir --config Debug
    if ($LASTEXITCODE -ne 0)
    {
        throw "cmake build failed (exit $LASTEXITCODE)"
    }

    Write-Host "Build finished successfully."
}
catch
{
    Write-Error $_
    exit 1
}
finally
{
    Pop-Location
}

