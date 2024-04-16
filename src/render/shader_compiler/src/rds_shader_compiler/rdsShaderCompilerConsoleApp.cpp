#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompilerConsoleApp.h"
#include "file/gnu_make/rdsShaderGnuMakeSerializer.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompilerConsoleApp-Impl ---
#endif // 0
#if 1

ShaderCompilerConsoleApp::~ShaderCompilerConsoleApp()
{
	destroy();
}

void 
ShaderCompilerConsoleApp::parseCmdLine(const CmdLineArgs& cmdArgs) 
{
	ShaderCmdLineParser parser;
	parser.readCmdLineArgs(&compileRequest, cmdArgs);
}

void 
ShaderCompilerConsoleApp::create()
{
	{
		ProjectSetting::init();

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
	}

	setRootPath();

	{
		auto& ps = projectSetting();
		auto& includes = compileRequest.includes;
		includes.emplaceBackDir() = ps.rdsRoot();
		includes.emplaceBackDir() = ps.buildInRoot();
		includes.emplaceBackDir() = ps.buildInShaderRoot();
		includes.emplaceBackDir() = Path::dirname(compileRequest.inputFilename);
	}
}

void 
ShaderCompilerConsoleApp::destroy()
{
	Renderer::terminate();
	JobSystem::terminate();
	Logger::terminate();

	ProjectSetting::terminate();
}

void 
ShaderCompilerConsoleApp::onRun()
{
	create();
	compile(compileRequest);
}

void 
ShaderCompilerConsoleApp::compile(StrView filename, const ShaderCompileOption& opt)
{
	auto& cReq = compileRequest;

	if (filename.is_empty())
	{
		return;
	}
	
	TempString srcpath;
	Path::realpathTo(srcpath, filename);
	throwIf(!checkValid(srcpath), "invalid filename / path {}", srcpath);

	createOutpathTo(cReq.outputFilename, filename);
	TempString dstDir = cReq.outputFilename;		// need to copy

	ShaderInfo& info = cReq.shaderInfo;
	createShaderInfo(&cReq, &info, filename, dstDir);

	permutationOutputPathTo(dstDir);

	if (cReq.isGNUMakeCompile)
	{
		compileForVulkan(info, srcpath, dstDir, opt);
		markForHotReload(srcpath);
	}
	else
	{
		compileForVulkan(info, srcpath, dstDir, opt);
	}

	if (cReq.isGenerateMake)
	{
		TempString buf;
		fmtTo(buf, "{}/makeFile", cReq.outputFilename);
		ShaderGnuMakeSerializer::dump(buf, cReq);
	}
}

void 
ShaderCompilerConsoleApp::compile(const ShaderCompileRequest& cReq)
{
	compile(cReq.inputFilename, cReq.compileOption);
}

void 
ShaderCompilerConsoleApp::compileForVulkan(const ShaderInfo& info, StrView srcpath, StrView dstDir, const ShaderCompileOption& opt)
{
	auto& ps = projectSetting();

	StrView				binpath = ps.spirvPath();
	ShaderCompiler_Vk	compilerVk;
	ShaderCompiler_Dx12	compilerDx12;

	for (size_t iPass = 0; iPass < info.passes.size(); ++iPass) 
	{
		const auto& pass = info.passes[iPass];

		TempString dstBinPath;
		fmtTo(dstBinPath, "{}/{}/pass{}", dstDir, binpath, iPass);
		Path::create(dstBinPath);

		compilerDx12.compile(dstBinPath, srcpath, ShaderStageFlag::Compute,	pass.csFunc, opt, compileRequest);
		compilerDx12.compile(dstBinPath, srcpath, ShaderStageFlag::Vertex,	pass.vsFunc, opt, compileRequest);
		compilerDx12.compile(dstBinPath, srcpath, ShaderStageFlag::Pixel,	pass.psFunc, opt, compileRequest);

		auto opt2 = opt;
		opt2.isCompileBinary	= false;
		opt2.isReflect			= true;

		compilerVk.compile(dstBinPath, srcpath, ShaderStageFlag::Compute,	pass.csFunc, opt2, compileRequest);
		compilerVk.compile(dstBinPath, srcpath, ShaderStageFlag::Vertex,	pass.vsFunc, opt2, compileRequest);
		compilerVk.compile(dstBinPath, srcpath, ShaderStageFlag::Pixel,		pass.psFunc, opt2, compileRequest);

		TempString dstAllStageUnionInfoPath;
		fmtTo(dstAllStageUnionInfoPath, "{}/pass{}", dstBinPath, iPass);
		compilerVk.writeAllStageUnionInfo(dstAllStageUnionInfoPath);
	}
}

