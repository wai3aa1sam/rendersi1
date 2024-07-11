#include "rds_render_api_layer-pch.h"
#include "rdsShader_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/shader/rdsShaderCompileRequest.h"

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
#pragma mark --- rdsVk_Descriptor_Helper-Impl ---
#endif // 0
#if 1

struct Vk_Descriptor_Helper
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	template<class INFO, size_t N, class ALLOC> static 
	void 
	createShaderResourceLayoutBinding(Vector<VkDescriptorSetLayoutBinding, N, ALLOC>& dst, const INFO& infos, VkDescriptorType type, ShaderStageFlag stageFlag)
	{
		using Util = Vk_RenderApiUtil;
		for (const auto& paramInfo : infos)
		{
			#if RDS_SHADER_USE_BINDLESS
			bool isBindless =  type	== VK_DESCRIPTOR_TYPE_STORAGE_BUFFER	
				|| type	== VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE		|| type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				|| type == VK_DESCRIPTOR_TYPE_SAMPLER			|| type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			bool isIgnore = isBindless;

			#if !RDS_VK_USE_IMMUTABLE_SAMPLER
			isIgnore &= !StrUtil::isSame(paramInfo.name.c_str(), "rds_samplers");
			#endif // !RDS_VK_USE_IMMUTABLE_SAMPLER

			if (isIgnore)
				continue;

			#endif // RDS_SHADER_USE_BINDLESS

			auto& e = dst.emplace_back();
			e.descriptorType		= type;
			e.stageFlags			= Util::toVkShaderStageBit(stageFlag);
			e.binding				= paramInfo.bindPoint;
			e.descriptorCount		= paramInfo.bindCount;
			e.pImmutableSamplers	= nullptr;
			e.stageFlags			= VK_SHADER_STAGE_ALL;

			RDS_CORE_ASSERT(e.descriptorCount != 0, "");
		}
	}

	static 
	void 
	createVkDescriptorSetLayout(Vk_DescriptorSetLayout* dst, const ShaderStageInfo& info, RenderDevice_Vk* rdDevVk)
	{
		Vector<VkDescriptorSetLayoutBinding, 64> bindings;

		SizeType bindingCount = info.bindingCount();

		bindings.reserve(bindingCount);
		createShaderResourceLayoutBinding(bindings, info.constBufs,		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			info.stageFlag);
		
		#if !RDS_VK_USE_IMMUTABLE_SAMPLER
		createShaderResourceLayoutBinding(bindings, info.samplers,		VK_DESCRIPTOR_TYPE_SAMPLER,					info.stageFlag);
		#endif // RDS_VK_USE_IMMUTABLE_SAMPLER

		#if !RDS_SHADER_USE_BINDLESS
		//createShaderResourceLayoutBinding(bindings, info.textures,		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,			info.stageFlag);
		//createShaderResourceLayoutBinding(bindings, info.samplers,		VK_DESCRIPTOR_TYPE_SAMPLER,					info.stageFlag);

		createShaderResourceLayoutBinding(bindings, info.samplers,		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	info.stageFlag);

		createShaderResourceLayoutBinding(bindings, info.storageBufs,	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			info.stageFlag);
		createShaderResourceLayoutBinding(bindings, info.storageImages,	VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,			info.stageFlag);
		#endif // RDS_NO_BINDLESS

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = sCast<u32>(bindings.size());
		layoutInfo.pBindings	= bindings.data();

		dst->create(&layoutInfo, rdDevVk);
	}
};

#endif


#if 0
#pragma mark --- rdsVk_PipelineCDesc-Impl ---
#endif // 0
#if 1

struct Vk_ShaderStagesCDesc : public Vk_CDesc_Base
{
	using Vk_ShaderStageCInfos = Vector<VkPipelineShaderStageCreateInfo, Traits::s_kShaderStageCount>;

public:
	static constexpr SizeType s_kLocalSize = 8;

public:
	void createGraphics(VkGraphicsPipelineCreateInfo& out, ShaderPass_Vk* shaderPass)
	{
		destroy();
		_shaderStageCInfos.reserve(Traits::s_kShaderStageCount);

		const auto& info = shaderPass->info(); RDS_UNUSED(info);

		shaderPass->createVkShaderStageCInfos(_shaderStageCInfos);
		out.stageCount	= sCast<u32>(_shaderStageCInfos.size());
		out.pStages		= _shaderStageCInfos.data();
	}

