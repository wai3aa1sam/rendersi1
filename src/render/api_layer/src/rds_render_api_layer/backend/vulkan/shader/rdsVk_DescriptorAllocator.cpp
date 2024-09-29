#include "rds_render_api_layer-pch.h"
#include "rdsVk_DescriptorAllocator.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/buffer/rdsRenderGpuBuffer_Vk.h"
#include "rds_render_api_layer/backend/vulkan/texture/rdsTexture_Vk.h"

#include "rdsShader_Vk.h"

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
	for (auto* e : _pools)
	{
		e->destroy(_rdDevVk);
	}
	_freePools.clear();
	_pools.clear();
	_alloc.clear();

	_rdDevVk = nullptr;
}

void 
Vk_DescriptorAllocator::reset()
{
	RDS_TODO("seesm should have a smutex to lock, but since we are using 4 frame design, all material should be held until next same frame idx, but it will wait beforehand");
	RDS_TODO("alothugh currently bindDescriptorSet is in RenderThread, maybe bind all first in UpdateThread when create? then no need smutex, otherwise, we need a smutex");
	RDS_TODO("I think bind all first in UpdateThread is better, it is non-sense to create material in other threads");
	for (auto* e : _pools)
	{
		auto usedCount = e->usedCount.load();
		if (usedCount == 0 
			&& /*bool isUnique = */_freePools.find(e) == _freePools.end())
		{
			e->reset(sCast<VkDescriptorPoolResetFlags>(0), _rdDevVk);
			_freePools.emplace_back(rds::move(e));
		}
	}

	// this mechanism seems ok, but it will slowly increase pool size, 100 mins -> 12 pools, seems some reseted pools cannot allocate...
	#if 0
	{
		RDS_LOG("// === \n _pools size: {}, free pools size: {}", _pools.size(), _freePools.size());
		TempString buf;
		for (size_t i = 0; i < _pools.size(); ++i)
		{
			auto* e = _pools[i];
			RDS_LOG("_pools[i] usedCount: {}", e->usedCount.load());
		}
	}
	#endif // 0
}

void
Vk_DescriptorAllocator::alloc(Vk_DescriptorSet* oSet, const Vk_DescriptorSetLayout* layout, bool checkValid)
{
	#if 0
	if (checkValid)
		checkRenderThreadExclusive(RDS_SRCLOC);		// need think the statement in Vk_DescriptorAllocator::reset() when multi thread
	#endif // 1

	RDS_ASSERT(_curPool && _curPool->hnd(), "invalid _curPool");

	Vk_DescriptorSet& out = *oSet;
	auto ret = createSet(out, layout, _curPool);

	while (!Util::isSuccess(ret))
	{
		_curPool = requestPool();
		ret = createSet(out, layout, _curPool);
	}
	Util::throwIfError(ret);
	//Util::throwIfError(ret);
	RDS_ASSERT(out, "alloc vkDescrSet failed");
	RDS_VK_SET_DEBUG_NAME_SRCLOC(out);
}

Vk_DescriptorPool*
Vk_DescriptorAllocator::requestPool()
{
	Vk_DescriptorPool* out = nullptr;

	if (!_freePools.is_empty())
	{
		out = _freePools.moveBack();
	}
	else
	{
		createPool(&out, _cDesc.poolSizes, _cDesc.descrCount, _cDesc.cFlag, _rdDevVk);
		RDS_VK_SET_DEBUG_NAME_FMT(*out, "VkDescPools[{}]", _pools.size());
	}

	throwIf(!out, "requestPool() failed");

	return out;
}

VkResult 
Vk_DescriptorAllocator::createPool(Vk_DescriptorPool** out, const PoolSizes& poolSizes, u32 setReservedSize, VkDescriptorPoolCreateFlags cFlag, RenderDevice_Vk* rdDevVk)
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

	auto& vkDescrPool = *out;
	vkDescrPool = _pools.emplace_back(_alloc.newT<Vk_DescriptorPool>());
	auto ret = vkDescrPool->create(&poolCInfo, _rdDevVk);
	Util::throwIfError(ret);
	return ret;
}

