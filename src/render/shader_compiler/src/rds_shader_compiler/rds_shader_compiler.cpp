#include "rds_shader_compiler-pch.h"

#include "parser/rdsShaderParser.h"
#include "compiler/rdsShaderCompiler_Vk.h"
#include "compiler/rdsShaderCompiler_Dx12.h"
#include <filesystem>

namespace rds
{

class ShaderCompilerConsoleApp : public ConsoleApp_Base
{
public:
	using SizeType	= RenderApiLayerTraits::SizeType;
	using Traits	= RenderApiLayerTraits;

public:

public:

	~ShaderCompilerConsoleApp()
	{
		destroy();
	}

protected:

	virtual void onRun()
	{
		create();

		ShaderCompileOption opt;
		opt.isDebug		= true;
		opt.isToSpirv	= true;
		opt.enableLog	= false;

		#if 1
		opt.isNoOffset	= false;
		compile("asset/shader/test/test.shader",					opt);
		compile("asset/shader/test/test_texture.shader",			opt);
		compile("asset/shader/test/test_texture_set0.shader",		opt);
		compile("asset/shader/test/test_compute.shader",			opt);

		opt.isNoOffset	= true;
		compile("asset/shader/ui/imgui.shader",					opt);
		compile("asset/shader/present.shader",					opt);
		compile("asset/shader/preDepth.shader",					opt);
		compile("asset/shader/gBuffer.shader",					opt);
		compile("asset/shader/deferredLighting.shader",			opt);
		compile("asset/shader/skybox.shader",					opt);

		compile("asset/shader/pbr/pbrBasic.shader",				opt);
		compile("asset/shader/pbr/pbrIbl.shader",				opt);
		compile("asset/shader/pbr/hdrToCube.shader",			opt);
		compile("asset/shader/pbr/irradianceEnvCube.shader",	opt);
		compile("asset/shader/pbr/prefilteredEnvCube.shader",	opt);
		compile("asset/shader/pbr/brdfLut.shader",				opt);

		opt.enableLog	= false; 
		compile("asset/shader/test/test_bindless.shader",			opt);
		compile("asset/shader/test/test_compute_bindless.shader",	opt);

		#endif // 0

		compile("asset/shader/demo/hello_triangle/hello_triangle.shader",	opt);
		
	}

	void compile(StrView filename, const ShaderCompileOption& opt)
	{
		TempString srcpath;
		Path::realpathTo(srcpath, filename);

		TempString dstDir = createOutpath(filename);
		throwIf(!checkValid(srcpath), "invalid filename / path {}", srcpath);

		ShaderInfo info;
		createShaderInfo(info, filename, dstDir);

		{
			StrView				binpath = Traits::s_spirvPath;
			ShaderCompiler_Vk	compilerVk;
			ShaderCompiler_Dx12	compilerDx12;

			Path::create(binpath);

			SizeType passIdx = 0;
			for (const auto& pass : info.passes)
			{
				TempString dstBinPath;
				fmtTo(dstBinPath, "{}/{}/pass{}", dstDir, binpath, passIdx);
				Path::create(dstBinPath);

				compilerDx12.compile(dstBinPath, srcpath, ShaderStageFlag::Compute,	pass.csFunc, opt);
				compilerDx12.compile(dstBinPath, srcpath, ShaderStageFlag::Vertex,	pass.vsFunc, opt);
				compilerDx12.compile(dstBinPath, srcpath, ShaderStageFlag::Pixel,	pass.psFunc, opt);

				auto opt2 = opt;
				opt2.isCompileBinary	= false;
				opt2.isReflect			= true;

				compilerVk.compile(dstBinPath, srcpath, ShaderStageFlag::Compute,	pass.csFunc, opt2);
				compilerVk.compile(dstBinPath, srcpath, ShaderStageFlag::Vertex,	pass.vsFunc, opt2);
				compilerVk.compile(dstBinPath, srcpath, ShaderStageFlag::Pixel,		pass.psFunc, opt2);

				TempString dstAllStageUnionInfoPath;
				fmtTo(dstAllStageUnionInfoPath, "{}/pass{}", dstBinPath, passIdx);
				compilerVk.writeAllStageUnionInfo(dstAllStageUnionInfoPath);
			}
		}
	}

	bool checkValid(StrView filename)
	{
		return Path::isExist(filename);
	}

	void createShaderInfo(ShaderInfo& outInfo, StrView filename, StrView outDir)
	{
		ShaderParser parser;
		parser.parse(outInfo, filename);

		TempString outpath;
		fmtTo(outpath, "{}/info.json", outDir);
		parser.dump(outpath);
	}

	TempString createOutpath(StrView filename)
	{
		TempString out;

		TempString base;
		fmtTo(base, "{}/asset", Path::getCurrentDir());

		TempString filenameRealpath;
		Path::realpathTo(filenameRealpath, filename);

		TempString dstpath;
		Path::relativeTo(dstpath, filenameRealpath, base);

		toOutpath(out, filename);
		Path::create(out);
		//Path::setCurrentDir(out);

		return out;
	}

	void create()
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
	}

	void destroy()
	{
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

		Path::create(Traits::s_defaultShaderOutPath);
	}

	template<class STR>
	static void toOutpath(STR& out, StrView fileBasename)
	{
		fmtTo(out, "{}/{}", Traits::s_defaultShaderOutPath, fileBasename);
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