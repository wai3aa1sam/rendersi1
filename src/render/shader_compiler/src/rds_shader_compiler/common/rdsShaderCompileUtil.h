#pragma once

#include <rds_render.h>
#include "rds_shader_compiler-config.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompileUtil-Decl ---
#endif // 0
#if 1

struct ShaderGnuMake_VarName
{
	static constexpr const char* buildDX11			= "BUILD_DX11";
	static constexpr const char* buildDX12			= "BUILD_DX12";
	static constexpr const char* buildOpengl		= "BUILD_OPENGL";
	static constexpr const char* buildSpirv			= "BUILD_SPIRV";
	static constexpr const char* buildMetal			= "BUILD_METAL";

	static constexpr const char* dx11				= "DX11";
	static constexpr const char* dx12				= "DX12";
	static constexpr const char* opengl				= "OPENGL";
	static constexpr const char* spirv				= "SPIRV";
	static constexpr const char* metal				= "METAL";

	static constexpr const char* currentMakeFile	= "CURRENT_MAKEFILE";
	static constexpr const char* rdsRoot			= "RDS_ROOT";
	static constexpr const char* projectRoot		= "PROJECT_ROOT";
	static constexpr const char* shaderCompilerPath	= "RDS_SHADER_COMPILER_PATH";

	static constexpr const char* shaderCompiler		= "rdsShaderCompiler";
	static constexpr const char* glslc				= "glslc";

	static constexpr const char* shaderFilePath		= "SHADER_FILE_PATH";
	static constexpr const char* shaderFileRoot		= "SHADER_FILE_ROOT";
	static constexpr const char* shaderBinRoot		= "SHADER_BIN_ROOT";

	static constexpr const char* builtInPath		= "BUILT_IN_PATH";
	static constexpr const char* builtInRoot		= "BUILT_IN_ROOT";
	static constexpr const char* builtInShaderPath	= "BUILT_IN_SHADER_PATH";
	static constexpr const char* builtInShaderRoot	= "BUILT_IN_SHADER_ROOT";
};
extern ShaderGnuMake_VarName gnuMakeVarName;

#endif

#if 0
#pragma mark --- rdsShaderCompileUtil-Decl ---
#endif // 0
#if 1

struct ShaderCompileUtil
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using ApiType = RenderApiType;

public:
	static const char* getBuildTargetName(	ApiType type);
	static const char* getBuildBinName(		ApiType type);
	static const char* getCompilerName(		ApiType type);
	static const char* getStageProfile(		ShaderStageFlag stage, ApiType type);
	static const char* getGlslcStageProfile(ShaderStageFlag stage);
	static const char* getShaderFormat(		ApiType v);

	
	template<class STR> static void getDstDirTo(		STR& o, StrView filename, const ProjectSetting& ps);
	template<class STR> static void getBinDirTo(		STR& o, StrView dstDir, SizeType iPass, RenderApiType apiType);
	template<class STR> static void getBinFilepathTo(	STR& o, StrView binDir, ShaderStageFlag stageProfile, RenderApiType apiType);


public:
};

template<class STR> inline
void 
ShaderCompileUtil::getDstDirTo(STR& o, StrView filename, const ProjectSetting& ps)
{
	fmtTo(o, "{}/{}/{}", ps.projectRoot(), ps.importedShaderPath(), filename);
}

template<class STR> inline
void 
ShaderCompileUtil::getBinDirTo(STR& o, StrView dstDir, SizeType iPass, RenderApiType apiType)
{
	fmtTo(o, "{}/{}/pass{}", dstDir, getShaderFormat(apiType), iPass);
}

template<class STR> inline
void 
ShaderCompileUtil::getBinFilepathTo(STR& o, StrView binDir, ShaderStageFlag stage, RenderApiType apiType)
{
	using SRC = rds::RenderApiType;

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
	fmtTo(o, "{}/{}.bin", binDir, stageProfile);
}


#endif


}