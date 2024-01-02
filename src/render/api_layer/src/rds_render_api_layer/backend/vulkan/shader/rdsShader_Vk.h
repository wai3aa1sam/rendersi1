#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
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

template<class VK_SHADER_STAGE>
struct Vk_ShaderStage : public VK_SHADER_STAGE
{
public:
	using Base = VK_SHADER_STAGE;
	using Util = Vk_RenderApiUtil;

public:
	Vk_ShaderStage()
	{

	}

	~Vk_ShaderStage()
	{
		
	}

	void create(ShaderPass_Vk* pass, StrView passPath)
	{
		TempString binPath;
		fmtTo(binPath, "{}/{}.bin", passPath, Util::toShaderStageProfile(stageFlag()));

		auto* rdDevVk = pass->shader()->renderDeviceVk();
		_vkModule.create(binPath, rdDevVk);

		binPath += ".json";
		JsonUtil::readFile(binPath, _info);

		RDS_VK_SET_DEBUG_NAME_FMT_IMPL(_vkModule, rdDevVk, "{}-{}", pass->shader()->filename(), stageFlag());
	}

	void destroy(ShaderPass_Vk* pass)
	{
		_vkModule.destroy(pass->shader()->renderDeviceVk());
	}

	VkPipelineShaderStageCreateInfo createVkStageInfo(const char* entry)
	{
		VkPipelineShaderStageCreateInfo	stageInfo = {};
		stageInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage					= Util::toVkShaderStageBit(stageFlag());
		stageInfo.module				= _vkModule.hnd();
		stageInfo.pName					= entry;
		stageInfo.pSpecializationInfo	= nullptr;

		return stageInfo;
	}

	//RenderDevice_Vk* renderDeviceVk() { return pass->shader()->renderDeviceVk(); }

protected:
	//ShaderPass_Vk*	_passVk = nullptr;
	Vk_ShaderModule _vkModule;
};

struct Vk_VertexShaderStage : public Vk_ShaderStage<VertexShaderStage>
{
public:
	using Base = Vk_ShaderStage<VertexShaderStage>;

public:
	
};

struct Vk_PixelShaderStage : public Vk_ShaderStage<PixelShaderStage>
{
public:
	using Base = Vk_ShaderStage<PixelShaderStage>;

public:

};

struct Vk_ComputeShaderStage : public Vk_ShaderStage<ComputeShaderStage>
{
public:
	using Base = Vk_ShaderStage<ComputeShaderStage>;

public:

};

class ShaderPass_Vk : public ShaderPass
{
public:
	using Base = ShaderPass;
	using Util = Vk_RenderApiUtil;

	using VertexStage	= Vk_VertexShaderStage;
	using PixelStage	= Vk_PixelShaderStage;
	using ComputeStage	= Vk_ComputeShaderStage;

public:
	ShaderPass_Vk();
	virtual ~ShaderPass_Vk();

	Shader_Vk* shader();

	VertexStage*	vkVertexStage();
	PixelStage*		vkPixelStage();
	ComputeStage*	vkComputeStage();

protected:
	virtual void onCreate(Shader* shader, const Info* info, StrView passPath) override;
	virtual void onDestroy() override;

	template<size_t N>	void createVkShaderStageCInfos			(Vector<VkPipelineShaderStageCreateInfo, N>& outCInfos);
						void createComputeVkShaderStageCInfo	(VkPipelineShaderStageCreateInfo& out);

protected:
	VertexStage		_vkVertexStage;
	PixelStage		_vkPixelStage;
	ComputeStage	_vkComputeStage;
};

template<size_t N> inline 
void 
ShaderPass_Vk::createVkShaderStageCInfos(Vector<VkPipelineShaderStageCreateInfo, N>& outCInfos)
{
	outCInfos.clear();
	outCInfos.reserve(enumInt(ShaderStageFlag::_kCount));
	if (!info().vsFunc.is_empty()) { outCInfos.emplace_back(_vkVertexStage  .createVkStageInfo(info().vsFunc.c_str())); }
	if (!info().psFunc.is_empty()) { outCInfos.emplace_back( _vkPixelStage  .createVkStageInfo(info().psFunc.c_str())); }
}

inline Shader_Vk* ShaderPass_Vk::shader() { return sCast<Shader_Vk*>(_shader); }


inline ShaderPass_Vk::VertexStage*	ShaderPass_Vk::vkVertexStage	() { return &_vkVertexStage; }
inline ShaderPass_Vk::PixelStage*	ShaderPass_Vk::vkPixelStage		() { return &_vkPixelStage; }
inline ShaderPass_Vk::ComputeStage*	ShaderPass_Vk::vkComputeStage	() { return &_vkComputeStage; }

#endif

#if 0
#pragma mark --- rdsShader_Vk-Decl ---
#endif // 0
#if 1

class Shader_Vk : public RenderResource_Vk<Shader>
{
public:
	using Base = RenderResource_Vk<Shader>;
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