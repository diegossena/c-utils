@ECHO OFF
SETLOCAL enabledelayedexpansion

SET package_name=%1

SET defines=-DSDK_DEVELOPMENT
SET compiler_flags=-O3 -g -pg
SET include_flags=-Iinclude
SET linker_flags=-ld3d11 -ld2d1 -lDwrite -ld3dcompiler -lole32 -lwindowscodecs -ldxguid -lgdi32 -lws2_32 
:: get all c files on the package
SET source_path=.\src\%package_name%
SET cFilenames=
FOR /R "%source_path%" %%f IN (*.c *.o) DO CALL SET cFilenames=%%cFilenames%% "%%f"
FOR /R .\src\sdk %%f IN (*.c *.o) DO CALL SET cFilenames=%%cFilenames%% "%%f"
:: compile resource files
FOR /R "%source_path%" %%f IN (*.rc) DO windres "%%f" -o "%%~dpnf.o"
:: clear exe
if EXIST .\bin\%package_name%.exe DEL .\bin\%package_name%.exe
ECHO %package_name% compiling...
:: get start time
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "start=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
:: compile
gcc %compiler_flags%%cFilenames% -o ./bin/%package_name% %defines% %include_flags% %linker_flags%
:: get end time
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "end=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
IF %ERRORLEVEL% NEQ 0 (
  GOTO :EOF
) 
:: Get elapsed time:
set /A elapsed=end-start
ECHO %package_name% compiled in %elapsed%ms
:: get start time
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "start=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
:: execute
bin\%package_name%
:: get end time
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "end=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
:: Get elapsed time:
set /A elapsed=end-start
ECHO %package_name% executed in %elapsed%ms

::FOR /R "%source_path%" %%f IN (*.o) DO del "%%~dpnf.o"