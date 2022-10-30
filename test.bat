CLS
@ECHO OFF
gcc src/main.c -o bin/main -Imodules -Isrc -Wall -Wextra
IF %ERRORLEVEL% == 0 (
  .\bin\main
)