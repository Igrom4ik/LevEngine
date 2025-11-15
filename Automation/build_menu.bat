@echo off
REM HuyEngine - launch interactive build menu (Automation\automation_new.py)
setlocal ENABLEDELAYEDEXPANSION
set "SCRIPT_DIR=%~dp0"

REM Check if script exists
if not exist "%SCRIPT_DIR%automation_new.py" (
    echo [ERROR] automation_new.py not found in %SCRIPT_DIR%
    echo Please ensure the file exists and the path is correct.
    pause
    exit /b 2
)

REM Find Python
where python >nul 2>&1
if not errorlevel 1 (
    set "PYEXE=python"
    goto :found_python
)

where py >nul 2>&1
if not errorlevel 1 (
    set "PYEXE=py -3"
    goto :found_python
)

REM Python not found
echo ========================================
echo [ERROR] Python not found in PATH
echo ========================================
echo.
echo Please install Python 3.x or add it to PATH.
echo.
echo Download Python from: https://www.python.org/downloads/
echo Make sure to check "Add Python to PATH" during installation.
echo.
pause
exit /b 9009

:found_python
REM Default action in automation_new.py is 'menu', so no args opens the menu
%PYEXE% "%SCRIPT_DIR%automation_new.py" %*
set "EXIT_CODE=%ERRORLEVEL%"

if %EXIT_CODE% neq 0 (
    echo.
    echo ========================================
    echo [INFO] Script exited with code: %EXIT_CODE%
    echo ========================================
)

exit /b %EXIT_CODE%
