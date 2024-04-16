#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompileUtil.h"

#include <rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h>

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompileUtil-Impl ---
#endif // 0
#if 1
 
const char* 
ShaderCompileUtil::getBuildTargetName(ApiType type)
{
	using SRC = ApiType;
	switch (type)
	{
		case SRC::Dx11:		{ return gnuMakeVarName.buildDX11; }
		case SRC::Dx12:		{ return gnuMakeVarName.buildDX12; }
		case SRC::OpenGL:	{ return gnuMakeVarName.buildOpengl; }
		case SRC::Vulkan:	{ return gnuMakeVarName.buildSpirv; }
		case SRC::Metal:	{ return gnuMakeVarName.buildMetal; }
		default: { RDS_THROW("unknow api type"); }
	}
}

const char* 
ShaderCompileUtil::getBuildBinName(ApiType type)
{
	using SRC = ApiType;
	switch (type)
	{
		case SRC::Dx11:		{ return gnuMakeVarName.dx11; }
		case SRC::Dx12:		{ return gnuMakeVarName.dx12; }
		case SRC::OpenGL:	{ return gnuMakeVarName.opengl; }
		case SRC::Vulkan:	{ return gnuMakeVarName.spirv; }
		case SRC::Metal:	{ return gnuMakeVarName.metal; }
		default: { RDS_THROW("unknow api type"); }
	}
}

const char* 
ShaderCompileUtil::getCompilerName(ApiType type)
{
	using SRC = ApiType;
	switch (type)
	{
		case SRC::Dx11:		{ return gnuMakeVarName.shaderCompiler; }
					  //case SRC::Dx12:		{ return gnuMakeVarName.dx12; }
					  //case SRC::OpenGL:	{ return gnuMakeVarName.opengl; }
		case SRC::Vulkan:	{ return gnuMakeVarName.shaderCompiler; }
						//case SRC::Metal:	{ return gnuMakeVarName.metal; }
		default: { RDS_THROW("unknow api type"); }
	}
}

const char* 
ShaderCompileUtil::getStageProfile(ShaderStageFlag stage, ApiType type)
{
	using SRC = ApiType;
	switch (type)
	{
		//case SRC::Dx11:		{ return DX11Util::getDxStageProfile(stage); }
		//case SRC::Dx12:		{ return gnuMakeVarName.dx12; }
		//case SRC::OpenGL:	{ return gnuMakeVarName.opengl; }
		case SRC::Vulkan:	{ return Vk_RenderApiUtil::toVkShaderStageProfile(stage); }
						//case SRC::Metal:	{ return gnuMakeVarName.metal; }
		default: { RDS_THROW("unknow api type, getBuildBinName()"); }
	}
}
 
const char* 
ShaderCompileUtil::getGlslcStageProfile(ShaderStageFlag stage)
{
	using SRC = ShaderStageFlag;
	switch (stage)
	{
		case SRC::Vertex:	{ return "vertex"; }
		case SRC::Pixel:	{ return "fragment"; }
		default: { RDS_THROW("unknow ShaderStageFlag, getGlslcStageProfile()"); }
	}
}
 
StrView
ShaderCompileUtil::getBuildApiPath(ApiType type)
{
	auto& ps = *ProjectSetting::instance();

	using SRC = ApiType;
	switch (type)
	{
		//case SRC::Dx11:		{ return "dx11"; }
					  //case SRC::Dx12:		{ return ""; }
					  //case SRC::OpenGL:	{ return ""; }
		case SRC::Vulkan:	{ return ps.spirvPath(); }
						//case SRC::Metal:	{ return ""; }
		default: { RDS_THROW("unknow api type"); }
	}
}



#endif


}