#include "rds_render_api_layer-pch.h"
#include "rdsVk_DescriptorAllocator.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
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
Vk_DescriptorAllocator::create(RenderDevice_Vk* rdDevVk)
{
	auto cDesc = makeCDesc();
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_SAMPLER,					0.5f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	4.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,			4.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,			1.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,	1.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,	1.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			2.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			2.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1.0f });
	cDesc.poolSizes.emplace_back(Vk_DescriptorTypePair{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,		0.5f });

	create(cDesc, rdDevVk);
}

void 
Vk_DescriptorAllocator::create(const CreateDesc& cDesc, RenderDevice_Vk* rdDevVk)
{
	destroy();

	_cDesc = cDesc;

	_rdDevVk	= rdDevVk;
	_curPool	= requestPool();
}

void 
Vk_DescriptorAllocator::createBindless(CreateDesc& cDesc, RenderDevice_Vk* rdDevVk)
{
	cDesc.cFlag |= VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	create(cDesc, rdDevVk);
}

void 
Vk_DescriptorAllocator::destroy()
{
	for (auto& e : _freePools)
	{
		e.destroy(_rdDevVk);
	}
	for (auto& e : _usedPools)
	{
		e.destroy(_rdDevVk);
	}

	_freePools.clear();
	_usedPools.clear();

	_curPool.destroy(_rdDevVk);
	_rdDevVk = nullptr;
}

void 
Vk_DescriptorAllocator::reset()
{
	for (auto& e : _usedPools)
	{
		e.reset(sCast<VkDescriptorPoolResetFlags>(0), _rdDevVk);
		_freePools.emplace_back(rds::move(e));
	}
	_usedPools.clear();
	//_curPool = requestPool();

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
		ret = createSet(out, layout, &_curPool);
	}

	RDS_TODO("allocation debug name");
	RDS_VK_SET_DEBUG_NAME_SRCLOC(out);

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
		createPool(out, _cDesc.poolSizes, _cDesc.descrCount, _cDesc.cFlag, _rdDevVk);
		RDS_VK_SET_DEBUG_NAME_SRCLOC(out);
	}

	return out;
}

VkResult 
Vk_DescriptorAllocator::createPool(Vk_DescriptorPool& out, const PoolSizes& poolSizes, u32 setReservedSize, VkDescriptorPoolCreateFlags cFlag, RenderDevice_Vk* rdDevVk)
{
	Vector<VkDescriptorPoolSize, 32> sizes;
	sizes.reserve(poolSizes.size());
	for (auto sz : poolSizes) 
	{
		sizes.push_back({ sz.first, sCast<u32>(sz.second * setReservedSize) });
	}

	VkDescriptorPoolCreateInfo poolCInfo = {};
	//poolCInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCInfo.flags			= cFlag;
	poolCInfo.maxSets		= setReservedSize;
	poolCInfo.poolSizeCount = sCast<u32>(sizes.size());
	poolCInfo.pPoolSizes	= sizes.data();

	return out.create(&poolCInfo, _rdDevVk);
}

VkResult 
Vk_DescriptorAllocator::createSet(Vk_DescriptorSet& out, const Vk_DescriptorSetLayout* layout, Vk_DescriptorPool* pool)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	//allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool		= pool->hnd();
	allocInfo.descriptorSetCount	= 1;
	allocInfo.pSetLayouts			= layout->hndArray();

	return out.create(&allocInfo, _rdDevVk);
}

void 
Vk_DescriptorAllocator::_setDebugName()
{

}

Vk_Device_T* Vk_DescriptorAllocator::vkDevHnd() { return _rdDevVk->vkDevice(); }

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
Vk_DescriptorBuilder::build(Vk_DescriptorSet& dstSet, const Vk_DescriptorSetLayout& layout, ShaderResources& shaderRscs)
{
	dstSet = _alloc->alloc(&layout);
	if (!dstSet)
	{
		return false;
	}

	RDS_TODO("since the TransferContext is committed before RenderContext, using staging will not cause it failed to upload (cleared)");
	shaderRscs.uploadToGpu(renderDeviceVk());

	#define RDS_TEMP_BIND_TEX_WITH_SAMPLER 1
	#define RDS_TEMP_COMBINED_TEXTURE 1

	auto shaderStageFlag = Util::toVkShaderStageBit(shaderRscs.info().stageFlag);

	for (auto& e : shaderRscs.constBufs())		bindConstantBuffer	(dstSet, e, shaderStageFlag);

	#if !RDS_TEMP_COMBINED_TEXTURE
	#if !RDS_TEMP_BIND_TEX_WITH_SAMPLER

	for (auto& e : shaderRscs.texParams())		bindTexture	(dstSet, e, shaderStageFlag);
	for (auto& e : shaderRscs.samplerParams())	bindSampler	(dstSet, e, shaderStageFlag);

	#else

	for (auto& e : shaderRscs.texParams())		bindTextureWithSampler	(dstSet, e, shaderRscs, shaderStageFlag);

	#endif // 0

	#else

	for (auto& e : shaderRscs.texParams())		bindCombinedTexture	(dstSet, e.shaderResource(), shaderStageFlag);

	#endif // 0


	//for (auto& e : shaderRscs.bufferParams())		bindBuffer	(dstSet, e, shaderStageFlag);


	if (!_writeDescs.is_empty())
		vkUpdateDescriptorSets(_alloc->vkDevHnd(), sCast<u32>(_writeDescs.size()), _writeDescs.data(), 0, nullptr);

	clear();

	return true;
}

