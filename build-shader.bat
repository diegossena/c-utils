@ECHO OFF
:: config
SET name=shader
:: shader compiler
ECHO Compiling shaders...
tools\fxc.exe /nologo /T vs_5_0 /E vs_main /Fo share/vs.cso assets/shader.hlsl
tools\fxc.exe /nologo /T ps_5_0 /E ps_main /Fo share/ps.cso assets/shader.hlsl
IF %ERRORLEVEL% NEQ 0 ECHO Error: %ERRORLEVEL% && EXIT /B %ERRORLEVEL%