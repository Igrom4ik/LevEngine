@echo off
REM HuyEngine build/menu launcher (scripts\build.bat)
REM - No args or known action -> run interactive Automation\automation_new.py
REM - Other args (e.g. --preset ...) -> forward to ide-build.bat

setlocal ENABLEDELAYEDEXPANSION
set "SCRIPT_DIR=%~dp0"
set "ROOT_DIR=%SCRIPT_DIR%.."

set "ACTIONS=generate build rebuild clean clean-all run format menu"

if "%~1"=="" goto run_menu
set "FOUND=0"
for %%A in (%ACTIONS%) do (
    if /I "%%~A"=="%~1" set "FOUND=1"
)
if "%FOUND%"=="1" goto run_automation
goto forward

:ensure_python
where python >nul 2>&1
if not errorlevel 1 (
    set "PYEXE=python"
    goto :eof
)
where py >nul 2>&1
if not errorlevel 1 (
    set "PYEXE=py -3"
    goto :eof
)
echo Python not found in PATH. Please install Python 3.x or add it to PATH.
exit /b 9009

:run_menu
if exist "%ROOT_DIR%\Automation\automation_new.py" (
    call :ensure_python
    %PYEXE% "%ROOT_DIR%\Automation\automation_new.py" %*
    exit /b %ERRORLEVEL%
) else (
    echo Automation\automation_new.py not found in "%ROOT_DIR%\Automation\"
    exit /b 2
)

:run_automation
if exist "%ROOT_DIR%\Automation\automation_new.py" (
    call :ensure_python
    %PYEXE% "%ROOT_DIR%\Automation\automation_new.py" %*
    exit /b %ERRORLEVEL%
) else (
    echo Automation\automation_new.py not found in "%ROOT_DIR%\Automation\"
    exit /b 2
)

:forward
if exist "%SCRIPT_DIR%\ide-build.bat" (
    call "%SCRIPT_DIR%\ide-build.bat" %*
    exit /b %ERRORLEVEL%
) else (
    echo Error: "%SCRIPT_DIR%\ide-build.bat" not found.
    exit /b 1
)
