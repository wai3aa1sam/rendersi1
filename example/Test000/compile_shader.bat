@REM #change current directory to this file
@%~d0
@cd %~dp0

set comp=..\..\build\rendersi1-x64-windows\src\render\shader_compiler\Debug\rds_shader_compiler.exe


%comp%

@rem pause