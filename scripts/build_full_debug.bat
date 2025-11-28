@echo off
rem Full configure+build (Debug) using find_and_call_vcvars.bat to initialize MSVC env in the same session
set SCRIPT_DIR=%~dp0
set FORCE_CLEAN=%1
if /i "%FORCE_CLEAN%"=="/force-clean" (
  echo === FORCE CLEAN: removing cmake-build-debug ===
  rd /s /q "%~dp0..\cmake-build-debug" 2>nul || echo No build dir to remove
)
echo === Starting Full Debug Build ===
rem Run configure; if it fails, stop
call "%SCRIPT_DIR%find_and_call_vcvars.bat" "cd /d \"%~dp0..\" && echo --- Configuring (otladka preset) --- && cmake --preset otladka || exit /b 1"
if errorlevel 1 (
  echo Configuration failed.
  type "%SCRIPT_DIR%vcvars.log"
  exit /b 1
)
echo --- Running clean target (debug) ---
call "%SCRIPT_DIR%find_and_call_vcvars.bat" "cd /d \"%~dp0..\" && cmake --build cmake-build-debug --config Debug --target clean || echo Clean returned non-zero"

echo --- Building (Debug) with verbose output ---
call "%SCRIPT_DIR%find_and_call_vcvars.bat" "cd /d \"%~dp0..\" && cmake --build cmake-build-debug --config Debug --clean-first -j -- -v || exit /b 1"
if errorlevel 1 (
  echo Build failed.
  type "%SCRIPT_DIR%vcvars.log"
  exit /b 1
)
echo === Debug Build Complete ===
type "%SCRIPT_DIR%vcvars.log"
exit /b 0