VkResult 
Vk_DescriptorAllocator::createSet(Vk_DescriptorSet& out, const Vk_DescriptorSetLayout* layout, Vk_DescriptorPool* pool)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	//allocInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool		= pool->hnd();
	allocInfo.descriptorSetCount	= 1;
	allocInfo.pSetLayouts			= layout->hndArray();

	return out.create(pool, &allocInfo, _rdDevVk);
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
Vk_DescriptorBuilder::build(Vk_DescriptorSet& dstSet, const Vk_DescriptorSetLayout& layout, ShaderResources& shaderRscs, ShaderPass_Vk* pass)
{
	_notYetSupported(RDS_SRCLOC);	/// all vk info need resize first, not enought will trigger resize and gg
	_alloc->alloc(&dstSet, &layout, true);
	if (!dstSet)
	{
		return false;
	}
	create(shaderRscs, pass);

	RDS_TODO("since the TransferContext is committed before RenderContext, using staging will not cause it failed to upload (cleared)");
	shaderRscs.uploadToGpu(pass);

	#define RDS_TEMP_BIND_TEX_WITH_SAMPLER 1
	#define RDS_TEMP_COMBINED_TEXTURE 1

	auto shaderStageFlag = Util::toVkShaderStageBit(shaderRscs.info().stageFlag);

	for (auto& e : shaderRscs.constBufs())		bindConstantBuffer	(dstSet, e, shaderStageFlag, pass);

	#if !RDS_TEMP_COMBINED_TEXTURE
	#if !RDS_TEMP_BIND_TEX_WITH_SAMPLER

	for (auto& e : shaderRscs.texParams())		bindTexture	(dstSet, e, shaderStageFlag);
	for (auto& e : shaderRscs.samplerParams())	bindSampler	(dstSet, e, shaderStageFlag);

	#else

	for (auto& e : shaderRscs.texParams())		bindTextureWithSampler	(dstSet, e, shaderRscs, shaderStageFlag);

	#endif // 0

	#else

	for (auto& e : shaderRscs.texParams())		bindCombinedTexture	(dstSet, e, shaderStageFlag, pass);

	#endif // 0


	//for (auto& e : shaderRscs.bufferParams())		bindBuffer	(dstSet, e, shaderStageFlag);


	if (!_writeDescs.is_empty())
		vkUpdateDescriptorSets(_alloc->vkDevHnd(), sCast<u32>(_writeDescs.size()), _writeDescs.data(), 0, nullptr);

	clear();

	return true;
}

bool 
Vk_DescriptorBuilder::buildBindless(Vk_DescriptorSet& dstSet, const Vk_DescriptorSetLayout& layout, ShaderResources& shaderRscs, ShaderPass_Vk* pass)
{
	RDS_TODO("_alloc->alloc should be thread safe, material also need to ensure only 1 thread is accessing");
	_alloc->alloc(&dstSet, &layout, true);
	if (!dstSet)
	{
		RDS_THROW("vk descriptor allocate failed");
		//return false;
	}
	create(shaderRscs, pass);

	auto shaderStageFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

	for (auto& e : shaderRscs.constBufs()) bindConstantBuffer(dstSet, e, shaderStageFlag, pass);

	#if !RDS_VK_USE_IMMUTABLE_SAMPLER
	for (auto& e : shaderRscs.samplerParams())
	{
		if (StrUtil::isSame(e.name().c_str(), "rds_samplers"))
		{
			bindSamplers(dstSet, e, pass->renderDeviceVk()->bindlessResourceVk().vkSamplers(), shaderStageFlag, pass);
		}
	}
	#endif // RDS_VK_USE_IMMUTABLE_SAMPLER

	if (!_writeDescs.is_empty())
		vkUpdateDescriptorSets(_alloc->vkDevHnd(), sCast<u32>(_writeDescs.size()), _writeDescs.data(), 0, nullptr);

	clear();

	return true;
}

