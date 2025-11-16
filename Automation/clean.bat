@echo off
REM Compatibility wrapper: forward calls to scripts\clean.bat (keeps tools expecting clean.bat in repo root working)
set SCRIPT_DIR=%~dp0
if exist "%SCRIPT_DIR%scripts\clean.bat" (
    call "%SCRIPT_DIR%scripts\clean.bat" %*
    exit /b %ERRORLEVEL%
) else (
    echo Error: %SCRIPT_DIR%scripts\clean.bat not found.& exit /b 1
)
