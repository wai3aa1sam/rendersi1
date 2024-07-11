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
#pragma mark --- rdsVk_MaterialPass_Stage-Impl ---
#endif // 0
#if 1


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
	
	if (auto* stage = shaderPass->vertexStage())
	{
		_vkVertexStage.create(this, stage);
		_vertexStage = &_vkVertexStage;
	}

	if (auto* stage = shaderPass->tessellationControlStage())
	{
		_vkTescStage.create(this, stage);
		_tescStage = &_vkTescStage;
	}

	if (auto* stage = shaderPass->tessellationEvaluationStage())
	{
		_vkTeseStage.create(this, stage);
		_teseStage = &_vkTeseStage;
	}

	if (auto* stage = shaderPass->geometryStage())
	{
		_vkGeometryStage.create(this, stage);
		_geometryStage = &_vkGeometryStage;
	}

	if (auto* stage = shaderPass->pixelStage())
	{
		_vkPixelStage.create(this, stage);
		_pixelStage	= &_vkPixelStage;
	}

	if (auto* stage = shaderPass->computeStage())
	{
		_vkComputeStage.create(this, stage);
		_computeStage = &_vkComputeStage;
	}

	_vkFramedDescrSets.resize(s_kFrameInFlightCount);
}

void 
MaterialPass_Vk::onDestroy()
{
	//auto* rdDevVk = renderDeviceVk();

	if (_vertexStage)
	{
		_vkVertexStage.destroy();
		_vertexStage = nullptr;
	}

	if (_tescStage)
	{
		_vkTescStage.destroy();
		_tescStage = nullptr;
	}

	if (_teseStage)
	{
		_vkTeseStage.destroy();
		_teseStage = nullptr;
	}

	if (_geometryStage)
	{
		_vkGeometryStage.destroy();
		_geometryStage = nullptr;
	}

	if (_pixelStage)
	{
		_vkPixelStage.destroy();
		_pixelStage	= nullptr;
	}

	if (_computeStage)
	{
		_vkComputeStage.destroy();
		_computeStage	= nullptr;
	}

	_vkFramedDescrSets.clear();

	Base::onDestroy();
}

void 
MaterialPass_Vk::onBind(RenderContext* ctx, const VertexLayout* vtxLayout, RenderPrimitiveType primitive, Vk_CommandBuffer* vkCmdBuf, u32 iFrame)
{
	//auto* rdDevVk	= renderDeviceVk();
	//auto* vkCtx		= sCast<RenderContext_Vk*>(ctx);
	//auto* vkCmdBuf	= vkCtx->graphicsVkCmdBuf();
	auto* vkRdPass	= vkCmdBuf->getVkRenderPass();

	throwIf(!vkRdPass, "no render pass");

	VkPipelineBindPoint vkBindPt = VK_PIPELINE_BIND_POINT_GRAPHICS;
	bindPipeline(vkCmdBuf, vkRdPass, vtxLayout, primitive);
	bindDescriptorSet(vkBindPt, ctx, vkCmdBuf, iFrame);
}

void 
MaterialPass_Vk::onBind(RenderContext* ctx, Vk_CommandBuffer* vkCmdBuf, u32 iFrame)
{
	VkPipelineBindPoint vkBindPt = VK_PIPELINE_BIND_POINT_COMPUTE;

	auto* vkPipelineHnd = shaderPass()->computeVkPipeline().hnd();
	vkCmdBuf->cmd_bindPipeline(vkBindPt, vkPipelineHnd);
	bindDescriptorSet(vkBindPt, ctx, vkCmdBuf, iFrame);
}

void 
MaterialPass_Vk::bindPipeline(Vk_CommandBuffer* vkCmdBuf, Vk_RenderPass* vkRdPass, const VertexLayout* vtxLayout, RenderPrimitiveType primitive)
{
	RDS_CORE_ASSERT(vkCmdBuf, "");
	VkPipelineBindPoint vkBindPt		= VK_PIPELINE_BIND_POINT_GRAPHICS;
	Vk_Pipeline_T*		vkPipelineHnd	= nullptr;

	vkPipelineHnd = shaderPass()->createUnqiueVkPipeline(vkRdPass, vtxLayout, primitive);
	vkCmdBuf->cmd_bindPipeline(vkBindPt, vkPipelineHnd);
}

void 
MaterialPass_Vk::bindDescriptorSet(VkPipelineBindPoint vkBindPt, RenderContext* ctx, Vk_CommandBuffer* vkCmdBuf, u32 iFrame)
{
	auto* rdDevVk	= renderDeviceVk();
	auto* vkCtx		= sCast<RenderContext_Vk*>(ctx);

	{
		//auto* mtl		= pass->material();
		auto& shaderRsc = _framedShaderRscs.shaderResource(iFrame);

		/*
			maybe move to RenderCommand_DrawCall::setMaterial(), but it will have problem too (maybe setParam() then setMaterial twice, will have two rotate in same frame)
			, upload in Render thread will have sync problem
		*/
		RDS_TODO("shaderRsc.uploadToGpu() should in Update loop");
		shaderRsc.uploadToGpu(shaderPass());
		material()->_internal_resetFrame();

		auto& vkDescrSet = _vkFramedDescrSets[iFrame];
		if (!vkDescrSet)
		{
			auto&	descriptorAlloc	= vkCtx->vkRdFrame().descriptorAllocator();
			auto	builder			= Vk_DescriptorBuilder::make(&descriptorAlloc);
			builder.buildBindless(vkDescrSet, shaderPass()->vkDescriptorSetLayout(), shaderRsc, shaderPass());
		}

		auto* vkDescrSetHnd			= vkDescrSet.hnd();
		auto* vkPipelineLayoutHnd	= vkPipelineLayout().hnd();
		auto  setIdx				= sCast<u32>(rdDevVk->bindlessResourceVk().bindlessTypeCount());

		vkCmdBuf->cmd_bindDescriptorSet(vkBindPt, setIdx, vkDescrSetHnd, vkPipelineLayoutHnd);
	}
}

Vk_PipelineLayout& MaterialPass_Vk::vkPipelineLayout()				{ return shaderPass()->vkPipelineLayout(); }


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