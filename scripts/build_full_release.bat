@echo off
rem Full configure+build (Release) using find_and_call_vcvars.bat to initialize MSVC env in the same session
set SCRIPT_DIR=%~dp0
set FORCE_CLEAN=%1
if /i "%FORCE_CLEAN%"=="/force-clean" (
  echo === FORCE CLEAN: removing cmake-build-release ===
  rd /s /q "%~dp0..\cmake-build-release" 2>nul || echo No build dir to remove
)
echo === Starting Full Release Build ===
rem Run configure; if it fails, stop
call "%SCRIPT_DIR%find_and_call_vcvars.bat" "cd /d \"%~dp0..\" && echo --- Configuring (reliz preset) --- && cmake --preset reliz || exit /b 1"
if errorlevel 1 (
  echo Configuration failed.
  type "%SCRIPT_DIR%vcvars.log"
  exit /b 1
)
echo --- Building: running clean target first ---
rem First run clean target explicitly to ensure full rebuild
call "%SCRIPT_DIR%find_and_call_vcvars.bat" "cd /d \"%~dp0..\" && echo --- Running clean target --- && cmake --build cmake-build-release --config Release --target clean || echo Clean returned non-zero"
if errorlevel 1 (
  echo Clean reported a non-zero exit code (continuing to build anyway)...
)
echo --- Building (Release) ---
call "%SCRIPT_DIR%find_and_call_vcvars.bat" "cd /d \"%~dp0..\" && echo --- Running build --- && cmake --build cmake-build-release --config Release -j || exit /b 1"
if errorlevel 1 (
  echo Build failed.
  type "%SCRIPT_DIR%vcvars.log"
  exit /b 1
)
echo === Release Build Complete ===
type "%SCRIPT_DIR%vcvars.log"
exit /b 0
