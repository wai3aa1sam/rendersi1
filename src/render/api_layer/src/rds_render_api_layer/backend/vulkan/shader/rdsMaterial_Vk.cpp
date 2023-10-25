#include "rds_render_api_layer-pch.h"
#include "rdsMaterial_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderContext_Vk.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandBuffer.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

Vk_DescriptorSetLayout_T* g_testVkDescriptorSetLayout;


SPtr<Material> 
Renderer_Vk::onCreateMaterial(Shader* shader)
{
	auto p = onCreateMaterial();
	p->setShader(shader);
	return p;
}

SPtr<Material> 
Renderer_Vk::onCreateMaterial()
{
	auto p = SPtr<Material>(makeSPtr<Material_Vk>());
	p->create();
	return p;
}


#if 0
#pragma mark --- rdsVk_PipelineCDesc-Impl ---
#endif // 0
#if 1

struct Vk_ShaderStagesCDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Vk_ShaderStages = Vector<VkPipelineShaderStageCreateInfo, enumInt(ShaderStageFlag::_kCount)>;

public:
	static constexpr SizeType s_kLocalSize = 8;

public:
	void create(VkGraphicsPipelineCreateInfo& out, ShaderPass_Vk* shaderPass)
	{
		destroy();
		_shaderStages.reserve(enumInt(ShaderStageFlag::_kCount));

		const auto& info = shaderPass->info();

		if (!info.vsFunc.is_empty()) { _shaderStages.emplace_back(shaderPass->vkVertexStage()->createVkStageInfo(info.vsFunc.c_str())); }
		if (!info.psFunc.is_empty()) { _shaderStages.emplace_back(shaderPass->vkPixelStage ()->createVkStageInfo(info.psFunc.c_str())); }

		out.stageCount	= sCast<u32>(_shaderStages.size());
		out.pStages		= _shaderStages.data();
	}


protected:
	void destroy()
	{
		_shaderStages.clear();
	}

private:
	Vk_ShaderStages _shaderStages;
};

struct Vk_PipelineLayoutCDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kLocalSize = 8;

public:
	void create(Vk_PipelineLayout& out, MaterialPass_Vk* pass, Renderer_Vk* rdr)
	{
		destroy();

		if (pass->vertexStage())	_hnds.emplace_back(pass->vkVertexStage_NoCheck()._vkDescriptorSetLayout.hnd());
		if (pass->pixelStage())		_hnds.emplace_back( pass->vkPixelStage_NoCheck()._vkDescriptorSetLayout.hnd());
		
		RDS_CORE_ASSERT(!_hnds.is_empty(), "no descriptor set layout");

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		{
			pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount			= sCast<u32>(_hnds.size());	// Optional
			pipelineLayoutInfo.pSetLayouts				= _hnds.data();						// Optional
			pipelineLayoutInfo.pushConstantRangeCount	= 0;		// Optional
			pipelineLayoutInfo.pPushConstantRanges		= nullptr;	// Optional
		}
		out.create(rdr, &pipelineLayoutInfo);
	}

protected:
	void destroy()
	{
		_hnds.clear();
	}

private:
	Vector<Vk_DescriptorSetLayout_T*, s_kLocalSize> _hnds;
};

struct Vk_RenderStateCDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	Vk_RenderStateCDesc()
	{
		_dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		_dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);

		dynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount	= sCast<u32>(_dynamicStates.size());
		dynamicState.pDynamicStates		= _dynamicStates.data();
	}

	void create(VkGraphicsPipelineCreateInfo& out)
	{
		out.pInputAssemblyState	= &inputAssembly;
		out.pViewportState		= &viewportState;
		out.pRasterizationState	= &rasterizer;
		out.pMultisampleState	= &multisampling;
		out.pDepthStencilState	= &depthStencil;			// Optional
		out.pColorBlendState	= &colorBlending;
		out.pDynamicState		= &dynamicState;
	}

	void createDefault(VkGraphicsPipelineCreateInfo& out)
	{
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

		create(out);
	}

protected:
	void destroy()
	{
		viewportState			= {};
		inputAssembly			= {};
		rasterizer				= {};
		multisampling			= {};
		depthStencil			= {};
		colorBlendAttachment	= {};
		colorBlending			= {};
		dynamicState			= {};
	}

