#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/shader/rdsShader.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class Shader_Vk;
class ShaderPass_Vk;

#if 0
#pragma mark --- rdsShaderPass_Vk-Decl ---
#endif // 0
#if 1


struct Vk_VertexShaderStage : public VertexShaderStage
{
public:
	using Base = VertexShaderStage;
	using Util = Vk_RenderApiUtil;

public:
	void load(ShaderPass_Vk* pass, StrView passPath);

	VkPipelineShaderStageCreateInfo createVkStageInfo(const char* entry);

protected:
	Vk_ShaderModule _vkModule;
};

struct Vk_PixelShaderStage : public PixelShaderStage
{
public:
	using Base = PixelShaderStage;
	using Util = Vk_RenderApiUtil;

public:
	void load(ShaderPass_Vk* pass, StrView passPath);

	VkPipelineShaderStageCreateInfo createVkStageInfo(const char* entry);

protected:
	Vk_ShaderModule _vkModule;
};

class ShaderPass_Vk : public ShaderPass
{
public:
	using Base = ShaderPass;
	using Util = Vk_RenderApiUtil;

	using VertexStage	= Vk_VertexShaderStage;
	using PixelStage	= Vk_PixelShaderStage;

public:
	ShaderPass_Vk() {}
	virtual ~ShaderPass_Vk() {}

public:
	virtual void onCreate(Shader* shader, const Info* info, StrView passPath) override;

	template<size_t N> void createVkShaderStageCInfos(Vector<VkPipelineShaderStageCreateInfo, N>& outCInfos);

	Shader_Vk* shader();

	VertexStage* vkVertexStage();
	PixelStage*	 vkPixelStage();

protected:
	void onCreateRenderPass();

protected:
	VertexStage _vkVertexStage;
	PixelStage	_vkPixelStage;

	//Vk_DescriptorSetLayout	_testVkDescriptorSetLayout;
	Vk_PipelineLayout		_vkPipelineLayout;
};

template<size_t N> inline 
void 
ShaderPass_Vk::createVkShaderStageCInfos(Vector<VkPipelineShaderStageCreateInfo, N>& outCInfos)
{
	outCInfos.clear();
	outCInfos.reserve(enumInt(ShaderStageFlag::_kCount));
	if (!info().vsFunc.is_empty()) { outCInfos.emplace_back(_vkVertexStage.createVkStageInfo(info().vsFunc.c_str())); }
	if (!info().psFunc.is_empty()) { outCInfos.emplace_back( _vkPixelStage.createVkStageInfo(info().psFunc.c_str())); }
}

inline Shader_Vk* ShaderPass_Vk::shader() { return sCast<Shader_Vk*>(_shader); }


inline ShaderPass_Vk::VertexStage*	ShaderPass_Vk::vkVertexStage() { return &_vkVertexStage; }
inline ShaderPass_Vk::PixelStage*	ShaderPass_Vk::vkPixelStage	() { return &_vkPixelStage; }

#endif

#if 0
#pragma mark --- rdsShader_Vk-Decl ---
#endif // 0
#if 1

class Shader_Vk : public Shader
{
public:
	using Base = Shader;
	using Util = Vk_RenderApiUtil;

	using Pass = ShaderPass_Vk;

public:
	Shader_Vk();
	~Shader_Vk();

	virtual void onReset() override;

protected:
	virtual void onCreate		(const CreateDesc& cDesc);
	virtual void onPostCreate	(const CreateDesc& cDesc);
	virtual void onDestroy		();

	//virtual UPtr<ShaderPass> onMakePass(Shader* shader, const ShaderPass::Info* info) override;

protected:

};

#endif
}
#endif

/*

vkCreatePipelineLayout for shader variables info (descriptor set)
vkCreateGraphicsPipelines needs pipeline layout + renderPass, spirv and render state, vertex layout
if subpasses are merged, new renderPass + vkCreateGraphicsPipelines should be recreated

RenderPass need attachment desc and subpass
framebuffer need RenderPass and image view
 

*/