CLS
@ECHO OFF
gcc src/main.c -o bin/main -Imodules -Isrc
IF %ERRORLEVEL% == 0 (
  .\bin\main
)