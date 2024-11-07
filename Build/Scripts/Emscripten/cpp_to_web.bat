@echo off

set ENGINE_PATH=..\..\..\Include\defGameEngine
set SOURCES_PATH=..\..\..\Sources
set OUTPUT_PATH=..\..\Target
set EMSDK_PATH=C:\SDK\emsdk
set SOL2_PATH=C:\SDK\sol2
set STB_PATH=C:\SDK\stb
set LUA_PATH=C:\SDK\lua542

set COMPILER_FLAGS=-O2 -Wall -pedantic -std=c++20
set EMSCRIPTEN_FLAGS=-sALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2

call %EMSDK_PATH%\emsdk activate latest

if "%1" == "build" goto :build
if "%1" == "run" goto :run
goto :leave

:build
    call em++ %COMPILER_FLAGS% %EMSCRIPTEN_FLAGS% -I"%SOL2_PATH%\include" -I"%STB_PATH%" -I"%LUA_PATH%\include" -I"%ENGINE_PATH%" -I"%SOURCES_PATH%" -L"%LUA_PATH%\" %SOURCES_PATH%\* -o %OUTPUT_PATH%\index.html --preload-file ..\..\..\Assets
    goto :leave

:run
    call emrun %OUTPUT_PATH%\index.html
    goto :leave

:leave
