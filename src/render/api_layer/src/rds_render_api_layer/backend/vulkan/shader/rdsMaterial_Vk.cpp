#include "rds_render_api_layer-pch.h"
#include "rdsMaterial_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderContext_Vk.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandBuffer.h"

#include "rds_render_api_layer/backend/vulkan/pass/rdsVk_RenderPassPool.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

SPtr<Material> 
RenderDevice_Vk::onCreateMaterial(const	Material_CreateDesc& cDesc)
{
	auto p = SPtr<Material>(makeSPtr<Material_Vk>());
	p->create(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsVk_PipelineCDesc-Impl ---
#endif // 0
#if 1

struct Vk_ShaderStagesCDesc : public Vk_CDesc_Base
{
	using Vk_ShaderStageCInfos = Vector<VkPipelineShaderStageCreateInfo, enumInt(ShaderStageFlag::_kCount)>;

public:
	static constexpr SizeType s_kLocalSize = 8;

public:
	void createGraphics(VkGraphicsPipelineCreateInfo& out, ShaderPass_Vk* shaderPass)
	{
		destroy();
		_shaderStageCInfos.reserve(enumInt(ShaderStageFlag::_kCount));

		const auto& info = shaderPass->info();

		if (!info.vsFunc.is_empty()) { _shaderStageCInfos.emplace_back(VkPipelineShaderStageCreateInfo{shaderPass->vkVertexStage()->createVkStageInfo(info.vsFunc.c_str())}); }
		if (!info.psFunc.is_empty()) { _shaderStageCInfos.emplace_back(VkPipelineShaderStageCreateInfo{shaderPass->vkPixelStage ()->createVkStageInfo(info.psFunc.c_str())}); }

		out.stageCount	= sCast<u32>(_shaderStageCInfos.size());
		out.pStages		= _shaderStageCInfos.data();
	}

	void createCompute(VkComputePipelineCreateInfo& out, ShaderPass_Vk* shaderPass)
	{
		destroy();

		const auto& info = shaderPass->info();

		throwIf(info.csFunc.is_empty(), "cs don't have an entry func");
		if (!info.csFunc.is_empty()) { out.stage = VkPipelineShaderStageCreateInfo{shaderPass->vkComputeStage()->createVkStageInfo(info.csFunc.c_str())}; }
	}

protected:
	void destroy()
	{
		_shaderStageCInfos.clear();
	}

private:
	Vk_ShaderStageCInfos _shaderStageCInfos;
};

struct Vk_GraphicsPipelineCDesc : public Vk_CDesc_Base
{
	using Vk_PipelineColorBlendAttachmentStates = Vector<VkPipelineColorBlendAttachmentState, RDS_VK_MAX_RENDER_TARGET_COUNT>;
public:
	Vk_GraphicsPipelineCDesc()
	{
		
	}

	void create(VkGraphicsPipelineCreateInfo& out, const RenderState& rdState, const ShaderStageInfo& psStageInfo)
	{
		_createViewportState				(viewportState,			rdState);
		_createInputAssemblyState			(inputAssembly,			rdState);
		_createRasterizerState				(rasterizer,			rdState);
		_createMultiSampleState				(multisampling,			rdState);
		_createDepthStencilState			(depthStencil,			rdState);
		_createColorBlendAttachmentStates	(colorBlendAttachments,	rdState, psStageInfo);
		_createColorBlendState				(colorBlending,			rdState, colorBlendAttachments);
		_createDynamicState					(dynamicState);

		_create(out);
	}

	void createDefault(VkGraphicsPipelineCreateInfo& out)
	{
		_createDynamicState(dynamicState);
		// viewport stage
		//VkPipelineViewportStateCreateInfo viewportState = {};
		{
			VkViewport viewport = {};
			viewport.x			= 0.0f;
			viewport.y			= 0.0f;
			viewport.width		= Traits::s_kDefaultWindowWidth;
			viewport.height		= Traits::s_kDefaultWindowHeight;
			viewport.minDepth	= 0.0f;
			viewport.maxDepth	= 1.0f;

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = VkExtent2D{ sCast<u32>(viewport.width), sCast<u32>(viewport.height) };

			viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount	= 1;
			viewportState.scissorCount	= 1;
			viewportState.pViewports	= &viewport;
			viewportState.pScissors		= &scissor;
		}

		// input assembly
		//VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		{
			inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable	= VK_FALSE;
		}

		// rasterizer
		//VkPipelineRasterizationStateCreateInfo rasterizer = {};
		{
			rasterizer.sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable			= VK_FALSE; // true when no want to discard out bound pixel, useful for shawdow map
			rasterizer.rasterizerDiscardEnable	= VK_FALSE;	// true then no gemotry pass through the rasterizer stage
			rasterizer.polygonMode				= VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth				= 1.0f;		// check gpu features
			rasterizer.cullMode					= VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE;	// VK_FRONT_FACE_CLOCKWISE VK_FRONT_FACE_COUNTER_CLOCKWISE
			rasterizer.depthBiasEnable			= VK_FALSE;
			rasterizer.depthBiasConstantFactor	= 0.0f; // Optional
			rasterizer.depthBiasClamp			= 0.0f; // Optional
			rasterizer.depthBiasSlopeFactor		= 0.0f; // Optional
		}

		// Multisampling
		//VkPipelineMultisampleStateCreateInfo multisampling = {};
		{
			multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable	= VK_FALSE;
			multisampling.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading		= 1.0f;		// Optional
			multisampling.pSampleMask			= nullptr;	// Optional
			multisampling.alphaToCoverageEnable	= VK_FALSE; // Optional
			multisampling.alphaToOneEnable		= VK_FALSE; // Optional
		}

		// depth and stencil
		//VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
		{
			depthStencil.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable		= VK_TRUE;
			depthStencil.depthWriteEnable		= VK_TRUE;
			depthStencil.depthCompareOp			= VK_COMPARE_OP_LESS;
			depthStencil.depthBoundsTestEnable	= VK_FALSE;
			depthStencil.minDepthBounds			= 0.0f; // Optional
			depthStencil.maxDepthBounds			= 1.0f; // Optional
			depthStencil.stencilTestEnable		= VK_FALSE;
			depthStencil.front					= {}; // Optional
			depthStencil.back					= {}; // Optional
		}

		// color blending
		//VkPipelineColorBlendAttachmentState colorBlendAttachment = {};	// for per attached framebuffer
		//VkPipelineColorBlendStateCreateInfo colorBlending = {};			// is global color blending setting (for all framebuffer)
		{
			auto& colorBlendAttachment = colorBlendAttachments.emplace_back();
			colorBlendAttachment.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable			= VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;				// Optional
			colorBlendAttachment.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;		// Optional
			colorBlendAttachment.colorBlendOp			= VK_BLEND_OP_ADD;							// Optional
			colorBlendAttachment.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE;						// Optional
			colorBlendAttachment.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO;					// Optional
			colorBlendAttachment.alphaBlendOp			= VK_BLEND_OP_ADD;							// Optional

			colorBlending.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable		= VK_FALSE;
			colorBlending.logicOp			= VK_LOGIC_OP_COPY; // Optional
			colorBlending.attachmentCount	= 1;
			colorBlending.pAttachments		= &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f; // Optional
			colorBlending.blendConstants[1] = 0.0f; // Optional
			colorBlending.blendConstants[2] = 0.0f; // Optional
			colorBlending.blendConstants[3] = 0.0f; // Optional
		}

		_create(out);
	}

protected:
	void destroy()
	{
		viewportState			= {};
		inputAssembly			= {};
		rasterizer				= {};
		multisampling			= {};
		depthStencil			= {};
		colorBlending			= {};
		dynamicState			= {};

		colorBlendAttachments.clear();
	}

	void _create(VkGraphicsPipelineCreateInfo& out)
	{
		out.pInputAssemblyState	= &inputAssembly;
		out.pViewportState		= &viewportState;
		out.pRasterizationState	= &rasterizer;
		out.pMultisampleState	= &multisampling;
		out.pDepthStencilState	= &depthStencil;			// Optional
		out.pColorBlendState	= &colorBlending;
		out.pDynamicState		= &dynamicState;
	}

	void _createViewportState(VkPipelineViewportStateCreateInfo& out, const RenderState& rdState)
	{
		_viewport	= {};
		_scissor	= {};

		_viewport.x			= 0.0f;
		_viewport.y			= 0.0f;
		_viewport.width		= Traits::s_kDefaultWindowWidth;
		_viewport.height	= Traits::s_kDefaultWindowHeight;
		_viewport.minDepth	= 0.0f;
		_viewport.maxDepth	= 1.0f;

		_scissor.offset = { 0, 0 };
		_scissor.extent = VkExtent2D{ sCast<u32>(_viewport.width), sCast<u32>(_viewport.height) };

		out.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		out.viewportCount	= 1;
		out.scissorCount	= 1;
		out.pViewports		= &_viewport;
		out.pScissors		= &_scissor;
	}

	void _createInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo& out, const RenderState& rdState)
	{
		out = {};

		out.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		out.topology				= Util::toVkPrimitiveTopology(rdState.primitiveType);
		out.primitiveRestartEnable	= VK_FALSE;
	}

	void _createRasterizerState(VkPipelineRasterizationStateCreateInfo& out, const RenderState& rdState)
	{
		out = {};

		out.sType						= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		out.depthClampEnable			= VK_FALSE; // true when no want to discard out bound pixel, useful for shawdow map
		out.rasterizerDiscardEnable		= VK_FALSE;	// true then no gemotry pass through the rasterizer stage
		out.polygonMode					= rdState.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
		out.lineWidth					= 1.0f;		// check gpu features
		out.cullMode					= Util::toVkCullMode(rdState.cull);
		out.frontFace					= VK_FRONT_FACE_CLOCKWISE;	// VK_FRONT_FACE_CLOCKWISE VK_FRONT_FACE_COUNTER_CLOCKWISE
		out.depthBiasEnable				= VK_FALSE;
		out.depthBiasConstantFactor		= 0.0f; // Optional
		out.depthBiasClamp				= 0.0f; // Optional
		out.depthBiasSlopeFactor		= 0.0f; // Optional
	}

	void _createMultiSampleState(VkPipelineMultisampleStateCreateInfo& out, const RenderState& rdState)
	{
		out = {};

		multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable	= VK_FALSE;
		multisampling.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading		= 1.0f;		// Optional
		multisampling.pSampleMask			= nullptr;	// Optional
		multisampling.alphaToCoverageEnable	= VK_FALSE; // Optional
		multisampling.alphaToOneEnable		= VK_FALSE; // Optional
	}

	void _createDepthStencilState(VkPipelineDepthStencilStateCreateInfo& out, const RenderState& rdState)
	{
		out = {};

		out.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		out.depthTestEnable			= rdState.depthTest.isEnable() ? VK_TRUE : VK_FALSE;
		out.depthWriteEnable		= rdState.depthTest.writeMask  ? VK_TRUE : VK_FALSE;
		out.depthCompareOp			= Util::toVkCompareOp(rdState.depthTest.op);
		out.depthBoundsTestEnable	= VK_FALSE;
		out.minDepthBounds			= 0.0f; // Optional
		out.maxDepthBounds			= 1.0f; // Optional
		out.stencilTestEnable		= VK_FALSE;
		out.front					= {}; // Optional
		out.back					= {}; // Optional
	}

	void _createColorBlendAttachmentStates(Vk_PipelineColorBlendAttachmentStates& out, const RenderState& rdState, const ShaderStageInfo& psStageInfo)
	{
		auto n = psStageInfo.renderTargetCount();

		out.clear();
		out.reserve(n);

		for (size_t i = 0; i < n; i++)
		{
			auto& blendState = out.emplace_back();
			_createColorBlendAttachmentState(blendState, rdState);
		}
	}

	void _createColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& out, const RenderState& rdState)
	{
		out = {};
		out.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		out.blendEnable				= rdState.blend.isEnable() ? VK_TRUE : VK_FALSE;

		out.srcColorBlendFactor		= Util::toVkBlendFactor	(rdState.blend.rgb.srcFactor);
		out.dstColorBlendFactor		= Util::toVkBlendFactor	(rdState.blend.rgb.dstFactor);
		out.colorBlendOp			= Util::toVkBlendOp		(rdState.blend.rgb.op);			
		out.srcAlphaBlendFactor		= Util::toVkBlendFactor	(rdState.blend.alpha.srcFactor);
		out.dstAlphaBlendFactor		= Util::toVkBlendFactor	(rdState.blend.alpha.dstFactor);
		out.alphaBlendOp			= Util::toVkBlendOp		(rdState.blend.alpha.op);			
	}

	void _createColorBlendState(VkPipelineColorBlendStateCreateInfo& out, const RenderState& rdState, Span<VkPipelineColorBlendAttachmentState> colorBlendAttachments_)
	{
		// colorBlendAttachments have life cycle problem

		out = {};
		out.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		out.logicOpEnable		= VK_FALSE;
		out.logicOp				= VK_LOGIC_OP_COPY; // Optional
		/*out.attachmentCount		= sCast<u32>(colorBlendAttachments.size());
		out.pAttachments		= colorBlendAttachments.data();*/
		out.blendConstants[0]	= 0.0f; // Optional
		out.blendConstants[1]	= 0.0f; // Optional
		out.blendConstants[2]	= 0.0f; // Optional
		out.blendConstants[3]	= 0.0f; // Optional

		out.attachmentCount		= sCast<u32>(colorBlendAttachments_.size());
		out.pAttachments		= colorBlendAttachments_.data();
	}

	void _createDynamicState(VkPipelineDynamicStateCreateInfo& out)
	{
		_dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		_dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);

		dynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount	= sCast<u32>(_dynamicStates.size());
		dynamicState.pDynamicStates		= _dynamicStates.data();
	}

