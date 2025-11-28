@echo off
rem Find vcvarsall.bat in common locations or using vswhere, then call it and run provided command
setlocal EnableExtensions EnableDelayedExpansion
set "VCVARS="
set "LOG=%~dp0vcvars.log"
echo ===== find_and_call_vcvars log at %DATE% %TIME% =====> "%LOG%"

rem Common candidate paths
set "CAND1=%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat"
set "CAND2=%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
set "CAND3=%ProgramFiles%\Microsoft Visual Studio\17\Community\VC\Auxiliary\Build\vcvarsall.bat"
set "CAND4=%ProgramFiles%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
set "CAND5=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"

echo Checking candidate: %CAND1% >> "%LOG%" & if exist "%CAND1%" echo FOUND: %CAND1% >> "%LOG%" & if exist "%CAND1%" set "VCVARS=%CAND1%"
echo Checking candidate: %CAND2% >> "%LOG%" & if exist "%CAND2%" echo FOUND: %CAND2% >> "%LOG%" & if not defined VCVARS if exist "%CAND2%" set "VCVARS=%CAND2%"
echo Checking candidate: %CAND3% >> "%LOG%" & if exist "%CAND3%" echo FOUND: %CAND3% >> "%LOG%" & if not defined VCVARS if exist "%CAND3%" set "VCVARS=%CAND3%"
echo Checking candidate: %CAND4% >> "%LOG%" & if exist "%CAND4%" echo FOUND: %CAND4% >> "%LOG%" & if not defined VCVARS if exist "%CAND4%" set "VCVARS=%CAND4%"
echo Checking candidate: %CAND5% >> "%LOG%" & if exist "%CAND5%" echo FOUND: %CAND5% >> "%LOG%" & if not defined VCVARS if exist "%CAND5%" set "VCVARS=%CAND5%"

rem Try vswhere if available to locate Visual Studio installation
if not defined VCVARS (
  echo Trying vswhere >> "%LOG%"
  if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
    for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2^>nul`) do (
      set "VSROOT=%%i"
      echo vswhere returned: %%i >> "%LOG%"
    )
    if defined VSROOT (
      echo Checking vswhere candidate: !VSROOT!\VC\Auxiliary\Build\vcvarsall.bat >> "%LOG%"
      if exist "!VSROOT!\VC\Auxiliary\Build\vcvarsall.bat" (
        echo FOUND via vswhere: !VSROOT!\VC\Auxiliary\Build\vcvarsall.bat >> "%LOG%"
        set "VCVARS=!VSROOT!\VC\Auxiliary\Build\vcvarsall.bat"
      )
    ) else (
      echo vswhere didn't return installation path >> "%LOG%"
    )
  ) else (
    echo vswhere not found at %ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe >> "%LOG%"
  )
)

if not defined VCVARS (
  echo ERROR: Could not find vcvarsall.bat in common locations and vswhere not available. >> "%LOG%"
  echo ERROR: Could not find vcvarsall.bat in common locations and vswhere not available.
  echo Please install Visual Studio Build Tools or set VCVARS path in this script.
  exit /b 2
)

echo Using vcvars: "%VCVARS%" >> "%LOG%"
echo Using vcvars: "%VCVARS%"

rem Call vcvars for x64 (fall back to x86 if that fails)
call "%VCVARS%" x64 2>> "%LOG%"
if errorlevel 1 (
  echo vcvars x64 failed, trying x86... >> "%LOG%"
  echo vcvars x64 failed, trying x86...
  call "%VCVARS%" x86 2>> "%LOG%"
)

rem If no command provided, just return after setting environment
if "%~1"=="" (
  echo Environment initialized. No command provided. >> "%LOG%"
  echo Environment initialized. No command provided.
  exit /b 0
)

rem Execute the provided command (supports chains). Use %* to pass all args. Log the command.
echo Executing command: %* >> "%LOG%"
echo Executing command: %*
cmd /c %*
set "RC=%ERRORLEVEL%"
echo Command exited with %RC% >> "%LOG%"
endlocal
exit /b %RC%