protected:
	VkPipelineViewportStateCreateInfo		viewportState			= {};
	VkPipelineInputAssemblyStateCreateInfo	inputAssembly			= {};
	VkPipelineRasterizationStateCreateInfo	rasterizer				= {};
	VkPipelineMultisampleStateCreateInfo	multisampling			= {};
	VkPipelineDepthStencilStateCreateInfo	depthStencil			= {};
	VkPipelineColorBlendAttachmentState		colorBlendAttachment	= {};	
	VkPipelineColorBlendStateCreateInfo		colorBlending			= {};
	VkPipelineDynamicStateCreateInfo		dynamicState			= {};

private:
	Vector<VkDynamicState, 8> _dynamicStates;
};

struct Vk_VertexInputCDesc
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

struct MaterialStage_Helper
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	template<class INFO, size_t N, class ALLOC>
	static void 
	createShaderParamLayoutBinding(Vector<VkDescriptorSetLayoutBinding, N, ALLOC>& dst, const INFO& infos, VkDescriptorType type, ShaderStageFlag stageFlag)
	{
		using Util = Vk_RenderApiUtil;
		for (const auto& paramInfo : infos)
		{
			auto& e = dst.emplace_back();
			e.descriptorType		= type;
			e.stageFlags			= Util::toVkShaderStageBit(stageFlag);
			e.binding				= paramInfo.bindPoint;
			e.descriptorCount		= paramInfo.bindCount;
			e.pImmutableSamplers	= nullptr;

			RDS_CORE_ASSERT(e.descriptorCount != 0, "");
		}
	}

	template<class STAGE>
	static void 
	createVkDescriptorSetLayout(Vk_DescriptorSetLayout* dst, STAGE* stage)
	{
		Vector<VkDescriptorSetLayoutBinding, 64> bindings;
		const ShaderStageInfo& info = stage->info();

		SizeType bindingCount = info.bindingCount();

		bindings.reserve(bindingCount);
		createShaderParamLayoutBinding(bindings, info.constBufs,		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			stage->shaderStage()->stageFlag());
		createShaderParamLayoutBinding(bindings, info.textures,			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,			stage->shaderStage()->stageFlag());
		createShaderParamLayoutBinding(bindings, info.samplers,			VK_DESCRIPTOR_TYPE_SAMPLER,					stage->shaderStage()->stageFlag());
		createShaderParamLayoutBinding(bindings, info.storageBufs,		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			stage->shaderStage()->stageFlag());
		createShaderParamLayoutBinding(bindings, info.storageImages,	VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,			stage->shaderStage()->stageFlag());

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = sCast<u32>(bindings.size());
		layoutInfo.pBindings	= bindings.data();

		dst->create(&layoutInfo);
	}


	template<class STAGE> static 
	void 
	createVkDescriptorSet(STAGE* stage)
	{
		createVkDescriptorSetLayout(&stage->_vkDescriptorSetLayout, stage);
		stage->_vkFramedDescSets.resize(s_kFrameInFlightCount);
	}

	template<class STAGE> static 
	void 
	bind(STAGE* stage, u32 set, RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, const VertexLayout* vtxLayout, MaterialPass_Vk* pass)
	{
		if (!stage->shaderStage())
			return;

		auto* mtl = pass->material();

		stage->shaderParams(mtl).uploadToGpu();

		auto& vkDescSet = vkDescriptorSet(stage, mtl);
		if (!vkDescSet)
		{
			auto&	descriptorAlloc	= ctx->renderFrame().descriptorAllocator();
			auto	builder			= Vk_DescriptorBuilder::make(&descriptorAlloc);
			builder.build(vkDescSet, stage->_vkDescriptorSetLayout, stage->shaderParams(mtl));
		}

		VkPipelineBindPoint vkBindPt = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vkCmdBindDescriptorSets(vkCmdBuf->hnd(), vkBindPt, pass->vkPipelineLayout().hnd(), set, 1, vkDescSet.hndArray(), 0, nullptr);
	}

	template<class STAGE>
	static Vk_DescriptorSet&
	vkDescriptorSet(STAGE* stage, Material_Vk* mtl)
	{
		return stage->_vkFramedDescSets[mtl->frameIdx()]; 
	}
};

void
Vk_MaterialPass_VertexStage::create(MaterialPass_Vk* pass, ShaderStage* shaderStage)
{
	RDS_WARN_ONCE("move vk set to material pass, union reflection info");
	Base::create(pass, shaderStage);
	Helper::createVkDescriptorSet(this);
}

void 
Vk_MaterialPass_VertexStage::bind(RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, const VertexLayout* vtxLayout, MaterialPass_Vk* pass)
{
	Helper::bind(this, 0, ctx, vkCmdBuf, vtxLayout, pass);
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
	using Helper = MaterialStage_Helper;
	Helper::createVkDescriptorSet(this);
}

