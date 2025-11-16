# Диагностика окружения сборки для CMake и Clang-CL
# Этот скрипт проверяет наличие необходимых инструментов и выявляет конфликты

Write-Host "=== Диагностика окружения сборки ===" -ForegroundColor Cyan
Write-Host ""

# 1. Проверка clang-cl
Write-Host "1. Проверка clang-cl:" -ForegroundColor Yellow
$clangCl = Get-Command clang-cl -ErrorAction SilentlyContinue
if ($clangCl)
{
    Write-Host "   ✓ clang-cl найден: $( $clangCl.Source )" -ForegroundColor Green
    & clang-cl --version 2>&1 | Select-Object -First 3 | ForEach-Object { Write-Host "     $_" }
}
else
{
    Write-Host "   ✗ clang-cl НЕ НАЙДЕН в PATH" -ForegroundColor Red
    Write-Host "     Проверяем стандартные пути установки..." -ForegroundColor Yellow

    $possiblePaths = @(
        "C:\Program Files\LLVM\bin\clang-cl.exe",
        "C:\Program Files (x86)\LLVM\bin\clang-cl.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang-cl.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\Llvm\x64\bin\clang-cl.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\x64\bin\clang-cl.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin\clang-cl.exe"
    )

    $found = $false
    foreach ($path in $possiblePaths)
    {
        if (Test-Path $path)
        {
            Write-Host "     → Найден в: $path" -ForegroundColor Cyan
            $found = $true
        }
    }

    if (-not $found)
    {
        Write-Host "     → clang-cl не найден ни в одном стандартном месте" -ForegroundColor Red
        Write-Host "       Установите LLVM или Visual Studio с компонентом 'C++ Clang tools for Windows'" -ForegroundColor Yellow
    }
}
Write-Host ""

# 2. Проверка MSVC (cl.exe)
Write-Host "2. Проверка MSVC (cl.exe):" -ForegroundColor Yellow
$msvcCl = Get-Command cl -ErrorAction SilentlyContinue
if ($msvcCl)
{
    Write-Host "   ✓ cl.exe найден: $( $msvcCl.Source )" -ForegroundColor Green
}
else
{
    Write-Host "   ✗ cl.exe НЕ НАЙДЕН в PATH" -ForegroundColor Red
    Write-Host "     Возможно, нужно запустить vcvarsall.bat" -ForegroundColor Yellow
}
Write-Host ""

# 3. Проверка конфликтов с MSYS2/MinGW
Write-Host "3. Проверка конфликтов с MSYS2/MinGW/ucrt64:" -ForegroundColor Yellow
$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if ($gpp)
{
    Write-Host "   ⚠ g++ найден: $( $gpp.Source )" -ForegroundColor Yellow
    if ($gpp.Source -match "msys|mingw|ucrt")
    {
        Write-Host "     ВНИМАНИЕ: Это может конфликтовать с clang-cl!" -ForegroundColor Red
        Write-Host "     Рекомендуется временно удалить из PATH для сборки с clang-cl" -ForegroundColor Yellow
    }
}
else
{
    Write-Host "   ✓ g++ не найден (хорошо для clang-cl)" -ForegroundColor Green
}

# Проверка PATH на наличие MSYS2/MinGW
$pathEntries = $env:PATH -split ';'
$conflictingPaths = $pathEntries | Where-Object { $_ -match "msys|mingw|ucrt64" }
if ($conflictingPaths)
{
    Write-Host "   ⚠ Обнаружены пути MSYS2/MinGW в PATH:" -ForegroundColor Yellow
    foreach ($p in $conflictingPaths)
    {
        Write-Host "     - $p" -ForegroundColor Yellow
    }
}
Write-Host ""

# 4. Проверка CMake
Write-Host "4. Проверка CMake:" -ForegroundColor Yellow
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmake)
{
    Write-Host "   ✓ CMake найден: $( $cmake.Source )" -ForegroundColor Green
    & cmake --version | Select-Object -First 1 | ForEach-Object { Write-Host "     $_" }
}
else
{
    Write-Host "   ✗ CMake НЕ НАЙДЕН" -ForegroundColor Red
}
Write-Host ""

# 5. Проверка Ninja
Write-Host "5. Проверка Ninja:" -ForegroundColor Yellow
$ninja = Get-Command ninja -ErrorAction SilentlyContinue
if ($ninja)
{
    Write-Host "   ✓ Ninja найден: $( $ninja.Source )" -ForegroundColor Green
    & ninja --version | ForEach-Object { Write-Host "     Версия: $_" }
}
else
{
    Write-Host "   ✗ Ninja НЕ НАЙДЕН" -ForegroundColor Red
}
Write-Host ""

# 6. Проверка CMakeCache в папках сборки
Write-Host "6. Проверка состояния папок сборки:" -ForegroundColor Yellow
$buildDirs = @("build", "build-msvc", "cmake-build-debug", "cmake-build-release")
foreach ($dir in $buildDirs)
{
    $cachePath = Join-Path $PSScriptRoot "..\$dir\CMakeCache.txt"
    if (Test-Path $cachePath)
    {
        Write-Host "   → $dir/" -ForegroundColor Cyan

        # Проверить, какой компилятор в кэше
        $cacheContent = Get-Content $cachePath
        $compiler = $cacheContent | Select-String "CMAKE_CXX_COMPILER:.*=" | Select-Object -First 1
        if ($compiler)
        {
            Write-Host "     Компилятор: $( $compiler.Line -replace '.*=', '' )" -ForegroundColor Gray
        }

        $generator = $cacheContent | Select-String "CMAKE_GENERATOR:.*=" | Select-Object -First 1
        if ($generator)
        {
            Write-Host "     Генератор: $( $generator.Line -replace '.*=', '' )" -ForegroundColor Gray
        }
    }
}
Write-Host ""

# 7. Рекомендации
Write-Host "=== Рекомендации ===" -ForegroundColor Cyan
Write-Host ""

if (-not $clangCl)
{
    Write-Host "❌ clang-cl не найден:" -ForegroundColor Red
    Write-Host "   1. Установите LLVM: https://github.com/llvm/llvm-project/releases" -ForegroundColor Yellow
    Write-Host "      ИЛИ" -ForegroundColor Yellow
    Write-Host "   2. Установите Visual Studio с компонентом 'C++ Clang tools for Windows'" -ForegroundColor Yellow
    Write-Host ""
}

if ($conflictingPaths)
{
    Write-Host "⚠ Обнаружены конфликтующие пути в PATH:" -ForegroundColor Yellow
    Write-Host "   Для сборки с clang-cl рекомендуется временно очистить PATH:" -ForegroundColor Yellow
    Write-Host '   $env:PATH = ($env:PATH -split "";;"" | Where-Object { $_ -notlike ""*msys*"" -and $_ -notlike ""*mingw*"" -and $_ -notlike ""*ucrt64*"" }) -join "";""' -ForegroundColor Cyan
    Write-Host ""
}

Write-Host "📋 Команды для очистки кэша CMake:" -ForegroundColor Cyan
Write-Host "   # Удалить конкретную папку сборки:" -ForegroundColor Gray
Write-Host "   Remove-Item -Recurse -Force build/" -ForegroundColor White
Write-Host ""
Write-Host "   # Очистить только кэш (без полного удаления):" -ForegroundColor Gray
Write-Host "   Remove-Item -Force build/CMakeCache.txt" -ForegroundColor White
Write-Host "   Remove-Item -Recurse -Force build/CMakeFiles/" -ForegroundColor White
Write-Host ""

Write-Host "=== Конец диагностики ===" -ForegroundColor Cyan

