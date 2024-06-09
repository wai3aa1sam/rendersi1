#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderPermutations.h"

namespace rds
{

class Shader;

#if 0
#pragma mark --- rdsShaderCompileRequest-Decl ---
#endif // 0
#if 1

class ShaderCompileRequest : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Permutations = ShaderPermutations;

public:
	static bool hotReload(Renderer* renderer, JobSystem* jobSystem, const ProjectSetting* projectSetting);

public:
	template<class STR> static void getImportedShaderDirTo(			STR& o, StrView filename,		const ProjectSetting& ps);
	template<class STR> static void getShaderPermutationDirTo(		STR& o, StrView impShaderDir,	StrView permutName, const ProjectSetting& projSetting);
	template<class STR> static void getImportedBinDirTo(			STR& o, StrView impShaderDir,	RenderApiType apiType, const ShaderPermutations& permuts, const ProjectSetting& ps);
	template<class STR> static void getBinDirTo(					STR& o, StrView impShaderDir,	RenderApiType apiType);
	template<class STR> static void getBinPassDirTo(				STR& o, StrView binDir,			SizeType iPass);
	template<class STR> static void getBinFilepathTo(				STR& o, StrView binPassDir,		ShaderStageFlag stage, RenderApiType apiType);
	template<class STR> static void getShaderInfoFilepathTo(		STR& o, StrView impShaderDir);
	template<class STR> static void getShaderStageInfoFilepathTo(	STR& o, StrView binFilepath);
	template<class STR> static void getAllStageUnionInfoFilepathTo(	STR& o, StrView binPassDir,		SizeType iPass);
	template<class STR> static void getBinDependencyFilepathTo(		STR& o, StrView binFilepath);
	template<class STR> static void getMakefilePathTo(				STR& o, StrView binFilepath);

private:
	static void			_hotReloadShader(StrView filename, RenderDevice* renderDevice, const ProjectSetting& projectSetting);
	static void			reloadPermutation(RenderDevice* renderDevice, const ProjectSetting& projectSetting);
	static SPtr<Shader> compilePermutationShader(StrView filename, StrView impShaderDir, const Permutations& permuts, RenderDevice& renderDevice, const ProjectSetting& projectSetting);
};

template<class STR> inline
void 
ShaderCompileRequest::getImportedShaderDirTo(STR& o, StrView filename, const ProjectSetting& ps)
{
	o.clear();

	auto& shaderImportedDir	= o;
	fmtTo(shaderImportedDir, "{}/{}", ps.importedShaderPath(), filename);
}

template<class STR> inline
void 
ShaderCompileRequest::getShaderPermutationDirTo(STR& o, StrView impShaderDir, StrView permutName, const ProjectSetting& projSetting)
{
	o.clear();

	auto& ps = projSetting;
	fmtTo(o, "{}/{}/{}", impShaderDir, ps.shaderPermutationPath(), permutName);
}

template<class STR> inline
void
ShaderCompileRequest::getImportedBinDirTo(STR& o, StrView impShaderDir, RenderApiType apiType, const ShaderPermutations& permuts, const ProjectSetting& ps)
{
	o.clear();

	auto	formatPath			= RenderApiUtil::toShaderFormat(apiType);
	auto&	shaderImportedDir	= o;

	if (permuts.isEmpty())
	{
		fmtTo(shaderImportedDir, "{}/{}", impShaderDir, formatPath);
	}
	else
	{
		TempString permutName;
		permuts.appendNameTo(permutName);
		fmtTo(shaderImportedDir, "{}/{}/{}/{}", impShaderDir, ps.shaderPermutationPath(), permutName, formatPath);
	}
}

template<class STR> inline
void 
ShaderCompileRequest::getBinDirTo(STR& o, StrView impShaderDir, RenderApiType apiType)
{
	o.clear();

	auto formatPath	= RenderApiUtil::toShaderFormat(apiType);
	fmtTo(o, "{}/{}", impShaderDir, formatPath);
}

template<class STR> inline
void 
ShaderCompileRequest::getBinPassDirTo(STR& o, StrView binDir, SizeType iPass)
{
	o.clear();

	fmtTo(o, "{}/pass{}", binDir, iPass);
}

template<class STR> inline
void 
ShaderCompileRequest::getBinFilepathTo(STR& o, StrView binPassDir, ShaderStageFlag stage, RenderApiType apiType)
{
	using SRC = rds::RenderApiType;

	o.clear();

	const char* stageProfile = nullptr;
	switch (apiType)
	{
		//case SRC::OpenGL:	{} break;
		//case SRC::Dx11:		{} break;
		//case SRC::Metal:	{} break;
		case SRC::Vulkan:	{ stageProfile = RenderApiUtil::toVkShaderStageProfile(stage); }	break;
		case SRC::Dx12:		{ stageProfile = RenderApiUtil::toDx12ShaderStageProfile(stage); }	break;
		default: { RDS_THROW("getBinFilepathTo failed"); } break;
	}
	fmtTo(o, "{}/{}.bin", binPassDir, stageProfile);
}

template<class STR> inline
void 
ShaderCompileRequest::getShaderInfoFilepathTo(STR& o, StrView impShaderDir)
{
	o.clear();
	fmtTo(o, "{}/info.json", impShaderDir);
}

template<class STR> inline
void 
ShaderCompileRequest::getShaderStageInfoFilepathTo(STR& o, StrView binFilepath)
{
	o.clear();
	fmtTo(o, "{}.json", binFilepath);
}

template<class STR> inline
void 
ShaderCompileRequest::getAllStageUnionInfoFilepathTo(STR& o, StrView binPassDir, SizeType iPass)
{
	o.clear();

	auto& allStageUnionInfoPath = o;
	fmtTo(allStageUnionInfoPath, "{}/pass{}.json", binPassDir, iPass);
}

template<class STR> inline
void 
ShaderCompileRequest::getBinDependencyFilepathTo(STR& o, StrView binFilepath)
{
	o.clear();
	fmtTo(o, "{}.dep", binFilepath);
}

template<class STR> inline
void 
ShaderCompileRequest::getMakefilePathTo(STR& o, StrView impShaderDir)
{
	o.clear();
	fmtTo(o, "{}/makeFile", impShaderDir);
}



#endif

}