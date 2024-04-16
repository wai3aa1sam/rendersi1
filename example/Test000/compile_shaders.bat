@REM #change current directory to this file
@%~d0
@cd %~dp0

set PROJECT_ROOT=%~dp0
set RDS_ROOT=%~dp0..\..
set COMPILE_SHADER_PATH=%RDS_ROOT%\built-in\script\compile_shader

pushd %COMPILE_SHADER_PATH%

python rds_compile_shader.py %PROJECT_ROOT%

popd

@rem pause