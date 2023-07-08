@REM #change current directory to this file
@%~d0
@cd %~dp0

mkdir "local_temp\bin"

glslc -x glsl -fshader-stage=vertex -fentry-point=main -c   "hello_triangle0.vert" -o "local_temp/bin/hello_triangle0.vert.spv"
glslc -x glsl -fshader-stage=fragment -fentry-point=main -c "hello_triangle0.frag" -o "local_temp/bin/hello_triangle0.frag.spv"

glslc -x hlsl -fshader-stage=vertex     -fentry-point=vs_main -c "hello_triangle0.hlsl" -o "local_temp/bin/hello_triangle0.hlsl.vert.spv"
glslc -x hlsl -fshader-stage=fragment   -fentry-point=ps_main -c "hello_triangle0.hlsl" -o "local_temp/bin/hello_triangle0.hlsl.frag.spv"

glslc -x hlsl -fshader-stage=vertex     -fentry-point=vs_main -c "hello_triangle1.hlsl" -o "local_temp/bin/hello_triangle1.hlsl.vert.spv"
glslc -x hlsl -fshader-stage=fragment   -fentry-point=ps_main -c "hello_triangle1.hlsl" -o "local_temp/bin/hello_triangle1.hlsl.frag.spv"

@rem pause