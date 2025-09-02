#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompilerConsoleApp.h"

#define RDS_TEST_SHADER_COMPILER 0

namespace rds
{

ShaderGnuMake_VarName gnuMakeVarName;

}

int main(int argc, char* argv[])
{
	#if RDS_TEST_SHADER_COMPILER
	#if 0
	char* argvs[] =
	{
		"rds_shader_compiler.exe",
		"-cwd=../../../../../example/Test000",
		"-x=hlsl",
		//"-file = \"Assets\\\\Shaders\\\\test.shader\"",
		//"-file=\"../Test101/Assets/Shaders/test.shader\"", // legacy
		"-file=asset/shader/demo/hello_triangle/hello_triangle.shader",

		"-out=dx11/pass0/.bin",
		"-profile=vs_5_0",
		"-entry=vs_main",

		"-I=../../abc/v",
		"-I=../../abc/c /c../..",
		"-I=../../built-in",

		"-DSGE_IS_INVERT_Y=",
		"-D_JJHHY=5584",
		"-I=../../built-in/shader",
		"-D_Hahahah",

		"-generateMake",
		"-makeCompile"

		//"-I=\"../../../built-in\"",
	}; RDS_UNUSED(argvs);
	#else
	char* argvs[] =
	{
		"rds_shader_compiler.exe",
		"-cwd=../../../../../example/Test000",
		//"-x=hlsl",
		//"-file=asset/shader/demo/hello_triangle/hello_triangle.shader",
		//"-file=asset/shader/demo/forward_plus/forward_plus.shader",
		//"-file=asset/shader/test/test_compute_bindless.shader",
		//"-file=asset/shader/debug/displayNormals.shader",
		//"-file=asset/shader/test/test_compute_bindless.shader",
		"-file=asset/shader/lighting/rdsDefaultLighting.shader",
		//"-file=asset/shader/ui/imgui.shader",
		//"-file=asset/shader/pass_feature/utility/image/rdsScreenQuad.shader",
		//"-file=asset/shader/test/test_non_bindless.shader",

		//"-out=dx11/pass0/.bin",
		//"-profile=vs_5_0",
		//"-entry=vs_main",

		//"-opt=spirv"

		"-generateMake",
		//"-makeCompile",

	}; RDS_UNUSED(argvs);
	#endif // 0
	#endif // RDS_TEST_SHADER_COMPILER

	using namespace rds;
	
	int exitCode = 0;

	MemoryContext::init();
	{
		rds::ShaderCompilerConsoleApp app;
		#if RDS_TEST_SHADER_COMPILER
		app.parseCmdLine(CmdLineArgsView{ argvs });
		#else
		app.parseCmdLine(CmdLineArgsView{argc, argv});
		#endif // 0
		exitCode = app.run();
	}
	MemoryContext::terminate();

	return exitCode;
}