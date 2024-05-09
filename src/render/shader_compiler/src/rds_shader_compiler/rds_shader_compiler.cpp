#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompilerConsoleApp.h"


namespace rds
{

ShaderGnuMake_VarName gnuMakeVarName;

}

int main(int argc, char* argv[])
{
	#if 0
	char* argvs[] =
	{
		"rds_shader_compiler.exe",
		"-cwd=../../../../../examples/Test101",
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
		"-file=asset/shader/test/test_compute_bindless.shader",
		//"-file=asset/shader/demo/forward_plus/forward_plus.shader",
		//"-file=asset/shader/test/test_compute_bindless.shader",
		"-file=asset/shader/debug/displayNormals.shader",

		//"-out=dx11/pass0/.bin",
		//"-profile=vs_5_0",
		//"-entry=vs_main",

		//"-opt=spirv"

		"-generateMake",
		//"-makeCompile",

	}; RDS_UNUSED(argvs);
	#endif // 0


	using namespace rds;
	
	int exitCode = 0;

	MemoryContext::init();
	{
		rds::ShaderCompilerConsoleApp app;
		#if 1
		app.parseCmdLine(CmdLineArgsView{argc, argv});
		#else
		app.parseCmdLine(CmdLineArgsView{ argvs });
		#endif // 0
		app.run();
	}
	MemoryContext::terminate();

	return exitCode;
}