void 
Vk_MaterialPass_PixelStage::bind(RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, const VertexLayout* vtxLayout, MaterialPass_Vk* pass)
{
	Helper::bind(this, 1, ctx, vkCmdBuf, vtxLayout, pass);
}

#endif

#if 0
#pragma mark --- rdsMaterialPass_Vk-Impl ---
#endif // 0
#if 1

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
}

void 
MaterialPass_Vk::onBind(RenderContext* ctx, const VertexLayout* vtxLayout)
{
	auto* vkCtx		= sCast<RenderContext_Vk*>(ctx);
	auto* vkCmdBuf	= vkCtx->vkGraphicsCmdBuf();
	auto* vkRdPass	= vkCmdBuf->getRenderPass();

	throwIf(!vkRdPass, "no render pass");

	bindPipeline(vkCmdBuf, vkRdPass, vtxLayout);

	_vkVertexStage.bind(vkCtx, vkCmdBuf, vtxLayout, this);
	 _vkPixelStage.bind(vkCtx, vkCmdBuf, vtxLayout, this);
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
		}
	}
	
	vkCmdBindPipeline(vkCmdBuf->hnd(), vkBindPt, vkPipeline->hnd());
}

void
MaterialPass_Vk::createVkPipeline(Vk_Pipeline& out, Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout)
{
	Renderer_Vk* rdr = Renderer_Vk::instance();

	VkGraphicsPipelineCreateInfo pipelineCInfo = {};

	#if 1

	Vk_ShaderStagesCDesc vkShaderStagesCDesc;
	vkShaderStagesCDesc.create(pipelineCInfo, shaderPass());

	#else

	VkPipelineShaderStageCreateInfo	vsStageInfo = {};
	VkPipelineShaderStageCreateInfo	psStageInfo = {};

	Vk_ShaderModule vsModule;
	Vk_ShaderModule psModule;

	{
		vsModule.create(Renderer_Vk::instance(), "LocalTemp/imported/shader/asset/shader/test.shader/spirv/pass0/vs_1.1.bin");
		psModule.create(Renderer_Vk::instance(), "LocalTemp/imported/shader/asset/shader/test.shader/spirv/pass0/ps_1.1.bin");

		vsStageInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vsStageInfo.stage					= VK_SHADER_STAGE_VERTEX_BIT;
		vsStageInfo.module					= vsModule.hnd();
		vsStageInfo.pName					= "vs_main";
		vsStageInfo.pSpecializationInfo		= nullptr;

		psStageInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		psStageInfo.stage					= VK_SHADER_STAGE_FRAGMENT_BIT;
		psStageInfo.module					= psModule.hnd();
		psStageInfo.pName					= "ps_main";
		psStageInfo.pSpecializationInfo		= nullptr;

	}
	VkPipelineShaderStageCreateInfo shaderStages[] = { vsStageInfo, psStageInfo };
	pipelineCInfo.stageCount	= 2;
	pipelineCInfo.pStages		= shaderStages;

	#endif // 1

	Vk_PipelineLayoutCDesc vkPipelineLayoutCDesc;
	vkPipelineLayoutCDesc.create(_vkPipelineLayout, this, rdr);

	Vk_RenderStateCDesc vkRenderStateCDesc;
	vkRenderStateCDesc.createDefault(pipelineCInfo);

	Vk_VertexInputCDesc vkVtxInputCDesc;
	vkVtxInputCDesc.create(pipelineCInfo, &_vkVertexStage, vtxLayout);

	pipelineCInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCInfo.layout				= _vkPipelineLayout.hnd();
	pipelineCInfo.renderPass			= vkRdPass->hnd();
	pipelineCInfo.subpass				= 0;
	pipelineCInfo.basePipelineHandle	= VK_NULL_HANDLE;	// Optional for creating a new graphics pipeline by deriving from an existing pipeline with VK_PIPELINE_CREATE_DERIVATIVE_BIT
	pipelineCInfo.basePipelineIndex		= -1;				// Optional

	Vk_PipelineCache* vkPipelineCache = VK_NULL_HANDLE;
	out.create(rdr, &pipelineCInfo, 1, vkPipelineCache);
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
Material_Vk::onCreate()
{
	Base::onCreate();


}

void 
Material_Vk::onPostCreate()
{
	Base::onPostCreate();

}

void 
Material_Vk::onDestroy()
{
	Base::onDestroy();

}

#endif

}
#endif