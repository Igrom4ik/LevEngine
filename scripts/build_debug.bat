@echo off
rem Build script that calls vcvarsall and runs cmake commands in the same session
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
nif errorlevel 1 exit /b 1
cmake --preset otladka
nif errorlevel 1 exit /b 1
cmake --build cmake-build-debug --config Debug -j

