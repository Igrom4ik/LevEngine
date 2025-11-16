@echo off
REM Wrapper to call ide-build.ps1 from cmd/IDE
set SCRIPT_DIR=%~dp0
powershell -ExecutionPolicy Bypass -File "%SCRIPT_DIR%ide-build.ps1" %*
exit /b %ERRORLEVEL%