bool permutNameTo(String& o, const ShaderCompileRequest& compileRequest)
{
	const ShaderInfo& info	= compileRequest.shaderInfo;
	const auto& cReq		= compileRequest;

	if (info.permuts.size() == 0)
		return false;

	TempString tmp;
	//tmp += "_";
	for (size_t i = 0; i < info.permuts.size(); i++)
	{
		const auto& permut = info.permuts[i];
		int valueIdx = -1;
		const ShaderMarco* targetMar = nullptr;
		for (auto& mar : cReq.marcos)
		{
			if (mar.name == permut.name)
			{
				targetMar = &mar;
				break;
			}
		}
		if (!targetMar)
			return false;
		valueIdx = permut.findValueIdx(targetMar->value);
		if (valueIdx == -1)
			return false;
		tmp += StrUtil::toStr(valueIdx);
	}

	o = tmp;
	return true;
}

void 
ShaderCompilerConsoleApp::permutationOutputPathTo(TempString& outputPath)
{
	auto& ps = projectSetting();
	auto& cReq = compileRequest;

	if (cReq.marcos.size() > 0)
	{
		if (cReq.permutName.size() == 0)
			permutNameTo(cReq.permutName, cReq);
		if (cReq.permutName.size() == 0)
			return;
		fmtTo(outputPath, "/{}/{}", ps.shaderPermutationPath(), cReq.permutName);
		Path::create(outputPath);
	}
}

void 
ShaderCompilerConsoleApp::createShaderInfo(ShaderCompileRequest* oCReq, ShaderInfo* outInfo, StrView filename, StrView outDir)
{
	ShaderParser parser;
	parser.parse(oCReq, outInfo, filename);

	TempString outpath;
	fmtTo(outpath, "{}/info.json", outDir);
	parser.dump(outpath);
}

void 
ShaderCompilerConsoleApp::createOutpathTo(String& oStr, StrView filename)
{
	auto& ps	= projectSetting();
	auto& out	= oStr;

	if (out.is_empty())
	{
		fmtTo(out, "{}/{}/{}", ps.projectRoot(), ps.importedShaderPath(), filename);
	}
	Path::create(out);
}

//void 
//ShaderCompilerConsoleApp::createBinpath(const ShaderInfo& info, StrView outputPath, RenderApiType type)
//{
//	auto apiPath = ShaderCompileUtil::getBuildApiPath(type);
//	TempString passOutPath;
//
//	for (size_t iPass = 0; info.passes.size(); ++iPass) 
//	{
//		passOutPath.clear();
//		fmtTo(passOutPath, "{}/{}/pass{}", outputPath, apiPath, iPass);
//		Path::create(passOutPath);
//	}
//}

void 
ShaderCompilerConsoleApp::setRootPath()
{
	auto& compileInfo = compileRequest.compileInfo;

	auto& ps = projectSetting();
	{
		String file = getExecutableFilename();
		String path = Path::dirname(file);

		path.append("/../../../../../..");
		ps.setRdsRoot(path);
		ps.setProjectRoot(compileInfo.cwd);

		compileInfo.rdsRoot = ps.rdsRoot();
	}
}

void 
ShaderCompilerConsoleApp::markForHotReload(StrView inputFilename)
{
	auto& ps=  projectSetting();

	FileStream fs;
	fs.openWrite(ps.shaderRecompileListPath(), false);

	TempString tmp;

	//tmp += "\"";

	auto pair = StrUtil::splitByChar(inputFilename, "\\");
	while (pair.first.size() != 0)
	{
		tmp += pair.first;
		if (pair.second.size() != 0)
			tmp += "/";
		pair = StrUtil::splitByChar(pair.second, "\\");
	}
	
	tmp += "\n";
	fs.setPos(fs.fileSize());
	fs.writeBytes(makeByteSpan(tmp));
}

bool 
ShaderCompilerConsoleApp::checkValid(StrView filename) const
{
	return Path::isExist(filename);
}

#endif

}