protected:
	VkPipelineViewportStateCreateInfo		viewportState			= {};
	VkPipelineInputAssemblyStateCreateInfo	inputAssembly			= {};
	VkPipelineRasterizationStateCreateInfo	rasterizer				= {};
	VkPipelineMultisampleStateCreateInfo	multisampling			= {};
	VkPipelineDepthStencilStateCreateInfo	depthStencil			= {};
	Vk_PipelineColorBlendAttachmentStates	colorBlendAttachments;
	VkPipelineColorBlendStateCreateInfo		colorBlending			= {};
	VkPipelineDynamicStateCreateInfo		dynamicState			= {};

private:
	Vector<VkDynamicState, 8>	_dynamicStates;
	VkRect2D					_scissor	= {};
	VkViewport					_viewport	= {};

};

struct Vk_VertexInputCDesc : public Vk_CDesc_Base
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kVtxInputAttrLocalSize = 32;

public:
	using Attrs		= Vk_MaterialPass_VertexStage::Vk_VertexInputAttrs;
	using Binding	= VkVertexInputBindingDescription;

public:
	void create(VkGraphicsPipelineCreateInfo& out, Vk_MaterialPass_VertexStage* vtxStage, const VertexLayout* vtxLayout)
	{
		destroy();

		const auto& vkInputAttrs = vtxStage->cacheVtxInputAttrCDesc(vtxLayout);
		u32 bindingPt = 0;

		binding.binding		= bindingPt;
		binding.stride		= sCast<u32>(vtxLayout->stride());
		binding.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;

		cInfo.sType								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		cInfo.vertexBindingDescriptionCount		= 1;
		cInfo.pVertexBindingDescriptions		= &binding;		// Optional
		cInfo.vertexAttributeDescriptionCount	= sCast<u32>(vkInputAttrs.size());
		cInfo.pVertexAttributeDescriptions		= vkInputAttrs.data();	// Optional

		out.pVertexInputState = &cInfo;
	}
