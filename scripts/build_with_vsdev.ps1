# Скрипт для удобной сборки проекта с MSVC+Ninja через VS Dev Command Prompt
param(
    [string]$Preset = 'ninja-debug',
    [switch]$Clean
)

$vsCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat'
if(-not (Test-Path $vsCmd)) {
    Write-Error "VsDevCmd not found at $vsCmd. Запустите Developer Command Prompt вручную или поправьте путь в скрипте."
    exit 1
}

Write-Host "Activating Visual Studio developer environment..."
& $vsCmd -arch=amd64

if($LASTEXITCODE -ne 0) {
    Write-Error "Не удалось активировать VsDevCmd (код $LASTEXITCODE)."
    exit $LASTEXITCODE
}

$buildPreset = $Preset

if($Clean) {
    Write-Host "Cleaning build dir for preset: $buildPreset"
    # Пресеты в этом репозитории используют cmake-build-debug и т.п., но cmake --build --preset умеет очистить при помощи удаления каталога
    $binaryDir = "cmake-build-debug"
    if(Test-Path $binaryDir) { Remove-Item -Recurse -Force $binaryDir }
}

Write-Host "Configuring (preset: $buildPreset) -> cmake-config.log"
cmake --preset $buildPreset 2>&1 | Tee-Object -FilePath cmake-config.log
if($LASTEXITCODE -ne 0) {
    Write-Error "CMake configure failed. Смотрите cmake-config.log"
    exit $LASTEXITCODE
}

Write-Host "Building (build preset: $buildPreset) -> build.log"
cmake --build --preset $buildPreset 2>&1 | Tee-Object -FilePath build.log
if($LASTEXITCODE -ne 0) {
    Write-Error "Сборка завершилась с ошибкой. Смотрите build.log"
    exit $LASTEXITCODE
}

Write-Host "Сборка успешно завершена. Binaries: (см. соответствующую build-папку)."

