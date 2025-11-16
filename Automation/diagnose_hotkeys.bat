@echo off
REM Диагностика проблем с горячими клавишами
setlocal ENABLEDELAYEDEXPANSION
set "SCRIPT_DIR=%~dp0"

echo ========================================
echo ДИАГНОСТИКА ГОРЯЧИХ КЛАВИШ CLion
echo ========================================
echo.

REM 1. Проверка файла External Tools
echo [1/6] Проверка конфигурации External Tools...
if exist "%SCRIPT_DIR%..\idea\tools\Build Menu.xml" (
    echo [OK] Файл конфигурации найден
    echo      %SCRIPT_DIR%..\idea\tools\Build Menu.xml
) else (
    echo [ERROR] Файл конфигурации НЕ найден!
    echo         Ожидаемый путь: %SCRIPT_DIR%..\idea\tools\Build Menu.xml
    goto :error
)
echo.

REM 2. Проверка Python
echo [2/6] Проверка Python...
where python >nul 2>&1
if not errorlevel 1 (
    echo [OK] Python найден:
    python --version
) else (
    where py >nul 2>&1
    if not errorlevel 1 (
        echo [OK] Python найден через py launcher:
        py --version
    ) else (
        echo [ERROR] Python НЕ найден в PATH!
        echo         Установите Python: https://www.python.org/downloads/
        goto :error
    )
)
echo.

REM 3. Проверка build_menu.bat
echo [3/6] Проверка build_menu.bat...
if exist "%SCRIPT_DIR%build_menu.bat" (
    echo [OK] build_menu.bat найден
    echo      %SCRIPT_DIR%build_menu.bat
) else (
    echo [ERROR] build_menu.bat НЕ найден!
    goto :error
)
echo.

REM 4. Проверка automation_new.py
echo [4/6] Проверка automation_new.py...
if exist "%SCRIPT_DIR%automation_new.py" (
    echo [OK] automation_new.py найден
    echo      %SCRIPT_DIR%automation_new.py
) else (
    echo [ERROR] automation_new.py НЕ найден!
    goto :error
)
echo.

REM 5. Тест запуска скрипта
echo [5/6] Тест запуска скрипта...
python "%SCRIPT_DIR%automation_new.py" --help >nul 2>&1
if not errorlevel 1 (
    echo [OK] Скрипт запускается без ошибок
) else (
    py "%SCRIPT_DIR%automation_new.py" --help >nul 2>&1
    if not errorlevel 1 (
        echo [OK] Скрипт запускается через py launcher
    ) else (
        echo [ERROR] Скрипт не запускается!
        goto :error
    )
)
echo.

REM 6. Проверка Keymap конфигурации
echo [6/6] Проверка Keymap конфигурации...
if exist "%USERPROFILE%\.CLion*\config\keymaps" (
    echo [INFO] Найдена директория keymaps
    dir /b "%USERPROFILE%\.CLion*\config\keymaps" 2>nul
) else (
    echo [WARNING] Директория keymaps не найдена в стандартном месте
    echo           Возможно, вы ещё не настроили горячие клавиши
)
echo.

REM Успешная диагностика
echo ========================================
echo РЕЗУЛЬТАТ: ВСЕ ПРОВЕРКИ ПРОЙДЕНЫ!
echo ========================================
echo.
echo СЛЕДУЮЩИЕ ШАГИ:
echo.
echo 1. Закройте и снова откройте CLion (или нажмите File -^> Invalidate Caches / Restart)
echo.
echo 2. Откройте Settings (Ctrl+Alt+S)
echo.
echo 3. Перейдите в Keymap
echo.
echo 4. Найдите: External Tools -^> External Tools -^> Build Menu
echo.
echo 5. Щелкните правой кнопкой -^> Add Keyboard Shortcut
echo.
echo 6. Нажмите: Ctrl+Numpad1
echo.
echo 7. Нажмите OK -^> Apply -^> OK
echo.
echo 8. Попробуйте нажать Ctrl+Numpad1 в редакторе
echo.
echo ЕСЛИ НЕ РАБОТАЕТ:
echo.
echo - Проверьте, что Num Lock включен
echo - Попробуйте другую горячую клавишу (например, Ctrl+Alt+B)
echo - Проверьте конфликты в Keymap -^> Find Shortcut
echo - Используйте Tools -^> External Tools -^> Build Menu из меню
echo.
echo Подробная инструкция: %SCRIPT_DIR%..\docs\CLION_HOTKEYS.md
echo.
pause
exit /b 0

:error
echo.
echo ========================================
echo РЕЗУЛЬТАТ: ОБНАРУЖЕНЫ ОШИБКИ!
echo ========================================
echo.
echo Пожалуйста, исправьте указанные проблемы и запустите диагностику снова.
echo.
pause
exit /b 1
