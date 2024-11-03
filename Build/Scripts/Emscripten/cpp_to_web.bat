@echo off

set STB_PATH=C:\SDK\stb
set ENGINE_PATH=..\..\..\..\defGameEngine
set SOURCES_PATH=..\..\..\Sources
set EMSCRIPTEN_PATH=C:\SDK\emsdk
set OUTPUT_PATH=..\..\Target

set COMPILER_FLAGS=-O2 -Wall -pedantic -std=c++20
set EMSCRIPTEN_FLAGS=-sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2

call %EMSCRIPTEN_PATH%\emsdk activate latest

if "%1" == "build" goto :build
if "%1" == "run" goto :run

:build
    call em++ %COMPILER_FLAGS% %EMSCRIPTEN_FLAGS% -I"%STB_PATH%" -I"%ENGINE_PATH%" -I"%SOURCES_PATH%" %SOURCES_PATH%\Main.cpp -o %OUTPUT_PATH%\index.html --preload-file ..\..\..\Assets
    goto :leave

:run
    call emrun %OUTPUT_PATH%\index.html
    goto :leave

:leave