protected:
	void destroy()
	{
		binding = {};
		cInfo	= {};
	}

protected:
	Binding binding;
	VkPipelineVertexInputStateCreateInfo cInfo;
};

#endif

#if 0
#pragma mark --- rdsVk_MaterialPass_Stage-Impl ---
#endif // 0
#if 1


Vk_MaterialPass_VertexStage::Vk_MaterialPass_VertexStage()
{

}

Vk_MaterialPass_VertexStage::~Vk_MaterialPass_VertexStage()
{
}

void
Vk_MaterialPass_VertexStage::create(MaterialPass_Vk* pass, ShaderStage* shaderStage)
{
	RDS_WARN_ONCE("move vk set to material pass, union reflection info");
	Base::create(pass, shaderStage);
}

void
Vk_MaterialPass_VertexStage::destroy(MaterialPass_Vk* pass)
{
}

void 
Vk_MaterialPass_PixelStage::bind(RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, const VertexLayout* vtxLayout, MaterialPass_Vk* pass)
{
	//Helper::bind(this, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, pass->vkPipelineLayout().hnd(), ctx, vkCmdBuf, vtxLayout, pass);
}

const Vk_MaterialPass_VertexStage::Vk_VertexInputAttrs& 
Vk_MaterialPass_VertexStage::cacheVtxInputAttrCDesc(const VertexLayout* vtxLayout)
{
	auto& inputMaps = _vtxInputAttrsMap;

	auto vtxInputIter = inputMaps.find(vtxLayout);
	if (vtxInputIter == inputMaps.end())
	{
		Vk_VertexInputAttrs& attrs = inputMaps[vtxLayout];

		u32 bindingPt = 0;

		for (size_t i = 0; i < info().inputs.size(); i++)
		{
			const auto& input = info().inputs[i];

			const VertexLayout::Element* e = vtxLayout->find(input.semantic);
			throwIf(!e, "vertex semantic not found {}", input.semantic);

			VkVertexInputAttributeDescription& attr = attrs.emplace_back();
			attr.binding	= bindingPt;
			attr.location	= sCast<u32>(i);
			attr.format		= Util::toVkFormat(e->dataType);
			attr.offset		= e->offset;
		}

		VkVertexInputBindingDescription bindingDesc = {};
		bindingDesc.binding		= bindingPt;
		bindingDesc.stride		= sCast<u32>(vtxLayout->stride());
		bindingDesc.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;

		return attrs;
	}

	return vtxInputIter->second;
}

