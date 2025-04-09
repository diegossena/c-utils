@ECHO OFF
:: config
SET name=texture
:: shader compiler
ECHO Compiling shaders...
tools\fxc.exe /nologo /T vs_5_0 /E vs_main /Fo assets/%name%.vs.cso assets/%name%.hlsl
tools\fxc.exe /nologo /T ps_5_0 /E ps_main /Fo assets/%name%.ps.cso assets/%name%.hlsl
IF %ERRORLEVEL% NEQ 0 ECHO Error: %ERRORLEVEL% && EXIT /B %ERRORLEVEL%

node tools/cso_reader.js