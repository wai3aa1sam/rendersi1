#include "rds_render_api_layer-pch.h"
#include "rdsVk_DescriptorAllocator.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsVk_DescriptorAllocator-Impl---
#endif // 0
#if 1

Vk_DescriptorAllocator::CreateDesc Vk_DescriptorAllocator::makeCDesc() { return CreateDesc{}; };

Vk_DescriptorAllocator::Vk_DescriptorAllocator()
{

}

Vk_DescriptorAllocator::~Vk_DescriptorAllocator()
{
	destroy();
}

void 
Vk_DescriptorAllocator::create(Renderer_Vk* rdr)
{
	create(makeCDesc(), rdr);
}

void 
Vk_DescriptorAllocator::create(const CreateDesc& cDesc, Renderer_Vk* rdr)
{
	destroy();

	_rdr		= rdr;
	_curPool	= requestPool();
}

void 
Vk_DescriptorAllocator::destroy()
{
	for (auto& e : _freePools)
	{
		e.destroy(_rdr);
	}
	for (auto& e : _usedPools)
	{
		e.destroy(_rdr);
	}

	_freePools.clear();
	_usedPools.clear();

	_curPool.destroy(_rdr);
	_rdr = nullptr;
}

void 
Vk_DescriptorAllocator::reset()
{
	for (auto& e : _usedPools)
	{
		e.reset(sCast<VkDescriptorPoolResetFlags>(0), _rdr);
		_freePools.emplace_back(rds::move(e));
	}
	_usedPools.clear();
	_curPool = requestPool();

	RDS_TODO("handle reset or use different approach to handle [free] desc set, since reset and update in each frame would have great overhead");
}


Vk_DescriptorSet
Vk_DescriptorAllocator::alloc(const Vk_DescriptorSetLayout* layout)
{
	RDS_ASSERT(_curPool.hnd(), "");
	
	Vk_DescriptorSet out;
	auto ret = createSet(out, layout, &_curPool);

	if (!Util::isSuccess(ret))
	{
		_usedPools.emplace_back(rds::move(_curPool));
		_curPool = requestPool();
	}

	ret = createSet(out, layout, &_curPool);
	RDS_ASSERT(out.hnd(), "");

	return out;
}

Vk_DescriptorPool
Vk_DescriptorAllocator::requestPool()
{
	Vk_DescriptorPool out;

	if (!_freePools.is_empty())
	{
		out = _freePools.moveBack();
	}
	else
	{
		createPool(out, _cDesc.poolSizes, _cDesc.setReservedSize, _cDesc.cFlag, _rdr);
	}

	return out;
}

VkResult 
Vk_DescriptorAllocator::createPool(Vk_DescriptorPool& out, const PoolSizes& poolSizes, u32 setReservedSize, VkDescriptorPoolCreateFlags cFlag, Renderer_Vk* rdr)
{
	Vector<VkDescriptorPoolSize, 32> sizes;
	sizes.reserve(poolSizes.sizes.size());
	for (auto sz : poolSizes.sizes) 
	{
		sizes.push_back({ sz.first, sCast<u32>(sz.second * setReservedSize) });
	}

	VkDescriptorPoolCreateInfo poolCInfo = {};
	//poolCInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCInfo.flags			= cFlag;
	poolCInfo.maxSets		= setReservedSize;
	poolCInfo.poolSizeCount = sCast<u32>(sizes.size());
	poolCInfo.pPoolSizes	= sizes.data();

	return out.create(&poolCInfo, _rdr);
}

VkResult 
Vk_DescriptorAllocator::createSet(Vk_DescriptorSet& out, const Vk_DescriptorSetLayout* layout, Vk_DescriptorPool* pool)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	//allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool		= pool->hnd();
	allocInfo.descriptorSetCount	= 1;
	allocInfo.pSetLayouts			= layout->hndArray();

	return out.create(&allocInfo, _rdr);
}

Vk_Device* Vk_DescriptorAllocator::vkDev() { return _rdr->vkDevice(); }

#endif


#if 0
#pragma mark --- rdsVk_DescriptorBuilder-Impl ---
#endif // 0
#if 1

Vk_DescriptorBuilder 
Vk_DescriptorBuilder::make(Vk_DescriptorAllocator* alloc)
{
	Vk_DescriptorBuilder out;
	out._alloc = alloc;
	return out;
}

Vk_DescriptorBuilder::Vk_DescriptorBuilder()
{

}