void 
Vk_MaterialPass_PixelStage::create(MaterialPass_Vk* pass, ShaderStage* shaderStage)
{
	Base::create(pass, shaderStage);
}

void 
Vk_MaterialPass_PixelStage::destroy(MaterialPass_Vk* pass)
{
}

void 
Vk_MaterialPass_VertexStage::bind(RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, const VertexLayout* vtxLayout, MaterialPass_Vk* pass)
{
	//Helper::bind(this, VK_PIPELINE_BIND_POINT_GRAPHICS, 1, pass->vkPipelineLayout().hnd(), ctx, vkCmdBuf, vtxLayout, pass);
}

void 
Vk_MaterialPass_ComputeStage::create	(MaterialPass_Vk* pass, ShaderStage* shaderStage)
{
	Base::create(pass, shaderStage);
	using Helper = MaterialStage_Helper;
	//Helper::createVkDescriptorSet(this, pass->renderDeviceVk());
}

void 
Vk_MaterialPass_ComputeStage::destroy(MaterialPass_Vk* pass)
{
}

void 
Vk_MaterialPass_ComputeStage::bind(RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, MaterialPass_Vk* pass)
{
	//Helper::bind(this, VK_PIPELINE_BIND_POINT_COMPUTE, 0, pass->computeVkPipelineLayout().hnd(), ctx, vkCmdBuf, nullptr, pass);
}

