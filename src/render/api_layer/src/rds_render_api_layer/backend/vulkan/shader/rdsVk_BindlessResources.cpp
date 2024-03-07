#include "rds_render_api_layer-pch.h"
#include "rdsVk_BindlessResources.h"

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
	descrAllocCDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,	1.0f });
	descrAllocCDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,		1.0f });	// VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	descrAllocCDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,		1.0f });
	descrAllocCDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_SAMPLER,			1.0f });

	_descrAlloc.createBindless(descrAllocCDesc, rdDevVk);

	_createDescritporSet(_descrSetBuf,		_descrSetLayoutBuf,		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,	descriptorCount());	_descrSetBuf.setDebugName("bindless_buffer",		rdDevVk);
	_createDescritporSet(_descrSetTex,		_descrSetLayoutTex,		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,	descriptorCount()); _descrSetTex.setDebugName("bindless_texture",		rdDevVk);
	_createDescritporSet(_descrSetImg,		_descrSetLayoutImg,		VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,	descriptorCount());	_descrSetImg.setDebugName("bindless_image",			rdDevVk);
	_createDescritporSet(_descrSetSampler,	_descrSetLayoutSampler, VK_DESCRIPTOR_TYPE_SAMPLER,			descriptorCount());	_descrSetSampler.setDebugName("bindless_sampler",	rdDevVk);
}

void 
BindlessResources_Vk::onDestroy()
{
	auto* rdDevVk = renderDeviceVk();

	_descrSetLayoutBuf.destroy(rdDevVk);
	_descrSetLayoutTex.destroy(rdDevVk);
	_descrSetLayoutImg.destroy(rdDevVk);
	_descrSetLayoutSampler.destroy(rdDevVk);

	/*_descrSetBuf.destroy();
	_descrSetTex.destroy();
	_descrSetImg.destroy();
	_descrSetSampler.destroy();*/

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
	{
		auto data = _texAlloc.scopedULock();
		auto& rscs = data->rscs;

		Vector<VkDescriptorImageInfo,	64>	imageInfos;
		Vector<VkWriteDescriptorSet,	64>	writeDescs;

		auto* rdDevVk = renderDeviceVk();

		for (auto& rsc : rscs)
		{
			{
				auto& dstSet = _descrSetTex;

				auto& imageInfo	= imageInfos.emplace_back();
				imageInfo = {};
				imageInfo.imageLayout	= BitUtil::has(rsc->usageFlags(), TextureUsageFlags::DepthStencil) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView		= Vk_Texture::getVkImageViewHnd(rsc);
				//imageInfo.sampler		= Vk_Texture::getVkSamplerHnd(rsc);

				auto& out = writeDescs.emplace_back();
				out = {};
				out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				out.dstSet				= dstSet.hnd();
				out.dstBinding			= sCast<u32>(_vkSamplers.size());
				out.dstArrayElement		= sCast<u32>(rsc->bindlessHandle().getResourceIndex());
				out.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				out.descriptorCount		= 1;
				out.pBufferInfo			= nullptr;
				out.pImageInfo			= &imageInfo;
				out.pTexelBufferView	= nullptr;
			}

			//{
			//	auto& dstSet = _descrSetSampler;

			//	auto& imageInfo	= imageInfos.emplace_back();
			//	imageInfo = {};
			//	imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			//	//imageInfo.imageView		= Vk_Texture::getVkImageViewHnd(rsc);
			//	imageInfo.sampler		= Vk_Texture::getVkSamplerHnd(rsc);

			//	auto& out = writeDescs.emplace_back();
			//	out = {};
			//	out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//	out.dstSet				= dstSet.hnd();
			//	out.dstBinding			= 0;
			//	out.dstArrayElement		= rsc->bindlessHandle().getResourceIndex();
			//	out.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLER;
			//	out.descriptorCount		= 1;
			//	out.pBufferInfo			= nullptr;
			//	out.pImageInfo			= &imageInfo;
			//	out.pTexelBufferView	= nullptr;
			//}
		}

		vkUpdateDescriptorSets(rdDevVk->vkDevice(), sCast<u32>(writeDescs.size()), writeDescs.data(), 0, nullptr);
		rscs.clear();
	}
}

void 
BindlessResources_Vk::bind(Vk_CommandBuffer_T* vkCmdBufHnd, VkPipelineLayout_T* vkPipelineLayoutHnd)
{
	Vector<Vk_DescriptorSet_T*, 8> descrSets;
	descrSets.emplace_back(	   _descrSetBuf.hnd());
	descrSets.emplace_back(	   _descrSetTex.hnd());
	descrSets.emplace_back(	   _descrSetImg.hnd());
	descrSets.emplace_back(_descrSetSampler.hnd());

	//VkPipelineLayout_T* vkPipelineLayoutHnd = nullptr;
	vkCmdBindDescriptorSets(vkCmdBufHnd, VK_PIPELINE_BIND_POINT_GRAPHICS,
							vkPipelineLayoutHnd
							, 2, sCast<u32>(descrSets.size()), descrSets.data()
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

		flags.emplace_back() = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT /*| VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT*/;
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