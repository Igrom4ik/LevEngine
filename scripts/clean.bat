@echo off
REM Wrapper to run cleanup.ps1 from cmd
set SCRIPT_DIR=%~dp0
powershell -ExecutionPolicy Bypass -File "%SCRIPT_DIR%cleanup.ps1" %*
