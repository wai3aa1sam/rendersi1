#include "rds_render_api_layer-pch.h"
#include "rdsShader_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

SPtr<Shader> 
RenderDevice_Vk::onCreateShader(const Shader_CreateDesc& cDesc)
{
	auto p = SPtr<Shader>(makeSPtr<Shader_Vk>());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsVk_ShaderStage-Impl ---
#endif // 0
#if 1

//VkPipelineShaderStageCreateInfo	
//createVkStageInfo_Helper(ShaderStageFlag flag, const char* entry, Vk_ShaderModule& module)
//{
//	using Util = Vk_RenderApiUtil;
//
//	VkPipelineShaderStageCreateInfo	stageInfo = {};
//	stageInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	stageInfo.stage					= Util::toVkShaderStageBit(flag);
//	stageInfo.module				= module.hnd();
//	stageInfo.pName					= entry;
//	stageInfo.pSpecializationInfo	= nullptr;
//
//	return stageInfo;
//}
//
//void 
//loadShaderStage_Helper(ShaderPass_Vk* pass, StrView passPath, ShaderStageFlag stage, ShaderStageInfo& info, Vk_ShaderModule& module)
//{
//	using Util = Vk_RenderApiUtil;
//
//	TempString binPath;
//	fmtTo(binPath, "{}/{}.bin", passPath, Util::toShaderStageProfile(stage));
//	
//	auto* rdDevVk = pass->shader()->renderDeviceVk();
//	module.create(binPath, rdDevVk);
//
//	binPath += ".json";
//	JsonUtil::readFile(binPath, info);
//}

//void 
//Vk_VertexShaderStage::load(ShaderPass_Vk* pass, StrView passPath)
//{
//	loadShaderStage_Helper(pass, passPath, stageFlag(), _info, _vkModule);
//}
//
//VkPipelineShaderStageCreateInfo 
//Vk_VertexShaderStage::createVkStageInfo(const char* entry)
//{
//	return createVkStageInfo_Helper(stageFlag(), entry, _vkModule);
//}
//
//void 
//Vk_PixelShaderStage::load(ShaderPass_Vk* pass, StrView passPath)
//{
//	loadShaderStage_Helper(pass, passPath, stageFlag(), _info, _vkModule);
//}
//
//VkPipelineShaderStageCreateInfo 
//Vk_PixelShaderStage::createVkStageInfo(const char* entry)
//{
//	return createVkStageInfo_Helper(stageFlag(), entry, _vkModule);
//}

#endif

#if 0
#pragma mark --- rdsShaderPass_Vk-Impl ---
#endif // 0
#if 1

ShaderPass_Vk::ShaderPass_Vk()
{

}

ShaderPass_Vk::~ShaderPass_Vk()
{
	destroy();
}

void 
ShaderPass_Vk::onCreate(Shader* shader, const Info* info, StrView passPath)
{
	Base::onCreate(shader, info, passPath);

	_shader			= shader;
	_info			= info;

	if (!info->vsFunc.is_empty())
	{
		_vertexStage	= &_vkVertexStage;
		_vkVertexStage.create(this, passPath);
	}

	if (!info->psFunc.is_empty())
	{
		_pixelStage		= &_vkPixelStage;
		_vkPixelStage.create(this, passPath);
	}
}

void 
ShaderPass_Vk::onDestroy()
{
	if (!_info->vsFunc.is_empty())
	{
		_vkVertexStage.destroy(this);
		_vertexStage = nullptr;
	}

	if (!_info->psFunc.is_empty())
	{
		_vkPixelStage.destroy(this);
		_pixelStage = nullptr;
	}

	Base::onDestroy();
}

void 
ShaderPass_Vk::onCreateRenderPass()
{

}

#endif

#if 0
#pragma mark --- rdsShader_Vk-Impl ---
#endif // 0
#if 1

Shader_Vk::Shader_Vk()
{
}

Shader_Vk::~Shader_Vk()
{
	destroy();
}

void 
Shader_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
	
	onReset();
}

void 
Shader_Vk::onPostCreate(const CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void 
Shader_Vk::onDestroy()
{
	Base::onDestroy();
}

void 
Shader_Vk::onReset()
{
	Base::onReset();

	_passes.clear();
	SizeType passCount = _info.passes.size();
	_passes.reserve(passCount);

	using PassInfo = ShaderPassInfo;
	for (size_t i = 0; i < passCount; i++)
	{
		const PassInfo& pass = _info.passes[i];

		TempString passPath;
		fmtTo(passPath, "{}/{}/{}/pass{}", Traits::s_defaultShaderOutPath, filename(), Traits::s_spirvPath, i);

		auto up = makeUPtr<Pass>();
		up->create(this, &pass, passPath);
		_passes.emplace_back(rds::move(up));
	}
}

#endif

}
#endif