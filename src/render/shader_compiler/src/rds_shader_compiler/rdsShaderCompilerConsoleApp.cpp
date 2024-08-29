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
ShaderCompilerConsoleApp::parseCmdLine(const CmdLineArgsView& cmdArgs) 
{
	ShaderCmdLineParser parser;
	parser.readCmdLineArgsView(&_compileDesc, cmdArgs);
	_compileDesc.compileOption.isDebug = RDS_DEBUG;
}

void 
ShaderCompilerConsoleApp::create()
{
	{
		ProjectSetting::init();

		Logger::init();
		#if RDS_USE_JOB_SYSTEM
		JobSystem::init();
		#endif // RDS_JOB_SYSTEM
		#if RDS_USE_RENDERER
		Renderer::init();
		#endif // RDS_USE_RENDERER

		Logger::instance()->create(Logger::makeCDesc());
		#if RDS_USE_JOB_SYSTEM
		{
			auto cDesc = JobSystem::makeCDesc();
			cDesc.workerCount = 0;
			JobSystem::instance()->create(cDesc);
		}
		#endif

		#if RDS_USE_RENDERER
		{
			auto cDesc = Renderer::makeCDesc();
			cDesc.isUsingForCompileShader = true;
			Renderer::instance()->create(cDesc);
		}
		#endif
	}

	setRootPath();

	auto& cmpDesc = _compileDesc;
	{
		auto& ps = projectSetting();
		auto& includes = cmpDesc.includes;
		includes.emplaceBackDir() = ps.rdsRoot();
		includes.emplaceBackDir() = ps.buildInRoot();
		includes.emplaceBackDir() = ps.buildInShaderRoot();
		includes.emplaceBackDir() = Path::dirname(cmpDesc.inputFilename);
	}
	{
		auto& bindlessRsc = Renderer::renderDevice()->bindlessResource();
		auto& marcos = cmpDesc.marcos;

		_isPermutationCompile = !marcos.is_empty();

		marcos.emplace_back("RDS_K_SAMPLER_COUNT",			StrUtil::toTempStr(BindlessResources::supportSamplerCount()));
		marcos.emplace_back("RDS_CONSTANT_BUFFER_SPACE",	fmtAs_T<TempString>("space{}",	BindlessResources::bindlessTypeCount()));

		auto minBinding_vkSpec = 16;
		cmpDesc.globalBinding	= minBinding_vkSpec - 2;
		cmpDesc.globalSet		= sCast<int>(bindlessRsc.bindlessTypeCount());
	}
}

void 
ShaderCompilerConsoleApp::destroy()
{
	#if RDS_USE_RENDERER
	Renderer::terminate();
	#endif // RDS_USE_RENDERER

	#if RDS_USE_JOB_SYSTEM
	JobSystem::terminate();
	#endif

	Logger::terminate();

	ProjectSetting::terminate();
}

int
ShaderCompilerConsoleApp::onRun()
{
	create();
	try
	{
		compile(_compileDesc);
		return 0;
	}
	catch (const std::exception&)
	{
		return -1;
	}
}

void 
ShaderCompilerConsoleApp::compile(StrView filename, const ShaderCompileOption& opt)
{
	auto& cmpDesc = compileDesc();

	if (filename.is_empty())
	{
		return;
	}
	
	TempString srcFileRoot;
	Path::realpathTo(srcFileRoot, filename);
	throwIf(!checkValid(srcFileRoot), "invalid filename / path {}", srcFileRoot);

	TempString dstDir;
	if (cmpDesc.outputFilename.is_empty())
	{
		createImportedShaderDirTo(dstDir, filename);
	}
	else
	{
		dstDir = Path::basename(cmpDesc.outputFilename);
	}

	ShaderInfo& info = cmpDesc.shaderInfo;
	createShaderInfo(&cmpDesc, &info, filename, dstDir);

	createShaderPermutationDirTo(dstDir);

	TempString dstDirRoot;
	Path::realpathTo(dstDirRoot, dstDir);

	if (cmpDesc.isGNUMakeCompile)
	{
		compileForVulkan(info, srcFileRoot, dstDirRoot, opt);
		markForHotReload(filename);
	}
	else
	{
		compileForVulkan(info, srcFileRoot, dstDirRoot, opt);
	}

	if (cmpDesc.isGenerateMake)
	{
		TempString buf;
		ShaderCompileRequest::getMakefilePathTo(buf, dstDir);
		ShaderGnuMakeSerializer::dump(buf, cmpDesc);
	}
}

void 
ShaderCompilerConsoleApp::compile(const ShaderCompileDesc& cReq)
{
	compile(cReq.inputFilename, cReq.compileOption);
}

