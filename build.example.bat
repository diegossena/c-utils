@ECHO OFF
SET package_name=%1
SET defines=-DSDK_DEVELOPMENT -DSDK_UNITY
SET compiler_flags=-O3
SET include_flags=-Iinclude -I.
SET linker_flags=-lws2_32 -ld3d11 -ld2d1 -lDwrite -ld3dcompiler -lole32 -ldxguid -lgdi32
SET params=
:: clear exe
if EXIST .\bin\%package_name%.exe DEL .\bin\%package_name%.exe
ECHO %package_name% compiling...
:: get start time
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
  SET /A "start=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
:: compile
gcc %compiler_flags% ./src/%package_name%.c -o ./bin/%package_name% %defines% %include_flags% %linker_flags%
:: get end time
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
  set /A "end=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
SET /A elapsed=end-start
ECHO %package_name% compiled in %elapsed%ms
IF %ERRORLEVEL% NEQ 0 (
  GOTO :EOF
) 
:: bin size
for %%A in ("bin\%package_name%.exe") do (
  SET "tamanho=%%~zA"
)
ECHO file.size=%tamanho%
:: get start time
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
  SET /A "start=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
:: execute
bin\%package_name% %params%
:: get end time
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
  set /A "end=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)
SET /A elapsed=end-start
ECHO %package_name% executed in %elapsed%ms