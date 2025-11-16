# Скрипт для очистки кэша CMake
# Использование: .\clean_cmake_cache.ps1 [-BuildDir <путь>] [-All]

param(
    [string]$BuildDir = "",
    [switch]$All = $false,
    [switch]$DryRun = $false
)

function Remove-SafeRecurse
{
    param([string]$Path, [string]$Description)

    if (Test-Path $Path)
    {
        if ($DryRun)
        {
            Write-Host "   [DRY-RUN] Будет удалено: $Path" -ForegroundColor Yellow
        }
        else
        {
            Write-Host "   Удаление: $Description" -ForegroundColor Cyan
            Remove-Item -Recurse -Force $Path -ErrorAction Stop
            Write-Host "   ✓ Удалено" -ForegroundColor Green
        }
    }
    else
    {
        Write-Host "   ⊘ Не найдено: $Path" -ForegroundColor Gray
    }
}

Write-Host "=== Очистка кэша CMake ===" -ForegroundColor Cyan
Write-Host ""

if ($DryRun)
{
    Write-Host "⚠ Режим DRY-RUN: файлы не будут удалены" -ForegroundColor Yellow
    Write-Host ""
}

# Список стандартных директорий сборки
$standardBuildDirs = @(
    "build",
    "build-msvc",
    "cmake-build-debug",
    "cmake-build-release",
    "cmake-build-release-event-trace"
)

if ($All)
{
    Write-Host "Режим: Очистка всех папок сборки" -ForegroundColor Yellow
    Write-Host ""

    foreach ($dir in $standardBuildDirs)
    {
        $fullPath = Join-Path $PSScriptRoot "..\$dir"
        if (Test-Path $fullPath)
        {
            Write-Host "→ Папка: $dir/" -ForegroundColor Cyan
            Remove-SafeRecurse -Path $fullPath -Description "Вся папка $dir/"
            Write-Host ""
        }
    }
}
elseif ($BuildDir)
{
    Write-Host "Режим: Очистка указанной папки сборки" -ForegroundColor Yellow
    Write-Host "Папка: $BuildDir" -ForegroundColor White
    Write-Host ""

    $fullPath = if ( [System.IO.Path]::IsPathRooted($BuildDir))
    {
        $BuildDir
    }
    else
    {
        Join-Path $PSScriptRoot "..\$BuildDir"
    }

    if (-not (Test-Path $fullPath))
    {
        Write-Host "✗ Папка не найдена: $fullPath" -ForegroundColor Red
        exit 1
    }

    Remove-SafeRecurse -Path (Join-Path $fullPath "CMakeCache.txt") -Description "CMakeCache.txt"
    Remove-SafeRecurse -Path (Join-Path $fullPath "CMakeFiles") -Description "CMakeFiles/"
    Remove-SafeRecurse -Path (Join-Path $fullPath "cmake_install.cmake") -Description "cmake_install.cmake"
    Remove-SafeRecurse -Path (Join-Path $fullPath "build.ninja") -Description "build.ninja (если Ninja)"
    Remove-SafeRecurse -Path (Join-Path $fullPath "*.vcxproj") -Description "*.vcxproj (если MSVC)"
    Remove-SafeRecurse -Path (Join-Path $fullPath "*.sln") -Description "*.sln (если MSVC)"
}
else
{
    Write-Host "Режим: Очистка только кэша (CMakeCache.txt и CMakeFiles/) из всех папок сборки" -ForegroundColor Yellow
    Write-Host ""

    foreach ($dir in $standardBuildDirs)
    {
        $fullPath = Join-Path $PSScriptRoot "..\$dir"
        if (Test-Path $fullPath)
        {
            Write-Host "→ Папка: $dir/" -ForegroundColor Cyan
            Remove-SafeRecurse -Path (Join-Path $fullPath "CMakeCache.txt") -Description "  CMakeCache.txt"
            Remove-SafeRecurse -Path (Join-Path $fullPath "CMakeFiles") -Description "  CMakeFiles/"
            Write-Host ""
        }
    }
}

Write-Host "=== Очистка завершена ===" -ForegroundColor Green
Write-Host ""

if (-not $DryRun)
{
    Write-Host "Теперь можно заново запустить конфигурацию CMake:" -ForegroundColor Cyan
    Write-Host "  cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=Automation/CMAKE/Toolchains/toolchain-clang-cl.cmake -B build" -ForegroundColor White
}

