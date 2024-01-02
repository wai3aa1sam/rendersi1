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

	if (!info->csFunc.is_empty())
	{
		_computeStage	= &_vkComputeStage;
		_vkComputeStage.create(this, passPath);
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

	if (!_info->csFunc.is_empty())
	{
		_vkComputeStage.destroy(this);
		_computeStage = nullptr;
	}

	Base::onDestroy();
}

void 
ShaderPass_Vk::createComputeVkShaderStageCInfo(VkPipelineShaderStageCreateInfo& out)
{
	if (!info().csFunc.is_empty()) {out = _vkComputeStage.createVkStageInfo(info().csFunc.c_str()); }
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