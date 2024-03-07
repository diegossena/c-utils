@ECHO OFF
:: config
SET name=2d_paint
SET output=bin
:: shader compiler
ECHO Compiling shaders...
tools\fxc.exe /nologo /T vs_5_0 /E vs_main /Fo %output%/vs_%name%.cso assets/shaders/%name%.hlsl
IF %ERRORLEVEL% NEQ 0 ECHO Error: %ERRORLEVEL% && EXIT /B %ERRORLEVEL%
tools\fxc.exe /nologo /T ps_5_0 /E ps_main /Fo %output%/ps_%name%.cso assets/shaders/%name%.hlsl
IF %ERRORLEVEL% NEQ 0 ECHO Error: %ERRORLEVEL% && EXIT /B %ERRORLEVEL%
ECHO Done.