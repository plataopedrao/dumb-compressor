@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

set "NINJA_DIR=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
set "PATH=%NINJA_DIR%;%PATH%"

set "PROJ=C:\projetos\dumb compressor"

cmake -S "%PROJ%" -B "%PROJ%\build" -G Ninja -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 exit /b 1

cmake --build "%PROJ%\build"
if %errorlevel% neq 0 exit /b 1

:: Install VST3 to Peposoft subfolder (requires elevation)
set "VST3_SRC=%PROJ%\build\DumbCompressor_artefacts\Release\VST3\Dumb Compressor.vst3"
set "VST3_DEST=C:\Program Files\Common Files\VST3\Peposoft\Dumb Compressor.vst3"

powershell -Command "Start-Process powershell -Verb RunAs -ArgumentList '-Command New-Item -ItemType Directory -Force -Path ''C:\Program Files\Common Files\VST3\Peposoft'' | Out-Null; Copy-Item -Path ''%VST3_SRC%'' -Destination ''%VST3_DEST%'' -Recurse -Force; Write-Host Installed' -Wait"

echo.
echo Build and install complete.
echo VST3 -^> C:\Program Files\Common Files\VST3\Peposoft\
