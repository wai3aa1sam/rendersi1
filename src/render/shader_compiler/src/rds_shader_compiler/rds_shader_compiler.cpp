#include "rds_shader_compiler-pch.h"

#include "parser/rdsShaderParser.h"
#include "compiler/rdsShaderCompiler_Vk.h"
#include "compiler/rdsShaderCompiler_Dx12.h"

namespace rds
{

class ShaderCompilerConsoleApp : public ConsoleApp_Base
{
public:

protected:
	virtual void onRun()
	{
		Logger::init();
		JobSystem::init();
		Renderer::init();

		Logger::instance()->create(Logger::makeCDesc());
		{
			auto cDesc = JobSystem::makeCDesc();
			cDesc.workerCount = OsTraits::logicalThreadCount();
			JobSystem::instance()->create(cDesc);
		}
		{
			auto cDesc = Renderer::makeCDesc();
			Renderer::instance()->create(cDesc);
		}

		setRootPath();

		ShaderCompileOption opt;
		opt.isDebug		= true;
		opt.isToSpirv	= true;
		opt.enableLog	= true;

		ShaderCompiler_Vk compiler;
		//ShaderCompiler_Dx12 compiler;

		compiler.compile("LocalTemp/my_terrain.vert.spv", "asset/shader/terrain/my_terrain.shader", ShaderStageFlag::Vertex, "vs_main", opt);
		//compiler.compile("hello_triangle0.frag.spv", "asset/shader/vulkan/hello_triangle0.hlsl", ShaderStageFlag::Pxiel, "ps_main", opt);


		Renderer::terminate();
		JobSystem::terminate();
		Logger::terminate();
	}

	void setRootPath()
	{
		String file = getExecutableFilename();
		String path = Path::dirname(file);

		//auto* proj = ProjectSettings::instance();

		path.append("/../../../../../..");
		path.append("/example/Test000");
		Directory::setCurrent(path);

		static constexpr StrView s_importPath = "LocalTemp";
		Path::create(s_importPath);
	}
};


}

int main(int argc, char* argv[])
{
	using namespace rds;

	int exitCode = 0;

	MemoryContext::init();
	{
		ShaderCompilerConsoleApp app;
		app.run();
	}
	MemoryContext::terminate();

	return exitCode;
}