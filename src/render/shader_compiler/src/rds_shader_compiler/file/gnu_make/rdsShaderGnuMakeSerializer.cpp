#include "rds_shader_compiler-pch.h"
#include "rdsShaderGnuMakeSerializer.h"


namespace rds
{

#if 0
#pragma mark --- rdsShaderGnuMakeSerializer-Decl ---
#endif // 0
#if 1

void 
ShaderGnuMakeSerializer::dump(StrView filename, const ShaderCompileRequest& cReq)
{
	//auto& ps = *ProjectSetting::instance();

	ShaderGnuMakeSerializer make(cReq);

	auto& request	= make._request;
	auto& names		= gnuMakeVarName;			RDS_UNUSED(names);

	make.phony({"all", "clean"});

	make._init_includes(request);

	make._init_variables(request);

	//make._init_build_target(request, ApiType::Dx11);	make.nextLine();
	make._init_build_target(request, ApiType::Vulkan);	make.nextLine();

	RDS_LOG_DEBUG("=== Begin ShaderGnuMakeSerializer::dump-filename: {}", filename);
	make.flush(filename);
	RDS_LOG_DEBUG("=== End ShaderGnuMakeSerializer::dump-filename: {}", filename);
}

ShaderGnuMakeSerializer::ShaderGnuMakeSerializer(const ShaderCompileRequest& cReq)
	: Base(_request)
{
	_request.compileRequest = &cReq;
}


void 
ShaderGnuMakeSerializer::_init_variables(Request& request)
{
	auto& cReq	  = *request.compileRequest; RDS_UNUSED(cReq);
	ShaderGnuMake_VarName names;

	TempString tmp;

	assignVariable(names.currentMakeFile,		":=", "$(lastword $(MAKEFILE_LIST))");
	/*assignVariable(names.rdsRoot,				":=", "");		
	assignVariable(names.projectRoot,			":=", "");	
	assignVariable(names.shaderCompilerPath,	":=", cReq.compileInfo.compilerPath);
	assignVariable(names.shaderCompiler,		":=", cReq.compileInfo.shaderCompiler);
	assignVariable(names.glslc,					":=", "glslc");
	assignVariable(names.shaderFilePath,		":=", cReq.inputFilename);*/

	nextLine();
}

void 
ShaderGnuMakeSerializer::_init_includes(Request& request)
{
	auto& cReq	  = *request.compileRequest; RDS_UNUSED(cReq);
	auto& names	  = gnuMakeVarName;			  RDS_UNUSED(names);

	//includePath(cReq.compilerMakePath);
	//includePath(cReq.projectMakePath);

	nextLine();
}

void 
ShaderGnuMakeSerializer::_init_build_target(Request& request, ApiType apiType)
{
	auto& cReq	  = *request.compileRequest; RDS_UNUSED(cReq);
	auto& names	  = gnuMakeVarName;			  RDS_UNUSED(names);

	auto target = ShaderCompileUtil::getBuildTargetName(apiType);

	auto ifeq = Ifeq::ctor(request, target, "1");
	nextLine();

	size_t passIndex = 0;
	for (auto& pass : cReq.shaderInfo.passes) {

		BuildRequest bReq(pass.vsFunc, ShaderStageFlag::Vertex, passIndex, apiType);

		if (bReq.build(this, request, pass.csFunc, ShaderStageFlag::Compute,	passIndex, apiType)) continue;
		if (bReq.build(this, request, pass.vsFunc, ShaderStageFlag::Vertex,		passIndex, apiType)) continue;
		if (bReq.build(this, request, pass.psFunc, ShaderStageFlag::Pixel,		passIndex, apiType)) continue;

		passIndex++;
	}
}

bool 
ShaderGnuMakeSerializer::_init_bin(Request& request, BuildRequest& buildReq)
{
	auto& cReq	  = *request.compileRequest;	RDS_UNUSED(cReq);
	auto& names	  = gnuMakeVarName;				RDS_UNUSED(names);

	auto& entry		= *buildReq._entry;		RDS_UNUSED(entry);
	auto& stage		=  buildReq._stage;		RDS_UNUSED(stage);
	auto& passIndex	=  buildReq._passIndex;	RDS_UNUSED(passIndex);
	auto& apiType	=  buildReq._apiType;	RDS_UNUSED(apiType);

	const char* compilerName = ShaderCompileUtil::getCompilerName(apiType);

	generateDepdency(request, buildReq);

	TempString tmp;
	tmp.clear();

	bool isLastBin = false;
	const auto* apiName = ShaderCompileUtil::getBuildBinName(apiType);

	if (entry.size()) 
	{
		auto profile = ShaderCompileUtil::getStageProfile(stage, apiType);
		//auto binName = fmtAs_T<TempString>("$({})/pass{}/{}.bin", apiName, passIndex, profile);
		auto binVarName = fmtAs_T<TempString>("{}_pass{}_{}_bin", apiName, passIndex, profile);

		TempString dstDir;
		TempString binDir;
		TempString binVar;

		ShaderCompileUtil::getDstDirTo(dstDir, cReq.inputFilename, *ProjectSetting::instance());
		ShaderCompileUtil::getBinDirTo(binDir, dstDir, passIndex, apiType);
		ShaderCompileUtil::getBinFilepathTo(tmp, binDir, stage, apiType);

		assignVariable(binVarName,	":=", tmp);
		getVariableTo(binVar, binVarName);

		{ auto target0 = Target::ctor(request, "all", { binVar });  }
		{ tmp = binVar; tmp += ".dep";  includePath(tmp); }
		{
			auto target1 = Target::ctor(request, binVar);
			write(" "); write(toVariable(names.currentMakeFile));
			write(" "); write(toVariable(names.shaderFileRoot));
			if (compilerName) { write(" "); write(toVariable(compilerName)); }
			nextLine();
			isLastBin = _init_cli(request, buildReq);
			nextLine();
		}
	}

	return isLastBin;
}

bool 
ShaderGnuMakeSerializer::_init_cli(Request& request, BuildRequest& buildReq)
{
	auto& cReq	  = *request.compileRequest;	RDS_UNUSED(cReq);
	auto& names	  = gnuMakeVarName;				RDS_UNUSED(names);

	auto& entry		= *buildReq._entry;		RDS_UNUSED(entry);
	auto& stage		=  buildReq._stage;		RDS_UNUSED(stage);
	auto& passIndex	=  buildReq._passIndex;	RDS_UNUSED(passIndex);
	auto& apiType	=  buildReq._apiType;	RDS_UNUSED(apiType);

	using SRC = ApiType;
	switch (apiType)
	{
		//case SRC::Dx11:		{ return _init_cli_rdsShaderCompiler(request, buildReq); } break;
		case SRC::Vulkan:	{ return _init_cli_rdsShaderCompiler(request, buildReq); } break;
		default: { RDS_THROW("unknow api type, getBuildBinName()"); }
	}
}

bool 
ShaderGnuMakeSerializer::_init_cli_rdsShaderCompiler(Request& request, BuildRequest& buildReq)
{
	auto& cReq	  = *request.compileRequest; RDS_UNUSED(cReq);
	auto& names	  = gnuMakeVarName;			  RDS_UNUSED(names);

	onWrite(toVariable(names.shaderCompiler));

	{ auto bcmd = BeginCmd::ctor(request); write("-cwd=");			auto bstr = BeginString::ctor(request, toVariable(names.projectRoot)); }

	{ auto bcmd = BeginCmd::ctor(request); write("-makeCompile"); }
	{ auto bcmd = BeginCmd::ctor(request); write("-generateMake"); }

	#if 0
	auto& cReq	  = cReq.comileRequest;  RDS_UNUSED(cReq);
	const auto* profie	= getStageProfile(buildReq._stage, buildReq._apiType);
	auto& entry			= *buildReq._entry;

	{ auto bcmd = BeginCmd::ctor(request); write("-x=");			write(cReq.language); }
	{ auto bcmd = BeginCmd::ctor(request); write("-profile=");		write(profie); }
	{ auto bcmd = BeginCmd::ctor(request); write("-entry=");		write(entry); }		
	{ auto bcmd = BeginCmd::ctor(request); write("-file=");			auto bstr = BeginString::ctor(request, toVariable(names.shaderFilePath)); }
	{ auto bcmd = BeginCmd::ctor(request); write("-out=");			auto bstr = BeginString::ctor(request, "$@"); }		
	{ 
		_init_cli_include(request, buildReq, "-I=");
		_init_cli_marco(request, buildReq, "-D");
	}
	#else

	{ auto bcmd = BeginCmd::ctor(request); write("-file=");			auto bstr = BeginString::ctor(request, toVariable(names.shaderFilePath)); }

	#endif // 0

	return true;
}

bool 
ShaderGnuMakeSerializer::_init_cli_glslc(Request& request, BuildRequest& buildReq)
{
	auto& cReq	  = *request.compileRequest;	RDS_UNUSED(cReq);
	auto& names	  = gnuMakeVarName;				RDS_UNUSED(names);

	onWrite("cd "); write(toVariable(names.projectRoot)); write(" && ");
	write(toVariable(names.glslc));

	StrView profie	= ShaderCompileUtil::getGlslcStageProfile(buildReq._stage);
	auto& entry		= *buildReq._entry;

	// -MD must be behide -fshader-stage and -fentry-point

	{ auto bcmd = BeginCmd::ctor(request); write("-x ");				write(cReq.language); }
	{ auto bcmd = BeginCmd::ctor(request); write("-fshader-stage=");	write(profie); }
	{ auto bcmd = BeginCmd::ctor(request); write("-fentry-point=");		write(entry); }	
	{ auto bcmd = BeginCmd::ctor(request); write("-MD ");				{ auto bstr = BeginString::ctor(request); write(toVariable(names.shaderFilePath)); } }
	{ auto bcmd = BeginCmd::ctor(request); write("-o ");				{ auto bstr = BeginString::ctor(request); write(toVariable(names.shaderBinRoot)); write("/"); write("$@"); } }
	{
		_init_cli_include(request, buildReq, "-I");
		_init_cli_marco(request, buildReq, "-D");
	}

	return false;
}

void 
ShaderGnuMakeSerializer::_init_cli_include(Request& request, BuildRequest& buildReq, StrView syntax)
{
	auto& cReq	  = *request.compileRequest;

	for (auto& inc : cReq.includes.dirs())
	{
		auto bcmd = BeginCmd::ctor(request); write(syntax);
		auto bstr = BeginString::ctor(request);
		write(inc);
	}
}

void 
ShaderGnuMakeSerializer::_init_cli_marco(Request& request, BuildRequest& buildReq, StrView syntax)
{
	auto& cReq	  = *request.compileRequest;

	for (auto& mar : cReq.marcos)
	{
		auto bcmd = BeginCmd::ctor(request); write(syntax);
		auto bstr = BeginString::ctor(request);
		write(mar.name); write(" = "); write(mar.value); 
	}
}

void
ShaderGnuMakeSerializer::generateDepdency(Request& request, BuildRequest& buildReq)
{
	auto& cReq		= *request.compileRequest;	RDS_UNUSED(cReq);
	auto& names		= gnuMakeVarName;			RDS_UNUSED(names);

	auto& entry		= *buildReq._entry;		RDS_UNUSED(entry);
	auto& stage		=  buildReq._stage;		RDS_UNUSED(stage);
	auto& passIndex	=  buildReq._passIndex;	RDS_UNUSED(passIndex);
	auto& apiType	=  buildReq._apiType;	RDS_UNUSED(apiType);

	StrView apiName = ShaderCompileUtil::getBuildBinName(apiType);
	StrView profile	= ShaderCompileUtil::getStageProfile(stage, apiType);
	StrView format	= ShaderCompileUtil::getShaderFormat(apiType);

	TempString tmp;
	TempString content;
	TempString binName = fmtAs_T<TempString>("$({})/pass{}/{}.bin", apiName, passIndex, profile);

	content.reserve(4096);
	fmtTo(tmp, "local_temp/Imported/{}/{}/pass{}/{}.bin.dep", cReq.inputFilename, format, passIndex, profile);

	content += binName; content += ":\\\n";

	for (auto& inc : cReq.includes.files())
	{
	{
		content += "\t";
		content += inc;
		content += "\\\n";
	}

	File::writeFileIfChanged(tmp, content, false);
}

#endif


}


}