bool
Vk_DescriptorBuilder::build(Vk_DescriptorSet& dstSet, const Vk_DescriptorSetLayout& layout, const ShaderResources& shaderRscs)
{
	dstSet = _alloc->alloc(&layout);
	if (!dstSet)
	{
		return false;
	}

	#define RDS_TEMP_BIND_TEX_WITH_SAMPLER 1
	
	auto shaderStageFlag = Util::toVkShaderStageBit(shaderRscs.info().stageFlag);

	for (const auto& e : shaderRscs.constBufs())		bindBuffer	(dstSet, e, shaderStageFlag);

	#if !RDS_TEMP_BIND_TEX_WITH_SAMPLER

	for (const auto& e : shaderRscs.texParams())		bindTexture	(dstSet, e, shaderStageFlag);
	for (const auto& e : shaderRscs.samplerParams())	bindSampler	(dstSet, e, shaderStageFlag);

	#else

	for (const auto& e : shaderRscs.texParams())		bindTextureWithSampler	(dstSet, e, shaderRscs, shaderStageFlag);

	#endif // 0

	if (!_writeDescs.is_empty())
		vkUpdateDescriptorSets(_alloc->vkDev(), sCast<u32>(_writeDescs.size()), _writeDescs.data(), 0, nullptr);

	return true;
}

void 
Vk_DescriptorBuilder::bindBuffer(Vk_DescriptorSet& dstSet, const ConstBuffer& constBuf, VkShaderStageFlags stageFlag)
{
	const auto& info = constBuf.info();

	auto& bufInfo	= _bufInfos.emplace_back();
	bufInfo.buffer	= Util::toVkBufHnd(constBuf._gpuBuffer.ptr());
	bufInfo.offset	= 0;
	bufInfo.range	= info.size;

	auto& out = _writeDescs.emplace_back();
	out = {};

	out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	out.dstSet				= dstSet.hnd();
	out.dstBinding			= info.bindPoint;
	out.dstArrayElement		= 0;
	out.descriptorType		= Util::toVkDescriptorType(info.paramType());
	out.descriptorCount		= 1;
	out.pBufferInfo			= &bufInfo;
	out.pImageInfo			= nullptr; // Optional
	out.pTexelBufferView	= nullptr; // Optional
}

void 
Vk_DescriptorBuilder::bindTexture(Vk_DescriptorSet& dstSet, const TexParam& texParam, VkShaderStageFlags stageFlag)
{
	const auto& info = texParam.info();

	auto& imageInfo	= _imageInfos.emplace_back();
	imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView		= Vk_Texture::getImageViewHnd(texParam.getUpdatedTexture());
	//imageInfo.sampler		= _testVkTextureSampler.hnd();

	auto& out = _writeDescs.emplace_back();
	out = {};

	out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	out.dstSet				= dstSet.hnd();
	out.dstBinding			= info.bindPoint;
	out.dstArrayElement		= 0;
	out.descriptorType		= Util::toVkDescriptorType(info.paramType());
	out.descriptorCount		= 1;
	out.pBufferInfo			= nullptr;
	out.pImageInfo			= &imageInfo;
	out.pTexelBufferView	= nullptr;
}

void 
Vk_DescriptorBuilder::bindSampler(Vk_DescriptorSet& dstSet, const SamplerParam& samplerParam, VkShaderStageFlags stageFlag)
{
	throwIf(true, "need a global cache sampler");
	_bindSampler(dstSet, samplerParam, nullptr, stageFlag);
}

void 
Vk_DescriptorBuilder::bindTextureWithSampler(Vk_DescriptorSet& dstSet, const TexParam& texParam, const ShaderResources& shaderRscs, VkShaderStageFlags stageFlag)
{
	bindTexture(dstSet, texParam, stageFlag);
	TempString temp;
	shaderRscs._getAutoSetSamplerNameTo(temp, texParam.name());
	if (auto* samplerParam = shaderRscs.findSamplerParam(temp))
	{
		_bindSampler(dstSet, *samplerParam, Vk_Texture::getSamplerHnd(texParam.getUpdatedTexture()), stageFlag);
	}
}

void 
Vk_DescriptorBuilder::_bindSampler(Vk_DescriptorSet& dstSet, const SamplerParam& samplerParam, Vk_Sampler_T* samplerHnd, VkShaderStageFlags stageFlag)
{
	const auto& info = samplerParam.info();

	auto& imageInfo	= _imageInfos.emplace_back();
	imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.sampler		= samplerHnd;

	auto& out = _writeDescs.emplace_back();
	out = {};

	out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	out.dstSet				= dstSet.hnd();
	out.dstBinding			= info.bindPoint;
	out.dstArrayElement		= 0;
	out.descriptorType		= Util::toVkDescriptorType(info.paramType());
	out.descriptorCount		= 1;
	out.pBufferInfo			= nullptr;
	out.pImageInfo			= &imageInfo;
	out.pTexelBufferView	= nullptr;
}

#endif

}

#endif