bool 
Vk_DescriptorBuilder::buildBindless(Vk_DescriptorSet& dstSet, const Vk_DescriptorSetLayout& layout, ShaderResources::ConstBuffersView constBufsView, ShaderResources& shaderRscs)
{
	dstSet = _alloc->alloc(&layout);
	if (!dstSet)
	{
		return false;
	}

	for (auto& e : shaderRscs.constBufs()) bindConstantBuffer(dstSet, e, VkShaderStageFlagBits::VK_SHADER_STAGE_ALL);

	if (!_writeDescs.is_empty())
		vkUpdateDescriptorSets(_alloc->vkDevHnd(), sCast<u32>(_writeDescs.size()), _writeDescs.data(), 0, nullptr);

	clear();

	return true;
}

void 
Vk_DescriptorBuilder::clear()
{
	_writeDescs.clear();
	_bufInfos.clear();
	_imageInfos.clear();
}

void 
Vk_DescriptorBuilder::bindConstantBuffer(Vk_DescriptorSet& dstSet, ConstBuffer& constBuf, VkShaderStageFlags stageFlag)
{
	const auto& info = constBuf.info();

	if (!constBuf._gpuBuffer.ptr())
	{
		constBuf.create(&info, _alloc->renderDeviceVk());
	}

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
Vk_DescriptorBuilder::bindTexture(Vk_DescriptorSet& dstSet, TexParam& texParam, VkShaderStageFlags stageFlag)
{
	const auto& info = texParam.info();

	auto& imageInfo	= _imageInfos.emplace_back();
	imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView		= Vk_Texture::getVkImageViewHnd(texParam.getUpdatedTexture(renderDeviceVk()));
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
Vk_DescriptorBuilder::bindSampler(Vk_DescriptorSet& dstSet, SamplerParam& samplerParam, VkShaderStageFlags stageFlag)
{
	throwIf(true, "need a global cache sampler");
	_bindSampler(dstSet, samplerParam, nullptr, stageFlag);
}

void 
Vk_DescriptorBuilder::bindBuffer(Vk_DescriptorSet& dstSet, BufferParam& bufParam, VkShaderStageFlags stageFlag)
{
	const auto& info = bufParam.info();

	auto& bufInfo	= _bufInfos.emplace_back();
	bufInfo.buffer	= Util::toVkBufHnd(bufParam._buffer.ptr());
	bufInfo.offset	= 0;
	bufInfo.range	= bufParam._buffer->bufSize();

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
Vk_DescriptorBuilder::bindCombinedTexture(Vk_DescriptorSet& dstSet, TexParam& texParam, VkShaderStageFlags stageFlag)
{
	if (!texParam._tex)
		return;

	const auto& info = texParam.info();

	auto& imageInfo	= _imageInfos.emplace_back();
	imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView		= Vk_Texture::getVkImageViewHnd(texParam.getUpdatedTexture(renderDeviceVk()));
	//imageInfo.sampler		= Vk_Texture::getVkSamplerHnd(texParam.getUpdatedTexture(renderDeviceVk()));
	imageInfo.sampler		= renderDeviceVk()->bindlessResourceVk().vkSamplers()[0].hnd();

	auto& out = _writeDescs.emplace_back();
	out = {};

	out.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	out.dstSet				= dstSet.hnd();
	out.dstBinding			= info.bindPoint;
	out.dstArrayElement		= 0;
	out.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	out.descriptorCount		= 1;
	out.pBufferInfo			= nullptr;
	out.pImageInfo			= &imageInfo;
	out.pTexelBufferView	= nullptr;
}

void 
Vk_DescriptorBuilder::bindTextureWithSampler(Vk_DescriptorSet& dstSet, TexParam& texParam, const ShaderResources& shaderRscs, VkShaderStageFlags stageFlag)
{
	if (texParam.info().isBindlessResource())
		return;

	bindTexture(dstSet, texParam, stageFlag);

	TempString samplerName;
	ShaderResources::getSamplerNameTo(samplerName, texParam.name());
	if (auto* samplerParam = shaderRscs.findSamplerParam(samplerName))
	{
		_bindSampler(dstSet, samplerParam->shaderResource(), Vk_Texture::getVkSamplerHnd(texParam.getUpdatedTexture(renderDeviceVk())), stageFlag);
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