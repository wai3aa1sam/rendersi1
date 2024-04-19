#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "rdsShaderCompiler.h"

#include <spirv_cross.hpp>
#include <spirv_hlsl.hpp>

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"

//#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompiler_Vk-Decl ---
#endif // 0
#if 1

class ShaderCompiler_Vk : public ShaderCompiler
{
public:
	using Base = ShaderCompiler;

	using ShaderResources	= ::spirv_cross::ShaderResources;
	using SpirvCompiler		= ::spirv_cross::CompilerHLSL;

	using Util = Vk_RenderApiUtil;

public:
	/* .hlsl -> .spv */ 

protected:
	virtual void onCompile(const CompileDescView& descView) override;

protected:
	// reflect .spv by spirv_cross
	void reflect(StrView outpath, ByteSpan spvBytes, ShaderStageFlag stage);
	void _reflect_inputs		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_outputs		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_pushConstants	(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_constBufs		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_textures		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_samplers		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_storageBufs	(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_storageImages	(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);

	void _reflect_threadGroups	(ShaderStageInfo& outInfo, SpirvCompiler& compiler);
	void _reflect_struct		(ShaderStageInfo& outInfo, ShaderStageInfo::Variables* outVariables, SpirvCompiler& compiler, const spirv_cross::SPIRType& type, bool isRowMajor);
};



#endif

}

//#endif