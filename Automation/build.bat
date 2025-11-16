@echo off
REM build.bat - compatibility wrapper and interactive menu / automation launcher
set SCRIPT_DIR=%~dp0

REM List of automation actions handled by Automation\automation_new.py
set ACTIONS=generate build rebuild clean "clean-all" run format menu

if "%~1"=="" goto :run_menu

REM Check if first arg matches any action (case-insensitive)
setlocal EnableDelayedExpansion
set FOUND=0
for %%A in (%ACTIONS%) do (
    if /I "%%~A"=="%~1" set FOUND=1
)
endlocal & set FOUND=%FOUND%
if "%FOUND%"=="1" goto :run_automation
goto :forward

:run_menu
if exist "%SCRIPT_DIR%Automation\automation_new.py" (
    python "%SCRIPT_DIR%Automation\automation_new.py" %*
    exit /b %ERRORLEVEL%
) else (
    echo Automation\automation_new.py not found in %SCRIPT_DIR%Automation\
    exit /b 2
)

:run_automation
if exist "%SCRIPT_DIR%Automation\automation_new.py" (
    python "%SCRIPT_DIR%Automation\automation_new.py" %*
    exit /b %ERRORLEVEL%
) else (
    echo Automation\automation_new.py not found in %SCRIPT_DIR%Automation\
    exit /b 2
)

:forward
if exist "%SCRIPT_DIR%scripts\ide-build.bat" (
    call "%SCRIPT_DIR%scripts\ide-build.bat" %*
    exit /b %ERRORLEVEL%
) else (
    echo Error: %SCRIPT_DIR%scripts\ide-build.bat not found.& exit /b 1
)
