@echo off
rem Start a command prompt with MSVC environment and run Fleet in project directory (keeps window open)
rem It uses the helper find_and_call_vcvars.bat to locate vcvarsall.bat
set PROJDIR=D:\DEVELOP\LevEngine
if not exist "%~dp0find_and_call_vcvars.bat" (
  rem try relative path from workspace
  if exist "%PROJDIR%\scripts\find_and_call_vcvars.bat" (
    set FINDER=%PROJDIR%\scripts\find_and_call_vcvars.bat
  ) else (
    echo ERROR: find_and_call_vcvars.bat not found. Please ensure scripts\find_and_call_vcvars.bat exists.
    exit /b 2
  )
) else (
  set FINDER=%~dp0find_and_call_vcvars.bat
)

nrem Use cmd /k so the window remains open after launching Fleet. We pass a chained command: change dir and run fleet.
"%FINDER%" "cmd /k cd /d %PROJDIR% && fleet"

