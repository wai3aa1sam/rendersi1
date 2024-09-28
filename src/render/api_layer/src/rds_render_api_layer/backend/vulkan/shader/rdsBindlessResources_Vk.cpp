#include "rds_render_api_layer-pch.h"
#include "rdsBindlessResources_Vk.h"

#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/backend/vulkan/shader/rdsShader_Vk.h"

#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture2DArray_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture3D_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTextureCube_Vk.h"

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
BindlessResources_Vk::reserve(SizeType size)
{
	SizeType n = this->totalResourcesCount() + size;
	const size_t est = 10;
	_writeDescrSets.reserve(n * est);
	_bufInfos.reserve(		n * est);
	_texInfos.reserve(		n * est);
	_imgInfos.reserve(		n * est);

	//RDS_TODO("image has 6 or more uav, n is not enough, need to update twice or more");
}

void 
BindlessResources_Vk::onCommit_RenderGpuBuffer(RenderGpuBuffer* renderGpuBuf/*, VkWriteDescriptorSet* oVkWriteDescrSet, VkDescriptorBufferInfo* oVkDescrBufInfo*/)
{
	auto& dstSet	= descrSetBuf();
	auto* rdRsc		= renderGpuBuf;
	auto* rdRscVk	= sCast<RenderGpuBuffer_Vk*>(rdRsc);

	if (!rdRsc->isComputeBuffer())
		return;

	auto& oVkDescrInfo = _bufInfos.emplace_back();
	{
		auto& out = oVkDescrInfo;
		out = {};
		out.buffer	= rdRscVk->vkBufHnd();
		out.offset	= 0;
		out.range	= sCast<VkDeviceSize>(rdRsc->bufSize());		// VK_WHOLE_SIZE
	}

	auto& oVkWriteDescrSet = _writeDescrSets.emplace_back();
	{
		auto& out = oVkWriteDescrSet;
		out = {};
		out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		out.dstSet				= dstSet.hnd();
		out.dstBinding			= 0;
		out.dstArrayElement		= sCast<u32>(rdRsc->bindlessHandle().getResourceIndex());
		//out.descriptorType		= BitUtil::has(rscVk->typeFlags(), RenderGpuBufferTypeFlags::Compute) ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		out.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		out.descriptorCount		= 1;
		out.pBufferInfo			= &oVkDescrInfo;
		out.pImageInfo			= nullptr;
		out.pTexelBufferView	= nullptr;
	}
}

void 
BindlessResources_Vk::onCommit_Texture(Texture* texture/*, VkWriteDescriptorSet* oVkWriteDescrSet, VkDescriptorBufferInfo* oVkDescrBufInfo*/)
{
	if (texture->isTexture())
	{
		_onCommit_Texture(texture);
	}
	if (texture->isImage())
	{
		_onCommit_Image(texture);
	}
}

void 
BindlessResources_Vk::_onCommit_Texture(Texture* texture/*, VkWriteDescriptorSet* oVkWriteDescrSet, VkDescriptorBufferInfo* oVkDescrBufInfo*/)
{
	auto& dstSet	= descrSetTex();
	auto* rdRsc		= texture;
	//auto* rscVk		= sCast<Texture_Vk*>(texture);
	auto  type		= rdRsc->type(); RDS_UNUSED(type);

	auto*	srvVkImgViewHnd = Vk_Texture::getSrvVkImageViewHnd(rdRsc);
	RDS_CORE_ASSERT(srvVkImgViewHnd,	"rsc {}: getSrvVkImageViewHnd == nullptr, refCount: {}", rdRsc->debugName(), rdRsc->_refCount.load());
	RDS_CORE_ASSERT(rdRsc->isTexture(), "invalid Texture usage, usage has no TextureUsageFlags::ShaderResource or TextureUsageFlags::DepthStencil");

	auto& oVkDescrInfo = _texInfos.emplace_back();
	{
		auto& out = oVkDescrInfo;
		out = {};
		out.imageLayout	= BitUtil::has(rdRsc->usageFlags(), TextureUsageFlags::DepthStencil) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		out.imageView	= srvVkImgViewHnd;
		out.sampler		= nullptr;
	}

	auto& oVkWriteDescrSet = _writeDescrSets.emplace_back();
	{
		auto& out = oVkWriteDescrSet;
		out = {};
		out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		out.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		out.dstSet				= dstSet.hnd();
		out.dstBinding			= sCast<u32>(_vkSamplers.size());
		out.dstArrayElement		= sCast<u32>(rdRsc->bindlessHandle().getResourceIndex());
		out.descriptorCount		= 1;
		out.pBufferInfo			= nullptr;
		out.pImageInfo			= &oVkDescrInfo;
		out.pTexelBufferView	= nullptr;
	}
}

void 
BindlessResources_Vk::_onCommit_Image(Texture* texture/*, VkWriteDescriptorSet* oVkWriteDescrSet, VkDescriptorBufferInfo* oVkDescrBufInfo*/)
{
	auto& dstSet	= descrSetImg();
	auto* rdRsc		= texture;
	//auto* rscVk		= sCast<Texture_Vk*>(texture);
	auto  type		= rdRsc->type(); RDS_UNUSED(type);

	RDS_CORE_ASSERT(rdRsc->isImage(), "invalid Texture usage, usage has no TextureUsageFlags::UnorderedAccess");

	for (u32 i = 0; i < rdRsc->mipmapViewCount(); i++)
	{
		auto* uavVkImgHnd	= Vk_Texture::getUavVkImageViewHnd(rdRsc, i);
		RDS_CORE_ASSERT(uavVkImgHnd, "getUavVkImageViewHnd[{}] == nullptr, refCount: {}", i, rdRsc->_refCount.load());

		auto& oVkDescrInfo = _imgInfos.emplace_back();
		{
			auto& out = oVkDescrInfo;
			out = {};
			out.imageLayout	= VK_IMAGE_LAYOUT_GENERAL;
			out.imageView	= uavVkImgHnd;
			out.sampler		= nullptr;
		}

		auto& oVkWriteDescrSet = _writeDescrSets.emplace_back();
		{
			auto& out = oVkWriteDescrSet;
			out = {};
			out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			out.descriptorType		= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			out.dstSet				= dstSet.hnd();
			out.dstBinding			= 0;
			out.dstArrayElement		= sCast<u32>(rdRsc->uavBindlessHandle().getResourceIndex(i));
			out.descriptorCount		= 1;
			out.pBufferInfo			= nullptr;
			out.pImageInfo			= &oVkDescrInfo;
			out.pTexelBufferView	= nullptr;
		}
	}
}

void 
BindlessResources_Vk::onCommit()
{
	auto* rdDevVk			= renderDeviceVk();
	auto& writeDescrSets	= _writeDescrSets;
	if (!writeDescrSets.is_empty())
	{
		vkUpdateDescriptorSets(rdDevVk->vkDevice(), sCast<u32>(writeDescrSets.size()), writeDescrSets.data(), 0, nullptr);
	}

	writeDescrSets.clear();
	_bufInfos.clear();
	_texInfos.clear();
	_imgInfos.clear();
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
	_descrAlloc.alloc(&dstSet, &dstLayout, false);
}

#endif




}