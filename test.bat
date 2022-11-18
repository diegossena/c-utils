CLS
@ECHO OFF
gcc src/main.c -o bin/main -Imodules -Isrc -Wall -Wextra -ld3d11 -ldxguid -ld3dcompiler

IF %ERRORLEVEL% == 0 (
  .\bin\main
)