	void createCompute(VkComputePipelineCreateInfo& out, ShaderPass_Vk* shaderPass)
	{
		destroy();

		const auto& info = shaderPass->info();

		throwIf(info.csFunc.is_empty(), "cs don't have an entry func");
		shaderPass->createComputeVkShaderStageCInfo(out.stage);
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

	void create(VkGraphicsPipelineCreateInfo& out, const RenderState& rdState, const ShaderStageInfo& psStageInfo, RenderPrimitiveType primitive)
	{
		_createViewportState(				viewportState,			rdState);
		_createInputAssemblyState(			inputAssembly,			rdState, primitive);
		_createRasterizerState(				rasterizer,				rdState);
		_createMultiSampleState(			multisampling,			rdState);
		_createDepthStencilState(			depthStencil,			rdState);
		_createColorBlendAttachmentStates(	colorBlendAttachments,	rdState, psStageInfo);
		_createColorBlendState(				colorBlending,			rdState, colorBlendAttachments);
		_createDynamicState(				dynamicState);

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
			rasterizer.frontFace				= VK_FRONT_FACE_CLOCKWISE;	// VK_FRONT_FACE_CLOCKWISE VK_FRONT_FACE_COUNTER_CLOCKWISE
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

	void _createInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo& out, const RenderState& rdState, RenderPrimitiveType primitive)
	{
		out = {};

		out.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		out.topology				= Util::toVkPrimitiveTopology(primitive);
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
		out.frontFace					= VK_FRONT_FACE_COUNTER_CLOCKWISE;	// VK_FRONT_FACE_CLOCKWISE VK_FRONT_FACE_COUNTER_CLOCKWISE
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
	using Attrs		= Vk_VertexShaderStage::Vk_VertexInputAttrs;
	using Binding	= VkVertexInputBindingDescription;

public:
	void create(VkGraphicsPipelineCreateInfo& out, Vk_VertexShaderStage* vtxStage, const VertexLayout* vtxLayout)
	{
		destroy();

		const auto& vkInputAttrs = vtxStage->createUnqiueVtxInputAttrCDesc(vtxLayout);
		u32 bindingPt = 0;

		binding.binding		= bindingPt;
		binding.stride		= sCast<u32>(vtxLayout ? vtxLayout->stride() : 0);
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
#pragma mark --- rdsVk_ShaderStage-Impl ---
#endif // 0
#if 1

const Vk_VertexShaderStage::Vk_VertexInputAttrs& 
Vk_VertexShaderStage::createUnqiueVtxInputAttrCDesc(const VertexLayout* vtxLayout)
{
	auto& inputMaps = _vtxInputAttrsMap;

	auto vtxInputIter = inputMaps.find(vtxLayout);
	if (vtxInputIter == inputMaps.end())
	{
		Vk_VertexInputAttrs& attrs = inputMaps[vtxLayout];

		u32 bindingPt = 0;

		auto inputCount = info().inputs.size();
		throwIf(inputCount > s_kVtxInputAttrLocalSize, "VkVtxinputAttr local buf overflow");

		for (size_t i = 0; i < inputCount; i++)
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

		/*VkVertexInputBindingDescription bindingDesc = {};
		bindingDesc.binding		= bindingPt;
		bindingDesc.stride		= sCast<u32>(vtxLayout ? vtxLayout->stride() : 0);
		bindingDesc.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;*/

		return attrs;
	}

	return vtxInputIter->second;
}

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
ShaderPass_Vk::onCreate(Shader* shader_, const Info* info, StrView passPath)
{
	Base::onCreate(shader_, info, passPath);

	_shader	= shader_;
	_info	= info;

	Vk_Descriptor_Helper::createVkDescriptorSetLayout(&_vkDescriptorSetLayout, info->allStageUnionInfo, renderDeviceVk());

	if (!info->vsFunc.is_empty())
	{
		_vertexStage	= &_vkVertexStage;
		_vkVertexStage.create(this, passPath);
	}

	if (!_info->tescFunc.is_empty())
	{
		_tescStage = &_vkTescStage;
		_vkTescStage.create(this, passPath);
	}

	if (!_info->teseFunc.is_empty())
	{
		_teseStage = &_vkTeseStage;
		_vkTeseStage.create(this, passPath);
	}

	if (!_info->geomFunc.is_empty())
	{
		_geometryStage = &_vkGeometryStage;
		_vkGeometryStage.create(this, passPath);
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
		createComputeVkPipeline(_computeVkPipeline);
	}
}

void 
ShaderPass_Vk::onDestroy()
{
	auto* rdDevVk = renderDeviceVk();

	if (_vertexStage)
	{
		_vkVertexStage.destroy(this);
		_vertexStage = nullptr;
	}

	if (_tescStage)
	{
		_vkTescStage.destroy(this);
		_tescStage = nullptr;
	}

	if (_teseStage)
	{
		_vkTeseStage.destroy(this);
		_teseStage = nullptr;
	}

	if (_geometryStage)
	{
		_vkGeometryStage.destroy(this);
		_geometryStage = nullptr;
	}

	if (_pixelStage)
	{
		_vkPixelStage.destroy(this);
		_pixelStage = nullptr;
	}

	if (_computeStage)
	{
		_vkComputeStage.destroy(this);
		_computeStage = nullptr;

		_computeVkPipeline.destroy(rdDevVk);
	}

	_vkDescriptorSetLayout.destroy(renderDeviceVk());
	for (auto& e : _vkPipelineMap)
	{
		e.second.destroy(rdDevVk);
	}
	_vkPipelineMap.clear();
	_vkPipelineLayout.destroy(rdDevVk);

	Base::onDestroy();
}

void 
ShaderPass_Vk::createComputeVkShaderStageCInfo(VkPipelineShaderStageCreateInfo& out)
{
	if (!info().csFunc.is_empty()) {out = _vkComputeStage.createVkStageInfo(info().csFunc.c_str()); }
}

Vk_Pipeline_T* 
ShaderPass_Vk::createUnqiueVkPipeline(Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout, RenderPrimitiveType primitive)
{
	//VkPipelineBindPoint vkBindPt		= VK_PIPELINE_BIND_POINT_GRAPHICS;
	Vk_Pipeline_T*		vkPipelineHnd	= nullptr;

	{
		RDS_TODO("add a lock, also _vkPipelineMap can store in shader");
		auto hash = Vk_PipelineHash{ vkRdPass, primitive };
		auto it = _vkPipelineMap.find(hash);
		if (it != _vkPipelineMap.end())
		{
			vkPipelineHnd = it->second.hnd();
		}
		else
		{
			auto& outVkPipeline = _vkPipelineMap[hash];
			createVkPipeline(outVkPipeline, vkRdPass, vtxLayout, primitive);
			vkPipelineHnd = outVkPipeline.hnd();
			RDS_VK_SET_DEBUG_NAME_FMT(outVkPipeline, "{}-{}", shader()->filename(), "vkPipeline");
		}
	}

	return vkPipelineHnd;
}

void
ShaderPass_Vk::createVkPipeline(Vk_Pipeline& out, Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout, RenderPrimitiveType primitive)
{
	auto*		rdDevVk		= renderDeviceVk();
	const auto& filename	= shader()->filename();

	VkGraphicsPipelineCreateInfo pipelineCInfo = {};

	Vk_ShaderStagesCDesc vkShaderStagesCDesc;
	vkShaderStagesCDesc.createGraphics(pipelineCInfo, this);

	if (!_vkPipelineLayout)
	{
		Vk_PipelineLayoutCDesc vkPipelineLayoutCDesc;
		vkPipelineLayoutCDesc.create(_vkPipelineLayout, info(), &vkDescriptorSetLayout(), rdDevVk);
		RDS_VK_SET_DEBUG_NAME_FMT(_vkPipelineLayout, "{}-{}", filename, "vkPipelineLayout");
	}

	Vk_GraphicsPipelineCDesc vkRenderStateCDesc;
	vkRenderStateCDesc.create(pipelineCInfo, info().renderState, pixelStage()->info(), primitive);

	Vk_VertexInputCDesc vkVtxInputCDesc;
	vkVtxInputCDesc.create(pipelineCInfo, &_vkVertexStage, vtxLayout);

	pipelineCInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCInfo.layout				= _vkPipelineLayout.hnd();
	pipelineCInfo.renderPass			= vkRdPass ? vkRdPass->hnd() : nullptr;
	pipelineCInfo.subpass				= 0;
	pipelineCInfo.basePipelineHandle	= VK_NULL_HANDLE;	// Optional for creating a new graphics pipeline by deriving from an existing pipeline with VK_PIPELINE_CREATE_DERIVATIVE_BIT
	pipelineCInfo.basePipelineIndex		= -1;				// Optional

	Vk_PipelineCache* vkPipelineCache = VK_NULL_HANDLE;
	out.create(&pipelineCInfo, vkPipelineCache, rdDevVk);
}

void 
ShaderPass_Vk::createComputeVkPipeline(Vk_Pipeline& out)
{
	auto*		rdDevVk		= renderDeviceVk();
	const auto& filename	= shader()->filename();

	VkComputePipelineCreateInfo pipelineCInfo = {};

	if (!_vkPipelineLayout)
	{
		Vk_PipelineLayoutCDesc vkPipelineLayoutCDesc;
		vkPipelineLayoutCDesc.create(_vkPipelineLayout, info(), &vkDescriptorSetLayout(), rdDevVk);
		RDS_VK_SET_DEBUG_NAME_FMT(_vkPipelineLayout, "{}-{}", filename, "vkPipelineLayout");
	}

	Vk_ShaderStagesCDesc vkShaderStagesCDesc;
	vkShaderStagesCDesc.createCompute(pipelineCInfo, this);

	pipelineCInfo.sType		= VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCInfo.layout	= _vkPipelineLayout.hnd();

	Vk_PipelineCache* vkPipelineCache = VK_NULL_HANDLE;
	out.create(&pipelineCInfo, vkPipelineCache, rdDevVk);
}


RenderDevice_Vk* ShaderPass_Vk::renderDeviceVk() { return shaderVk()->renderDeviceVk(); }

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

}

#endif


#if 0
#pragma mark --- rdsVk_PipelineLayoutCDesc-Impl ---
#endif // 0
#if 1

void 
Vk_PipelineLayoutCDesc::create(Vk_PipelineLayout& out, const ShaderInfo::Pass& passInfo, Vk_DescriptorSetLayout* setLayout, RenderDevice_Vk* rdDevVk)
{
	destroy();

	rdDevVk->bindlessResourceVk().getDescriptorSetLayoutTo(_setLayoutHnds);
	if (setLayout)
	{
		RDS_CORE_ASSERT(setLayout->hnd(), "invalid set layout handle");
		auto* layoutHnd = setLayout->hnd();
		_setLayoutHnds.emplace_back(layoutHnd);
	}
	//if (pass->vertexStage())	_hnds.emplace_back(pass->vkVertexStage_NoCheck()._vkDescriptorSetLayout.hnd());
	//if (pass->pixelStage())		_hnds.emplace_back(pass->vkPixelStage_NoCheck()._vkDescriptorSetLayout.hnd());
	//if (pass->computeStage())	_hnds.emplace_back(pass->vkComputeStage_NoCheck()._vkDescriptorSetLayout.hnd());

	RDS_CORE_ASSERT(!_setLayoutHnds.is_empty(), "no descriptor set layout");

	for (auto& e : passInfo.allStageUnionInfo.pushConstants)
	{
		RDS_CORE_ASSERT(e.size == sizeof(PerObjectParam), " only support PerObjectParam for push_constant");
		RDS_CORE_ASSERT(StrUtil::isSame(e.name.c_str(), "rds_perObjectParam"), "only support rds_perObjectParam for push constant");

		VkPushConstantRange& pushConst = _pushConstantRanges.emplace_back();
		pushConst.offset		= e.offset;
		pushConst.size			= e.size;
		pushConst.stageFlags	= VK_SHADER_STAGE_ALL;
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	{
		pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount			= sCast<u32>(_setLayoutHnds.size());			// Optional
		pipelineLayoutInfo.pSetLayouts				= _setLayoutHnds.data();						// set0, set1, set2, ...
		pipelineLayoutInfo.pushConstantRangeCount	= sCast<u32>(_pushConstantRanges.size());		// Optional
		pipelineLayoutInfo.pPushConstantRanges		= _pushConstantRanges.data();					// Optional
	}
	out.create(&pipelineLayoutInfo, rdDevVk);
}

void 
Vk_PipelineLayoutCDesc::destroy()
{
	_setLayoutHnds.clear();
	_pushConstantRanges.clear();
}

#endif // 1

}
#endif