#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"
#include "rds_render_api_layer/shader/rdsShader.h"
#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"


#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class Shader_Vk;
class ShaderPass_Vk;

#if 0
#pragma mark --- rdsShaderPass_Vk-Decl ---
#endif // 0
#if 1

template<class SHADER_STAGE>
struct Vk_ShaderStage : public SHADER_STAGE
{
public:
	using Base = SHADER_STAGE;
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
		destroy(pass);	// actually should have a previous pass (renderDevice)

		TempString binFilepath;
		ShaderCompileRequest::getBinFilepathTo(binFilepath, passPath, stageFlag(), pass->shader()->apiType());

		createInfo(binFilepath);

		auto* rdDevVk = pass->renderDeviceVk();
		_vkModule.create(binFilepath, rdDevVk);

		RDS_VK_SET_DEBUG_NAME_FMT_IMPL(_vkModule, rdDevVk, "{}-{}", pass->shader()->shadername(), stageFlag());
	}

	void destroy(ShaderPass_Vk* pass)
	{
		_vkModule.destroy(pass->renderDeviceVk());
	}

	VkPipelineShaderStageCreateInfo createVkStageInfo(const char* entry)
	{
		RDS_CORE_ASSERT(_vkModule, "invalid vkModule");

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

struct Vk_TessellationControlShaderStage : public Vk_ShaderStage<TessellationControlShaderStage>
{
public:
	using Base = Vk_ShaderStage<TessellationControlShaderStage>;

public:

};

struct Vk_TessellationEvaluationShaderStage : public Vk_ShaderStage<TessellationEvaluationShaderStage>
{
public:
	using Base = Vk_ShaderStage<TessellationEvaluationShaderStage>;

public:

};

struct Vk_GeometryShaderStage : public Vk_ShaderStage<GeometryShaderStage>
{
public:
	using Base = Vk_ShaderStage<GeometryShaderStage>;

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

	using VertexStage					= Vk_VertexShaderStage;
	using TessellationControlStage		= Vk_TessellationControlShaderStage;
	using TessellationEvaluationStage	= Vk_TessellationEvaluationShaderStage;
	using GeometryStage					= Vk_GeometryShaderStage;
	using PixelStage					= Vk_PixelShaderStage;
	using ComputeStage					= Vk_ComputeShaderStage;

public:
	ShaderPass_Vk();
	virtual ~ShaderPass_Vk();
	
	template<size_t N>	void createVkShaderStageCInfos			(Vector<VkPipelineShaderStageCreateInfo, N>& outCInfos);
						void createComputeVkShaderStageCInfo	(VkPipelineShaderStageCreateInfo& out);

public:
	Shader_Vk*			shaderVk();
	RenderDevice_Vk*	renderDeviceVk();

	VertexStage*					vkVertexStage();
	TessellationControlStage*		vkTessellationControlStage();
	TessellationEvaluationStage*	vkTessellationEvaluationStage();
	GeometryStage*					vkGeometryStage();
	PixelStage*						vkPixelStage();
	ComputeStage*					vkComputeStage();

	Vk_DescriptorSetLayout&	vkDescriptorSetLayout();

protected:
	virtual void onCreate(Shader* shader, const Info* info, StrView passPath) override;
	virtual void onDestroy() override;


protected:
	VertexStage						_vkVertexStage;
	TessellationControlStage		_vkTescStage;
	TessellationEvaluationStage		_vkTeseStage;
	GeometryStage					_vkGeometryStage;
	PixelStage						_vkPixelStage;
	ComputeStage					_vkComputeStage;

	Vk_DescriptorSetLayout	_vkDescriptorSetLayout;
};

template<size_t N> inline 
void 
ShaderPass_Vk::createVkShaderStageCInfos(Vector<VkPipelineShaderStageCreateInfo, N>& outCInfos)
{
	auto& passInfo = info();
	outCInfos.clear();
	outCInfos.reserve(enumInt(ShaderStageFlag::_kCount));
	if (!passInfo.vsFunc.is_empty())	{ outCInfos.emplace_back(	_vkVertexStage.createVkStageInfo(passInfo.vsFunc.c_str())); }
	if (!passInfo.tescFunc.is_empty())	{ outCInfos.emplace_back(	  _vkTescStage.createVkStageInfo(passInfo.tescFunc.c_str())); }
	if (!passInfo.teseFunc.is_empty())	{ outCInfos.emplace_back(	  _vkTeseStage.createVkStageInfo(passInfo.teseFunc.c_str())); }
	if (!passInfo.geomFunc.is_empty())	{ outCInfos.emplace_back( _vkGeometryStage.createVkStageInfo(passInfo.geomFunc.c_str())); }
	if (!passInfo.psFunc.is_empty())	{ outCInfos.emplace_back(	 _vkPixelStage.createVkStageInfo(passInfo.psFunc.c_str())); }
}

inline Shader_Vk*		ShaderPass_Vk::shaderVk()		{ return sCast<Shader_Vk*>(_shader); }

inline ShaderPass_Vk::VertexStage*					ShaderPass_Vk::vkVertexStage	()				{ return &_vkVertexStage; }
inline ShaderPass_Vk::TessellationControlStage*		ShaderPass_Vk::vkTessellationControlStage()		{ return &_vkTescStage; }
inline ShaderPass_Vk::TessellationEvaluationStage*	ShaderPass_Vk::vkTessellationEvaluationStage()	{ return &_vkTeseStage; }
inline ShaderPass_Vk::GeometryStage*				ShaderPass_Vk::vkGeometryStage()				{ return &_vkGeometryStage; }
inline ShaderPass_Vk::PixelStage*					ShaderPass_Vk::vkPixelStage		()				{ return &_vkPixelStage; }
inline ShaderPass_Vk::ComputeStage*					ShaderPass_Vk::vkComputeStage	()				{ return &_vkComputeStage; }

inline Vk_DescriptorSetLayout&		ShaderPass_Vk::vkDescriptorSetLayout()	{ return _vkDescriptorSetLayout; }

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


protected:
	virtual void onCreate		(const CreateDesc& cDesc);
	virtual void onPostCreate	(const CreateDesc& cDesc);
	virtual void onDestroy		();

	virtual void onReset() override;

	virtual UPtr<ShaderPass> onMakePass(Shader* shader, const ShaderPass::Info& info) override;

protected:

};

inline
UPtr<ShaderPass> 
Shader_Vk::onMakePass(Shader* shader, const ShaderPass::Info& info)
{
	return makeUPtr<Pass>();
}


#endif

#if 0
#pragma mark --- rdsVk_PipelineLayoutCDesc-Decl ---
#endif // 0
#if 1

struct Vk_PipelineLayoutCDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kLocalSize = 8;

public:
	void create(Vk_PipelineLayout& out, const ShaderInfo::Pass& passInfo, Vk_DescriptorSetLayout* setLayout, RenderDevice_Vk* rdDevVk);

protected:
	void destroy();

private:
	Vector<Vk_DescriptorSetLayout_T*,	s_kLocalSize> _setLayoutHnds;
	Vector<VkPushConstantRange,			s_kLocalSize> _pushConstantRanges;
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