void 
Vk_DescriptorBuilder::create(ShaderResources& shaderRscs, ShaderPass_Vk* pass)
{
	auto samplerCount = pass->renderDeviceVk()->bindlessResourceVk().vkSamplers().size();

	auto bufInfoCount	= shaderRscs.constBufs().size() + shaderRscs.bufferParams().size();
	auto imageInfoCount = shaderRscs.texParams().size() + shaderRscs.imageParams().size() + shaderRscs.samplerParams().size() + samplerCount;

	_bufInfos.reserve(bufInfoCount);
	_imageInfos.reserve(imageInfoCount);
	_writeDescs.reserve(bufInfoCount + imageInfoCount);
}

void 
Vk_DescriptorBuilder::clear()
{
	_writeDescs.clear();
	_bufInfos.clear();
	_imageInfos.clear();
}

void 
Vk_DescriptorBuilder::bindConstantBuffer(Vk_DescriptorSet& dstSet, ConstBuffer& constBuf, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass)
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
Vk_DescriptorBuilder::bindTexture(Vk_DescriptorSet& dstSet, TexParam& texParam, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass)
{
	const auto& info = texParam.info();

	auto& imageInfo	= _imageInfos.emplace_back();
	imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView		= Vk_Texture::getSrvVkImageViewHnd(texParam.getUpdatedTexture(renderDeviceVk()));
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
Vk_DescriptorBuilder::bindSampler(Vk_DescriptorSet& dstSet, SamplerParam& samplerParam, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass)
{
	#if 1

	_bindSampler(dstSet, samplerParam, pass->renderDeviceVk()->bindlessResourceVk().vkSamplers()[0].hnd(), 0, stageFlag, pass);

	#else

	//throwIf(true, "need a global cache sampler");
	_bindSampler(dstSet, samplerParam, nullptr, stageFlag, pass);

	#endif // 0
}

void 
Vk_DescriptorBuilder::bindBuffer(Vk_DescriptorSet& dstSet, BufferParam& bufParam, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass)
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
Vk_DescriptorBuilder::bindCombinedTexture(Vk_DescriptorSet& dstSet, TexParam& texParam, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass)
{
	if (!texParam._tex)
		return;

	const auto& info = texParam.info();

	auto& imageInfo	= _imageInfos.emplace_back();
	imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView		= Vk_Texture::getSrvVkImageViewHnd(texParam.getUpdatedTexture(renderDeviceVk()));
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
Vk_DescriptorBuilder::bindTextureWithSampler(Vk_DescriptorSet& dstSet, TexParam& texParam, const ShaderResources& shaderRscs, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass)
{
	if (texParam.info().isBindlessResource())
		return;

	bindTexture(dstSet, texParam, stageFlag, pass);

	TempString samplerName;
	ShaderResources::getSamplerNameTo(samplerName, texParam.name());
	if (auto* samplerParam = shaderRscs.findSamplerParam(samplerName))
	{
		auto& bindlessRscVk = renderDeviceVk()->bindlessResourceVk();
		auto samplerIdx = bindlessRscVk.findSamplerIndex(samplerParam->samplerState());
		_bindSampler(dstSet, *samplerParam, bindlessRscVk.vkSamplers()[samplerIdx].hnd(), 0, stageFlag, pass);
	}
}

void 
Vk_DescriptorBuilder::_bindSampler(Vk_DescriptorSet& dstSet, const SamplerParam& samplerParam, Vk_Sampler_T* samplerHnd, u32 dstIdx, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass)
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
	out.dstArrayElement		= dstIdx;
	out.descriptorType		= Util::toVkDescriptorType(info.paramType());
	out.descriptorCount		= 1;
	out.pBufferInfo			= nullptr;
	out.pImageInfo			= &imageInfo;
	out.pTexelBufferView	= nullptr;
}

void 
Vk_DescriptorBuilder::bindSamplers(Vk_DescriptorSet& dstSet, const SamplerParam& samplerParam, Span<Vk_Sampler> samplers, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass)
{
	for (u32 i = 0; i < samplers.size(); i++)
	{
		auto& sampler = samplers[i];
		_bindSampler(dstSet, samplerParam, sampler.hnd(), i, stageFlag, pass);
	}
}

#endif

}

#endif