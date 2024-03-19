#include "rds_render_api_layer-pch.h"
#include "rdsBindlessResources_Vk.h"

#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

namespace rds
{

#if 0
#pragma mark --- rdsBindlessResources_Vk-Impl ---
#endif // 0
#if 1

BindlessResources_Vk::BindlessResources_Vk()
{

}

BindlessResources_Vk::~BindlessResources_Vk()
{
	destroy();
}

void 
BindlessResources_Vk::onCreate(const CreateDesc& cDesc)
{
	Base::onCreate(cDesc);

	auto* rdDevVk = renderDeviceVk();

	auto descrAllocCDesc = _descrAlloc.makeCDesc();
	descrAllocCDesc.descrCount = cDesc.size;
	descrAllocCDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,				1.0f });
	descrAllocCDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,					1.0f });	// VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	descrAllocCDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,					1.0f });
	//descrAllocCDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,	1.0f });

	_descrAlloc.createBindless(descrAllocCDesc, rdDevVk);

	_descrSets.resize(bindlessTypeCount());
	_descrSetLayouts.resize(bindlessTypeCount());

	_createDescritporSet(descrSetBuf(),		descrSetLayoutBuf(),		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,	descriptorCount());	descrSetBuf().setDebugName("bindless_buffer",	rdDevVk);
	_createDescritporSet(descrSetTex(),		descrSetLayoutTex(),		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,	descriptorCount()); descrSetTex().setDebugName("bindless_texture",	rdDevVk);
	_createDescritporSet(descrSetImg(),		descrSetLayoutImg(),		VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,	descriptorCount());	descrSetImg().setDebugName("bindless_image",	rdDevVk);

	{
		Vector<Vk_DescriptorSetLayout_T*, 8> layoutHnds;
		layoutHnds.reserve(_descrSetLayouts.size());
		for (auto& layout : _descrSetLayouts)
		{
			layoutHnds.emplace_back(layout.hnd());
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		{
			pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount			= sCast<u32>(_descrSetLayouts.size());			// Optional
			pipelineLayoutInfo.pSetLayouts				= layoutHnds.data();						// set0, set1, set2, ...
			pipelineLayoutInfo.pushConstantRangeCount	= 0;		// Optional
			pipelineLayoutInfo.pPushConstantRanges		= nullptr;	// Optional
		}
		_vkPipelineLayoutCommon.create(&pipelineLayoutInfo, rdDevVk);
	}
}

void 
BindlessResources_Vk::onDestroy()
{
	auto* rdDevVk = renderDeviceVk();

	_vkPipelineLayoutCommon.destroy(rdDevVk);

	for (auto& layout : _descrSetLayouts)
	{
		layout.destroy(rdDevVk);
	}
	_descrSetLayouts.clear();
	_descrSets.clear();

	_descrAlloc.destroy();

	for (auto& e : _vkSamplers)
	{
		e.destroy(rdDevVk);
	}
	_vkSamplers.clear();

	Base::onDestroy();
}

void 
BindlessResources_Vk::onCommit()
{
	// update descriptor set
	{
		Vector<VkWriteDescriptorSet,	64>	writeDescs;
		Vector<VkDescriptorImageInfo,	64>	imgInfos;
		Vector<VkDescriptorBufferInfo,	64>	bufInfos;

		auto* rdDevVk = renderDeviceVk();

		{
			auto data = _texAlloc.scopedULock();
			auto& rscs = data->rscs;

			writeDescs.reserve	(writeDescs.size()	+ rscs.size());
			imgInfos.reserve	(imgInfos.size()	+ rscs.size());

			for (auto& rsc : rscs)
			{
				{
					auto& dstSet = descrSetTex();

					RDS_CORE_ASSERT(Vk_Texture::getVkImageViewHnd(rsc), "_vkImageView == nullptr");

					auto& info	= imgInfos.emplace_back();
					info = {};
					info.imageLayout	= BitUtil::has(rsc->usageFlags(), TextureUsageFlags::DepthStencil) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					info.imageView		= Vk_Texture::getVkImageViewHnd(rsc);
					info.sampler		=  nullptr;
					//info.sampler		= Vk_Texture::getVkSamplerHnd(rsc);

					auto& out = writeDescs.emplace_back();
					out = {};
					out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					out.dstSet				= dstSet.hnd();
					out.dstBinding			= sCast<u32>(_vkSamplers.size());
					out.dstArrayElement		= sCast<u32>(rsc->bindlessHandle().getResourceIndex());
					out.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					out.descriptorCount		= 1;
					out.pBufferInfo			= nullptr;
					out.pImageInfo			= &info;
					out.pTexelBufferView	= nullptr;
				}
			}
		}

		{
			auto data = _bufAlloc.scopedULock();
			auto& rscs = data->rscs;

			bufInfos.reserve(bufInfos.size()		+ rscs.size());
			writeDescs.reserve(writeDescs.size()	+ rscs.size());

			for (auto& rsc : rscs)
			{
				{
					auto& dstSet	= descrSetBuf();
					auto* rscVk		= sCast<RenderGpuBuffer_Vk*>(rsc.ptr());

					auto& info	= bufInfos.emplace_back();
					info = {};
					info.buffer	= rscVk->vkBufHnd();
					info.offset	= 0;
					info.range	= sCast<VkDeviceSize>(rscVk->bufSize());		// VK_WHOLE_SIZE

					auto& out = writeDescs.emplace_back();
					out = {};
					out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					out.dstSet				= dstSet.hnd();
					out.dstBinding			= 0;
					out.dstArrayElement		= sCast<u32>(rsc->bindlessHandle().getResourceIndex());
					//out.descriptorType		= BitUtil::has(rscVk->typeFlags(), RenderGpuBufferTypeFlags::Compute) ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					out.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					out.descriptorCount		= 1;
					out.pBufferInfo			= &info;
					out.pImageInfo			= nullptr;
					out.pTexelBufferView	= nullptr;
				}
			}
		}

		vkUpdateDescriptorSets(rdDevVk->vkDevice(), sCast<u32>(writeDescs.size()), writeDescs.data(), 0, nullptr);
	}

	// clean up
	{
		auto data = _texAlloc.scopedULock();
		auto& rscs = data->rscs;
		rscs.clear();
	}

	{
		auto data = _bufAlloc.scopedULock();
		auto& rscs = data->rscs;
		rscs.clear();
	}
}