#endif

#if 0
#pragma mark --- rdsMaterialPass_Vk-Impl ---
#endif // 0
#if 1

MaterialPass_Vk::MaterialPass_Vk()
{

}

MaterialPass_Vk::~MaterialPass_Vk()
{
	destroy();
}

void 
MaterialPass_Vk::onCreate(Material* material, ShaderPass* shaderPass)
{
	Base::onCreate(material, shaderPass);
	
	if (shaderPass->vertexStage())
	{
		_vkVertexStage.create(this, shaderPass->vertexStage());
		_vertexStage = &_vkVertexStage;
	}

	if (shaderPass->pixelStage())
	{
		_vkPixelStage.create(this, shaderPass->pixelStage());
		_pixelStage	= &_vkPixelStage;
	}

	if (shaderPass->computeStage())
	{
		_vkComputeStage.create(this, shaderPass->computeStage());
		_computeStage = &_vkComputeStage;
		createComputeVkPipeline(_computeVkPipeline);
	}

	_vkFramedDescrSets.resize(s_kFrameInFlightCount);
}

void 
MaterialPass_Vk::onDestroy()
{
	auto* rdDevVk = renderDeviceVk();

	if (_vertexStage)
	{
		_vkVertexStage.destroy(this);
		_vertexStage = nullptr;
	}

	if (_pixelStage)
	{
		_vkPixelStage.destroy(this);
		_pixelStage	= nullptr;
	}

	if (_computeStage)
	{
		_vkComputeStage.destroy(this);
		_computeStage	= nullptr;

		_computeVkPipeline.destroy(rdDevVk);
	}

	for (auto& e : _vkPipelineMap)
	{
		e.second.destroy(rdDevVk);
	}
	_vkPipelineMap.clear();
	_vkPipelineLayout.destroy(rdDevVk);

	_vkFramedDescrSets.clear();

	Base::onDestroy();
}

