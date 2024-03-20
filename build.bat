@ECHO OFF
SETLOCAL enabledelayedexpansion

:: Get start time:
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "start=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)

SET package_name=%1

SET defines=
SET compiler_flags=-O3 -g -std=gnu2x
SET include_flags=-Iinclude
SET linker_flags=-ld3d11 -ld2d1 -lDwrite -ld3dcompiler -lole32 -lwindowscodecs -ldxguid
:: -lws2_32
:: get all c files on the package
SET source_path=.\packages\%package_name%
SET cFilenames=
:: compile resource files
FOR /R "%source_path%" %%f IN (*.rc) DO windres "%%f" -o "%%~dpnf.o"
:: compile
FOR /R "%source_path%" %%f IN (*.c *.o) DO CALL SET cFilenames=%%cFilenames%% "%%f"
if EXIST .\bin\%package_name%.exe DEL .\bin\%package_name%.exe
ECHO %package_name% compiling...
gcc %compiler_flags%%cFilenames% -o ./bin/%package_name% %defines% %include_flags% %linker_flags%
IF %ERRORLEVEL% NEQ 0 (
  ECHO gcc %compiler_flags%%cFilenames% -o ./bin/%package_name% %defines% %include_flags% %linker_flags%
  GOTO :EOF
) 
:: Get elapsed time:
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "end=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
set /A elapsed=end-start
ECHO %package_name% compiled in %elapsed%ms
:: clean
FOR /R "%source_path%" %%f IN (*.o) DO del "%%~dpnf.o"
:: execute
bin\%package_name%
