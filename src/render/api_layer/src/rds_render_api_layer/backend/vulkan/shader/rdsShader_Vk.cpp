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
			#if 0
			if (type	== VK_DESCRIPTOR_TYPE_STORAGE_BUFFER			&& StrUtil::isSame(paramInfo.name.c_str(), "bufferTable")
				|| type	== VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE				&& StrUtil::isSame(paramInfo.name.c_str(), "texture2DTable")
				|| type == VK_DESCRIPTOR_TYPE_SAMPLER					&& StrUtil::isSame(paramInfo.name.c_str(), "samplerTable")
				|| type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER	&& StrUtil::isSame(paramInfo.name.c_str(), "samplerTable")
				)
			{
				continue;
			}
			#endif // 0

			bool isBindless =  type	== VK_DESCRIPTOR_TYPE_STORAGE_BUFFER	
				|| type	== VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE		|| type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				|| type == VK_DESCRIPTOR_TYPE_SAMPLER			|| type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			if (isBindless && !StrUtil::isSame(paramInfo.name.c_str(), "rds_sampler"))
				continue;

			auto& e = dst.emplace_back();
			e.descriptorType		= type;
			e.stageFlags			= Util::toVkShaderStageBit(stageFlag);
			e.binding				= paramInfo.bindPoint;
			e.descriptorCount		= paramInfo.bindCount;
			e.pImmutableSamplers	= nullptr;

			RDS_TODO("manual shader stage flag for optimize?");
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

		if (true)
		{
			createShaderResourceLayoutBinding(bindings, info.samplers,		VK_DESCRIPTOR_TYPE_SAMPLER,					info.stageFlag);
		}
		#if RDS_NO_BINDLESS

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

	#if 0
	template<class STAGE> static 
		void 
		createVkDescriptorSet(STAGE* stage, RenderDevice_Vk* rdDevVk)
	{
		//createVkDescriptorSetLayout(&stage->_vkDescriptorSetLayout, stage, rdDevVk);
		//stage->_vkFramedDescSets.resize(1);
	}

	template<class STAGE> static 
		void 
		bind(STAGE* stage, VkPipelineBindPoint vkBindPt, u32 set, Vk_PipelineLayout_T* vkPipelineLayout, RenderContext_Vk* ctx, Vk_CommandBuffer* vkCmdBuf, const VertexLayout* vtxLayout, MaterialPass_Vk* pass)
	{
		if (!stage->shaderStage())
			return;

		//auto* mtl		= pass->material();
		//auto& shaderRsc = stage->shaderResources(mtl);

		//auto& vkDescSet = vkDescriptorSet(stage, mtl);

		RDS_TODO("rotate rsc, framed resource should in each shader resource, not in resources");

		RDS_TODO("temporary solution for binding, also, DescriptorAllocator is leaking memory since the Vk_DescriptorSet (it's debugName) has not destroy?");

		//if (!vkDescSet || shaderRsc.isDirty() || true)
		{
			auto&	descriptorAlloc	= ctx->vkRdFrame().descriptorAllocator();
			auto	builder			= Vk_DescriptorBuilder::make(&descriptorAlloc);
			//builder.build(vkDescSet, stage->_vkDescriptorSetLayout, shaderRsc);
		}

		//vkCmdBindDescriptorSets(vkCmdBuf->hnd(), vkBindPt, vkPipelineLayout, set, 1, vkDescSet.hndArray(), 0, nullptr);
	}
	#endif // 0
};

#endif

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
ShaderPass_Vk::onCreate(Shader* shader_, const Info* info, StrView passPath)
{
	Base::onCreate(shader_, info, passPath);

	_shader	= shader_;
	_info	= info;

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
	}

	Vk_Descriptor_Helper::createVkDescriptorSetLayout(&_vkDescriptorSetLayout, info->allStageUnionInfo, renderDeviceVk());
}

void 
ShaderPass_Vk::onDestroy()
{
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
	}

	_vkDescriptorSetLayout.destroy(renderDeviceVk());

	Base::onDestroy();
}

void 
ShaderPass_Vk::createComputeVkShaderStageCInfo(VkPipelineShaderStageCreateInfo& out)
{
	if (!info().csFunc.is_empty()) {out = _vkComputeStage.createVkStageInfo(info().csFunc.c_str()); }
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