void 
BindlessResources_Vk::bind(Vk_CommandBuffer_T* vkCmdBufHnd, VkPipelineBindPoint bindPt)
{
	Vector<Vk_DescriptorSet_T*, 8> descrSets;
	for (auto& set : _descrSets)
	{
		descrSets.emplace_back(set.hnd());
	}

	VkPipelineLayout_T* vkPipelineLayoutHnd = vkPipelineLayoutCommon().hnd();
	u32 firstSet = 0;

	//VkPipelineLayout_T* vkPipelineLayoutHnd = nullptr;
	vkCmdBindDescriptorSets(vkCmdBufHnd, bindPt,
							vkPipelineLayoutHnd
							, firstSet, sCast<u32>(descrSets.size()), descrSets.data()
							, 0, nullptr);

}

void 
BindlessResources_Vk::_createDescritporSet(Vk_DescriptorSet& dstSet, Vk_DescriptorSetLayout& dstLayout, VkDescriptorType type, SizeType descrCount)
{
	auto* rdDevVk = renderDeviceVk();

	Vector<VkDescriptorSetLayoutBinding,	2> bindings;
	Vector<VkDescriptorBindingFlags,		2>	flags;

	auto& e = bindings.emplace_back();
	e.descriptorType		= type;
	e.stageFlags			= Util::toVkShaderStageBit(ShaderStageFlag::All);
	e.binding				= 0;
	e.descriptorCount		= sCast<u32>(descrCount);
	e.pImmutableSamplers	= nullptr;

	flags.emplace_back() = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT /*| VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT*/;

	Vector<Vk_Sampler_T*, 64> vkSamplerHnds;
	if (type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
	{
		vkSamplerHnds.reserve(_samplerStateListTable.size());
		for (const auto& s : _samplerStateListTable)
		{
			auto& vkSampler = _vkSamplers.emplace_back();
			vkSampler.create(*s, rdDevVk);
			vkSamplerHnds.emplace_back(vkSampler.hnd());
		}

		e.binding				= sCast<u32>(vkSamplerHnds.size());

		auto& binding = bindings.emplace_back();
		binding.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLER;
		binding.stageFlags			= Util::toVkShaderStageBit(ShaderStageFlag::All);
		binding.binding				= 0;
		binding.descriptorCount		= sCast<u32>(vkSamplerHnds.size());
		binding.pImmutableSamplers	= vkSamplerHnds.data();

		flags.emplace_back() = {};// VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT /*| VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT*/;
	}

	VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags = {};
	bindingFlags.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	bindingFlags.pNext			= nullptr;
	bindingFlags.pBindingFlags	= flags.data();
	bindingFlags.bindingCount	= sCast<u32>(flags.size());

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = sCast<u32>(bindings.size());
	layoutInfo.pBindings	= bindings.data();
	layoutInfo.flags		= VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	layoutInfo.pNext		= &bindingFlags;

	dstLayout.create(&layoutInfo, rdDevVk);
	dstSet = _descrAlloc.alloc(&dstLayout);
}

#endif

}