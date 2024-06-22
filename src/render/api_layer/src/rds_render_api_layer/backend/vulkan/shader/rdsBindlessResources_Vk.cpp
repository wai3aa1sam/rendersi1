#include "rds_render_api_layer-pch.h"
#include "rdsBindlessResources_Vk.h"

#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/backend/vulkan/shader/rdsShader_Vk.h"

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

	_createDescritporSet(descrSetBuf(),		descrSetLayoutBuf(),		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,				descriptorCount());	descrSetBuf().setDebugName("bindless_buffer",		rdDevVk);
	_createDescritporSet(descrSetTex(),		descrSetLayoutTex(),		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				descriptorCount()); descrSetTex().setDebugName("bindless_texture",		rdDevVk);
	_createDescritporSet(descrSetImg(),		descrSetLayoutImg(),		VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				descriptorCount());	descrSetImg().setDebugName("bindless_image",		rdDevVk);
	if (isSupportAccelerationStruct())
	{
		//_createDescritporSet(descrSetAccStruct(),		descrSetLayoutAccStruct(),		VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,	descriptorCount());	descrSetAccStruct().setDebugName("bindless_accStruct",	rdDevVk);
	}

	{
		Vk_PipelineLayoutCDesc vkPipelineLayoutCDesc = {};
		ShaderPassInfo passInfo;
		passInfo.allStageUnionInfo.createDefaultPushConstant();
		vkPipelineLayoutCDesc.create(_vkPipelineLayoutCommon, passInfo, nullptr, rdDevVk);
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

template<class T>
class PersistentVector //: public Vector<>
{
public:
	using SizeType	= size_t;

public:

	~PersistentVector()
	{
		if constexpr (!IsTrivial<T>)
		{
			clear(); 
		}
	}

	void clear()
	{
		_alloc.destructAndClear<T>(_alloc.s_kDefaultAlign);
		_size = 0;
	}

	void reserve(SizeType n)
	{
		_alloc.setChunkSize(computeObjectsSize(n));
	}

	template<class... ARGS>
	T& emplace_back(ARGS&&... args)
	{
		auto* buf = _alloc.alloc(computeObjectsSize(1));
		auto* obj = new (buf) T(rds::forward<ARGS>(args)...);
		_size++;
		return *obj;
	}

public:
	SizeType size() const { return _size; }

	SizeType computeObjectsSize(SizeType n) const { return sizeof(T) * n; }

private:
	LinearAllocator _alloc;
	SizeType		_size = 0;
};

void 
BindlessResources_Vk::onCommit()
{
	RDS_TODO("currently texture and image is using same Alloc, should be separate, then the bindless handle need to modify too, keep simple first");
	// update descriptor set
	{
		/*
		* *** VkDescriptorImageInfo / VkDescriptorBufferInfo must need a non resize linear array!!!
		* PersistentVector is a joke, it will not be linear too when there are other chunks
		* TODO: better handle for this case is to update multiple times, set a maximum
		*/
		static constexpr SizeType s_kLocalSize = 128;
		Vector<VkWriteDescriptorSet,	s_kLocalSize * 3>	writeDescs;
		Vector<VkDescriptorBufferInfo,	s_kLocalSize>		bufInfos;
		Vector<VkDescriptorImageInfo,	s_kLocalSize>		texInfos;
		Vector<VkDescriptorImageInfo,	s_kLocalSize>		imgInfos;

		auto* rdDevVk = renderDeviceVk();

		{
			auto data = _bufAlloc.scopedULock();
			auto& rscs = data->rscs;

			writeDescs.reserve(	writeDescs.size()	+ rscs.size());
			bufInfos.reserve(	bufInfos.size()		+ data->totalResourceCount());

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

		{
			auto data = _texAlloc.scopedULock();
			auto& rscs = data->rscs;

			writeDescs.reserve(writeDescs.size()	+ rscs.size());
			texInfos.reserve(	texInfos.size()		+ data->totalResourceCount());

			auto& dstSet = descrSetTex();
			for (auto& rsc : rscs)
			{
				{
					RDS_CORE_ASSERT(Vk_Texture::getSrvVkImageViewHnd(rsc), "getSrvVkImageViewHnd == nullptr");
					bool isTexture	= BitUtil::hasAny(rsc->usageFlags(), TextureUsageFlags::ShaderResource | TextureUsageFlags::DepthStencil);
					RDS_CORE_ASSERT(isTexture, "invalid Texture usage, usage has no TextureUsageFlags::ShaderResource or TextureUsageFlags::DepthStencil");

					VkDescriptorImageInfo& info	= texInfos.emplace_back();
					info = {};
					info.imageLayout = BitUtil::has(rsc->usageFlags(), TextureUsageFlags::DepthStencil) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					info.imageView		= Vk_Texture::getSrvVkImageViewHnd(rsc);
					info.sampler		=  nullptr;
					//info.sampler		= Vk_Texture::getVkSamplerHnd(rsc);

					VkWriteDescriptorSet out;
					out = {};
					out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					out.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					out.dstSet				= dstSet.hnd();
					out.dstBinding			= sCast<u32>(_vkSamplers.size());
					out.dstArrayElement		= sCast<u32>(rsc->bindlessHandle().getResourceIndex());
					out.descriptorCount		= 1;
					out.pBufferInfo			= nullptr;
					out.pImageInfo			= &info;
					out.pTexelBufferView	= nullptr;

					auto& dst = writeDescs.emplace_back();
					dst = out;
				}
			}
		}

		{
			auto data = _imgAlloc.scopedULock();
			auto& rscs = data->rscs;

			writeDescs.reserve(	writeDescs.size()	+ rscs.size());
			imgInfos.reserve(	imgInfos.size()		+ data->totalResourceCount());

			auto& dstSet = descrSetImg();
			for (auto& rsc : rscs)
			{
				{
					//bool isTexture	= BitUtil::hasAny(rsc->usageFlags(), TextureUsageFlags::ShaderResource | TextureUsageFlags::DepthStencil);
					bool isImage	= BitUtil::hasAny(rsc->usageFlags(), TextureUsageFlags::UnorderedAccess);
					RDS_CORE_ASSERT(isImage, "invalid Texture usage, usage has no TextureUsageFlags::UnorderedAccess");

					for (u32 i = 0; i < rsc->mipmapViewCount(); i++)
					{
						RDS_CORE_ASSERT(Vk_Texture::getUavVkImageViewHnd(rsc, i), "getUavVkImageViewHnd[{}] == nullptr", i);

						VkDescriptorImageInfo& info	= imgInfos.emplace_back();
						info = {};
						info.imageLayout	= VK_IMAGE_LAYOUT_GENERAL;
						info.imageView		= Vk_Texture::getUavVkImageViewHnd(rsc, i);
						info.sampler		=  nullptr;
						//info.sampler		= Vk_Texture::getVkSamplerHnd(rsc);

						VkWriteDescriptorSet out;
						out = {};
						out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						out.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
						out.dstBinding			= 0;
						out.dstSet				= dstSet.hnd();
						out.dstArrayElement		= sCast<u32>(rsc->uavBindlessHandle().getResourceIndex(i));
						out.descriptorCount		= 1;
						out.pBufferInfo			= nullptr;
						out.pImageInfo			= &info;
						out.pTexelBufferView	= nullptr;

						auto& dst = writeDescs.emplace_back();
						dst = out;
					}
				}
			}
		}

		vkUpdateDescriptorSets(rdDevVk->vkDevice(), sCast<u32>(writeDescs.size()), writeDescs.data(), 0, nullptr);
	}

	// clean up
	{
		auto data = _bufAlloc.scopedULock();
		auto& rscs = data->rscs;
		rscs.clear();
	}

	{
		auto data = _texAlloc.scopedULock();
		auto& rscs = data->rscs;
		rscs.clear();
	}

	{
		auto data = _imgAlloc.scopedULock();
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