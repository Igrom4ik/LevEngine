@echo off
echo Build succeeded
cmake --build --preset %PRESET% || (echo Build failed & exit /b 1)
cmake --preset %PRESET% || (echo Configure failed & exit /b 1)
echo Using CMake preset %PRESET%
if "%PRESET%"=="" set PRESET=otladka
set PRESET=%1
:: Usage: gen_proj_msvc_ninja.bat [otladka|reliz]

