@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

set "NINJA_DIR=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
set "PATH=%NINJA_DIR%;%PATH%"

set "PROJ=C:\projetos\dumb compressor"

cmake -S "%PROJ%" -B "%PROJ%\build" -G Ninja -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 exit /b 1

cmake --build "%PROJ%\build"
