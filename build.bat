@ECHO OFF
SETLOCAL enabledelayedexpansion

:: Get start time:
for /F "tokens=1-4 delims=:.," %%a in ("%time%") do (
   set /A "start=((((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100)*10"
)

SET package_name=%1

SET defines=
SET compiler_flags=-g -std=gnu2x
SET include_flags=-Iinclude
SET linker_flags=
SET output_extension=exe
:: get all c files on the package
SET cFilenames=
SET source_path=
:: check if is a library
IF "%package_name%" == "" (
  SET source_path=.\src
  SET include_flags=%include_flags%
  SET linker_flags=%linker_flags% -lws2_32 -ld3d11 -ldxguid
  :: -lcurl -lz -lssl -lcrypto -lnetsnmp -lcrypt32 -luser32 -lversion -lwldap32 -lws2_32 -lshlwapi -lmingw32  -ld3dcompiler
  set defines=%defines% -DSDK_LIB
  SET output_extension=dll
  SET compiler_flags=%compiler_flags% -shared -static
  SET package_name=sdk
) ELSE (
  SET source_path=.\%package_name%
  SET include_flags=%include_flags%
  SET linker_flags=%linker_flags% -Lbin -lsdk
)
:: compile resource files
FOR /R "%source_path%" %%f IN (*.rc) DO windres "%%f" -o "%%~dpnf.o"
:: compile
FOR /R "%source_path%" %%f IN (*.c *.o) DO CALL SET cFilenames=%%cFilenames%% "%%f"
if EXIST .\bin\%package_name%.%output_extension% DEL .\bin\%package_name%.%output_extension%
ECHO %package_name% compiling...
gcc %compiler_flags%%cFilenames% -o ./bin/%package_name%.%output_extension% %defines% %include_flags% %linker_flags%
IF %ERRORLEVEL% NEQ 0 (
  ECHO gcc %compiler_flags%%cFilenames% -o ./bin/%package_name%.%output_extension% %defines% %include_flags% %linker_flags%
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
IF %output_extension%==exe bin\%package_name%
