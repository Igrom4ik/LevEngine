@echo off
REM Быстрая проверка конфигурации горячих клавиш
setlocal ENABLEDELAYEDEXPANSION

echo Проверка конфигурации External Tools...
if exist "%~dp0..\.idea\tools\Build Menu.xml" (
    echo [OK] External Tools настроены
) else (
    echo [ERROR] External Tools НЕ настроены
    exit /b 1
)

echo Проверка Python...
python --version >nul 2>&1 || py --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python не найден
    exit /b 1
) else (
    echo [OK] Python установлен
)

echo Проверка скриптов...
if exist "%~dp0build_menu.bat" (
    echo [OK] build_menu.bat найден
) else (
    echo [ERROR] build_menu.bat не найден
    exit /b 1
)

echo.
echo [SUCCESS] Все компоненты на месте!
echo.
echo Теперь настройте горячую клавишу:
echo 1. Откройте Settings (Ctrl+Alt+S) -^> Keymap
echo 2. Найдите: External Tools -^> Build Menu
echo 3. Добавьте горячую клавишу Ctrl+Numpad1
echo.
exit /b 0