void 
MaterialPass_Vk::onBind(RenderContext* ctx, const VertexLayout* vtxLayout, Vk_CommandBuffer* vkCmdBuf)
{
	//auto* rdDevVk	= renderDeviceVk();
	//auto* vkCtx		= sCast<RenderContext_Vk*>(ctx);
	//auto* vkCmdBuf	= vkCtx->graphicsVkCmdBuf();
	auto* vkRdPass	= vkCmdBuf->getVkRenderPass();

	throwIf(!vkRdPass, "no render pass");

	VkPipelineBindPoint vkBindPt = VK_PIPELINE_BIND_POINT_GRAPHICS;
	bindPipeline(vkCmdBuf, vkRdPass, vtxLayout);

	bindDescriptorSet(vkBindPt, ctx, vkCmdBuf);
}

void 
MaterialPass_Vk::onBind(RenderContext* ctx, Vk_CommandBuffer* vkCmdBuf)
{
	VkPipelineBindPoint vkBindPt = VK_PIPELINE_BIND_POINT_COMPUTE;
	vkCmdBindPipeline(vkCmdBuf->hnd(), vkBindPt, _computeVkPipeline.hnd());
	bindDescriptorSet(vkBindPt, ctx, vkCmdBuf);
}

void 
MaterialPass_Vk::bindPipeline(Vk_CommandBuffer* vkCmdBuf, Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout)
{
	RDS_CORE_ASSERT(vkCmdBuf, "");
	VkPipelineBindPoint vkBindPt	= VK_PIPELINE_BIND_POINT_GRAPHICS;
	Vk_Pipeline*		vkPipeline	= nullptr;

	{
		auto it = _vkPipelineMap.find(vkRdPass);
		if (it != _vkPipelineMap.end())
		{
			vkPipeline = &it->second;
		}
		else
		{
			auto& outVkPipeline = _vkPipelineMap[vkRdPass];
			createVkPipeline(outVkPipeline, vkRdPass, vtxLayout);
			vkPipeline = &outVkPipeline;
			RDS_VK_SET_DEBUG_NAME_FMT(*vkPipeline, "{}-{}", shader()->filename(), "vkPipeline");
		}
	}

	vkCmdBindPipeline(vkCmdBuf->hnd(), vkBindPt, vkPipeline->hnd());
}

void 
MaterialPass_Vk::bindDescriptorSet(VkPipelineBindPoint vkBindPt, RenderContext* ctx, Vk_CommandBuffer* vkCmdBuf)
{
	auto* rdDevVk	= renderDeviceVk();
	auto* vkCtx		= sCast<RenderContext_Vk*>(ctx);

	{
		//auto* mtl		= pass->material();
		auto& shaderRsc = shaderResources();

		auto& vkDescrSet = vkDescriptorSet();
		if (!vkDescrSet)
		{
			auto&	descriptorAlloc	= vkCtx->vkRdFrame().descriptorAllocator();
			auto	builder			= Vk_DescriptorBuilder::make(&descriptorAlloc);
			builder.buildBindless(vkDescriptorSet(), shaderPass()->vkDescriptorSetLayout(), shaderRsc, shaderPass());
		}

		shaderRsc.uploadToGpu(shaderPass());		// this will reset dirty

		auto				vkPipelineLayoutHnd = vkPipelineLayout().hnd();
		auto				set					= sCast<u32>(rdDevVk->bindlessResourceVk().bindlessTypeCount());
		//renderDeviceVk()->bindlessResourceVk().bind(vkCmdBuf->hnd(), vkBindPt);
		vkCmdBindDescriptorSets(vkCmdBuf->hnd(), vkBindPt, vkPipelineLayoutHnd, set, 1, vkDescrSet.hndArray(), 0, nullptr);
	}
}