void 
ShaderCompilerConsoleApp::compileForVulkan(const ShaderInfo& info, StrView srcFileRoot, StrView dstDirRoot, const ShaderCompileOption& opt)
{
	RDS_CORE_ASSERT(opt.apiType == RenderApiType::Vulkan, "");

	auto& cmpDesc = compileDesc();

	TempString binDir;
	ShaderCompileRequest::getBinDirTo(binDir, dstDirRoot, RenderApiType::Vulkan);

	ShaderCompiler_Vk	compilerVk;
	ShaderCompiler_Dx12	compilerDx12;

	for (size_t iPass = 0; iPass < info.passes.size(); ++iPass) 
	{
		const auto& pass = info.passes[iPass];

		TempString binPassDir;
		ShaderCompileRequest::getBinPassDirTo(binPassDir, binDir, iPass);
		Path::create(binPassDir);

		bool hasGeometryShader = !pass.geomFunc.is_empty();
		auto opt_invret_y = opt;
		//opt_invret_y.isInvertY = true;

		compilerDx12.compile(ShaderStageFlag::Compute,					srcFileRoot, binPassDir, pass.csFunc,	opt, cmpDesc);
		compilerDx12.compile(ShaderStageFlag::Vertex,					srcFileRoot, binPassDir, pass.vsFunc,	hasGeometryShader ? opt : opt_invret_y, cmpDesc);
		compilerDx12.compile(ShaderStageFlag::TessellationControl,		srcFileRoot, binPassDir, pass.tescFunc, opt, cmpDesc);
		compilerDx12.compile(ShaderStageFlag::TessellationEvaluation,	srcFileRoot, binPassDir, pass.teseFunc, opt, cmpDesc);
		compilerDx12.compile(ShaderStageFlag::Geometry,					srcFileRoot, binPassDir, pass.geomFunc, hasGeometryShader ? opt_invret_y : opt, cmpDesc);
		compilerDx12.compile(ShaderStageFlag::Pixel,					srcFileRoot, binPassDir, pass.psFunc,	opt, cmpDesc);

		auto opt2 = opt;
		opt2.isCompileBinary	= false;
		opt2.isReflect			= true;

		compilerVk.compile(ShaderStageFlag::Compute,					srcFileRoot, binPassDir, pass.csFunc,	opt2, cmpDesc);
		compilerVk.compile(ShaderStageFlag::Vertex,						srcFileRoot, binPassDir, pass.vsFunc,	opt2, cmpDesc);
		compilerVk.compile(ShaderStageFlag::TessellationControl,		srcFileRoot, binPassDir, pass.tescFunc, opt2, cmpDesc);
		compilerVk.compile(ShaderStageFlag::TessellationEvaluation,		srcFileRoot, binPassDir, pass.teseFunc, opt2, cmpDesc);
		compilerVk.compile(ShaderStageFlag::Geometry,					srcFileRoot, binPassDir, pass.geomFunc, opt2, cmpDesc);
		compilerVk.compile(ShaderStageFlag::Pixel,						srcFileRoot, binPassDir, pass.psFunc,	opt2, cmpDesc);

		TempString allStageUnionInfoFilepath;
		ShaderCompileRequest::getAllStageUnionInfoFilepathTo(allStageUnionInfoFilepath, binPassDir, iPass);
		compilerVk.writeAllStageUnionInfo(allStageUnionInfoFilepath);
	}
}

void 
ShaderCompilerConsoleApp::createShaderInfo(ShaderCompileDesc* oCmpDesc, ShaderInfo* outInfo, StrView filename, StrView outDir)
{
	ShaderParser parser;
	parser.parse(oCmpDesc, outInfo, filename);

	TempString outpath;
	ShaderCompileRequest::getShaderInfoFilepathTo(outpath, outDir);
	parser.dump(outpath);
}

void 
ShaderCompilerConsoleApp::createImportedShaderDirTo(TempString& oStr, StrView filename)
{
	auto& ps	= projectSetting();
	auto& out	= oStr;

	out.reserve(filename.size());
	if (out.is_empty())
	{
		ShaderCompileRequest::getImportedShaderDirTo(oStr, filename, ps);
	}
	Path::create(out);
}

void 
ShaderCompilerConsoleApp::createShaderPermutationDirTo(TempString& outDir)
{
	auto& ps		= projectSetting();
	auto& cmpDesc	= compileDesc();

	if (_isPermutationCompile)
	{
		TempString impShaderDir;
		ShaderCompileRequest::getImportedShaderDirTo(impShaderDir, cmpDesc.inputFilename, ps);

		ShaderPermutations::getNameTo(cmpDesc.permutName, cmpDesc.shaderInfo.permuts, cmpDesc.marcos);
		if (cmpDesc.permutName.is_empty())
		{
			return;
		}
		ShaderCompileRequest::getShaderPermutationDirTo(outDir, impShaderDir, cmpDesc.permutName, ps);
		Path::create(outDir);
	}
}

void 
ShaderCompilerConsoleApp::setRootPath()
{
	auto& compileInfo = compileDesc().compileInfo;

	auto& ps = projectSetting();
	{
		String file = getExecutableFilename();
		String path = Path::dirname(file);

		path.append("/../../../../../..");
		ps.setRdsRoot(path);
		//RDS_DUMP_VAR(Path::getCurrentDir());
		//RDS_DUMP_VAR(compileInfo.cwd);

		ps.setProjectRoot(compileInfo.cwd);

		compileInfo.rdsRoot = ps.rdsRoot();
	}
}

void 
ShaderCompilerConsoleApp::markForHotReload(StrView inputFilename)
{
	auto& ps=  projectSetting();

	TempString tmp;
	auto pair = StrUtil::splitByChar(inputFilename, "\\");
	while (pair.first.size() != 0)
	{
		tmp += pair.first;
		if (pair.second.size() != 0)
			tmp += "/";
		pair = StrUtil::splitByChar(pair.second, "\\");
	}
	tmp += "\n";

	FileStream fs;
	fs.openWrite(ps.shaderRecompileListPath(), false);
	auto lock = fs.scopedLock();
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