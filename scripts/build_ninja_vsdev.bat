@echo off
REM Helper to run CMake+Ninja from a Visual Studio developer environment (x64)
REM Usage: scripts\build_ninja_vsdev.bat [additional cmake args]

echo Activating Visual Studio developer environment...
set "VSDEVCMD="

REM Try common VS locations (Community, Professional, Enterprise, BuildTools)
for %%P in (
  "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
  "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
  "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
  "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
) do (
  if exist %%~P if not defined VSDEVCMD set "VSDEVCMD=%%~P"
)

if not defined VSDEVCMD (
  echo VsDevCmd.bat not found in standard locations.
  echo Please run this script from a Developer Command Prompt or adjust the script to point to VsDevCmd.bat.
  exit /b 1
)

call "%VSDEVCMD%" -arch=amd64
if errorlevel 1 exit /b %errorlevel%

REM Change to repository root (one level up from scripts folder)
pushd "%~dp0\.."
if errorlevel 1 (
  echo Failed to change directory to repository root.
  exit /b 1
)

echo Running CMake configure (Ninja)...
cmake -S . -B cmake-build-debug -G "Ninja" -DLEVENGINE_FETCH_THIRDPARTY=ON -DCMAKE_BUILD_TYPE=Debug %*
if errorlevel 1 (
  popd
  exit /b %errorlevel%
)

echo Building...
cmake --build cmake-build-debug --config Debug
set "RC=%ERRORLEVEL%"
popd
exit /b %RC%