void
MaterialPass_Vk::createVkPipeline(Vk_Pipeline& out, Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout)
{
	auto*		rdDevVk		= material()->renderDeviceVk();
	const auto& filename	= shader()->filename();

	VkGraphicsPipelineCreateInfo pipelineCInfo = {};

	Vk_ShaderStagesCDesc vkShaderStagesCDesc;
	vkShaderStagesCDesc.createGraphics(pipelineCInfo, shaderPass());

	if (!_vkPipelineLayout)
	{
		Vk_PipelineLayoutCDesc vkPipelineLayoutCDesc;
		vkPipelineLayoutCDesc.create(_vkPipelineLayout, info(), &vkDescriptorSetLayout(), rdDevVk);
		RDS_VK_SET_DEBUG_NAME_FMT(_vkPipelineLayout, "{}-{}", filename, "vkPipelineLayout");
	}

	Vk_GraphicsPipelineCDesc vkRenderStateCDesc;
	vkRenderStateCDesc.create(pipelineCInfo, info().renderState, pixelStage()->info());

	Vk_VertexInputCDesc vkVtxInputCDesc;
	vkVtxInputCDesc.create(pipelineCInfo, &_vkVertexStage, vtxLayout);

	pipelineCInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCInfo.layout				= _vkPipelineLayout.hnd();
	pipelineCInfo.renderPass			= vkRdPass->hnd();
	pipelineCInfo.subpass				= 0;
	pipelineCInfo.basePipelineHandle	= VK_NULL_HANDLE;	// Optional for creating a new graphics pipeline by deriving from an existing pipeline with VK_PIPELINE_CREATE_DERIVATIVE_BIT
	pipelineCInfo.basePipelineIndex		= -1;				// Optional

	Vk_PipelineCache* vkPipelineCache = VK_NULL_HANDLE;
	out.create(&pipelineCInfo, vkPipelineCache, rdDevVk);
}

void 
MaterialPass_Vk::createComputeVkPipeline(Vk_Pipeline& out)
{
	auto*		rdDevVk		= material()->renderDeviceVk();
	const auto& filename	= shader()->filename();

	VkComputePipelineCreateInfo pipelineCInfo = {};

	if (!_vkPipelineLayout)
	{
		Vk_PipelineLayoutCDesc vkPipelineLayoutCDesc;
		vkPipelineLayoutCDesc.create(_vkPipelineLayout, info(), &vkDescriptorSetLayout(), rdDevVk);
		RDS_VK_SET_DEBUG_NAME_FMT(_vkPipelineLayout, "{}-{}", filename, "vkPipelineLayout");
	}
	
	Vk_ShaderStagesCDesc vkShaderStagesCDesc;
	vkShaderStagesCDesc.createCompute(pipelineCInfo, shaderPass());

	pipelineCInfo.sType		= VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCInfo.layout	= _vkPipelineLayout.hnd();

	Vk_PipelineCache* vkPipelineCache = VK_NULL_HANDLE;
	out.create(&pipelineCInfo, vkPipelineCache, rdDevVk);
}

#endif

#if 0
#pragma mark --- rdsMaterial_Vk-Impl ---
#endif // 0
#if 1

Material_Vk::Material_Vk()
{
}

Material_Vk::~Material_Vk()
{
	destroy();
}

void 
Material_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
}

void 
Material_Vk::onPostCreate(const CreateDesc& cDesc)
{
	Base::onPostCreate(cDesc);
}

void 
Material_Vk::onDestroy()
{
	Base::onDestroy();
}

#endif

}
#endif