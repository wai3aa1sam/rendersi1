#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/shader/rdsMaterialPass.h"

// references:
// - https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

struct MaterialPass_Stage;

#if 0
#pragma mark --- rdsVk_DescriptorAllocator-Decl ---
#endif // 0
#if 1

//using DescriptorPoolSizes = Vector<Pair<VkDescriptorType, float>, 32 >;

struct DescriptorPoolSizes
{
	Vector<Pair<VkDescriptorType, float>, 32 > sizes =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER,					0.5f },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	4.0f },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				4.0f },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				1.0f },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,		1.0f },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,		1.0f },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			2.0f },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			2.0f },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1.0f },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1.0f },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			0.5f },
	};
};

struct Vk_DescriptorAllocator_CreateDesc
{
	DescriptorPoolSizes			poolSizes;
	u32							setReservedSize = 1000;
	VkDescriptorPoolCreateFlags cFlag			= 0;
};


class Vk_DescriptorAllocator : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using CreateDesc	= Vk_DescriptorAllocator_CreateDesc;
	using PoolSizes		= DescriptorPoolSizes;
	using Util			= Vk_RenderApiUtil;

public:
	static CreateDesc makeCDesc();

public:
	Vk_DescriptorAllocator();
	~Vk_DescriptorAllocator();

	void create(RenderDevice_Vk* rdDevVk);
	void create(const CreateDesc& cDesc, RenderDevice_Vk* rdDevVk);
	void destroy();

	void reset();
	Vk_DescriptorSet alloc(const Vk_DescriptorSetLayout* layout);

	Vk_Device_T*		vkDevHnd();
	RenderDevice_Vk*	renderDeviceVk();

protected:
	Vk_DescriptorPool requestPool();

	VkResult createPool	(Vk_DescriptorPool& out, const PoolSizes& poolSizes, u32 setReservedSize, VkDescriptorPoolCreateFlags cFlag, RenderDevice_Vk* rdDevVk);
	VkResult createSet	(Vk_DescriptorSet& out, const Vk_DescriptorSetLayout* layout, Vk_DescriptorPool* pool);

protected:
	void _setDebugName();

private:
	RenderDevice_Vk*	_rdDevVk = nullptr;
	Vk_DescriptorPool	_curPool;
	CreateDesc			_cDesc;
	
	Vector<Vk_DescriptorPool, 8> _usedPools;
	Vector<Vk_DescriptorPool, 8> _freePools;
};

inline RenderDevice_Vk* Vk_DescriptorAllocator::renderDeviceVk() { return _rdDevVk; }

#endif


#if 0
#pragma mark --- rdsVk_DescriptorBuilder-Decl ---
#endif // 0
#if 1

class Vk_DescriptorBuilder
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Util			= Vk_RenderApiUtil;
	using ConstBuffer	= MaterialPass_Stage::ConstBuffer;
	using TexParam		= MaterialPass_Stage::TexParam;
	using SamplerParam	= MaterialPass_Stage::SamplerParam;

public:
	static Vk_DescriptorBuilder make(Vk_DescriptorAllocator* alloc);

public:
	void createLayout(Vk_DescriptorSetLayout& layout, MaterialPass_Stage* stage);
	bool build(Vk_DescriptorSet& dstSet, const Vk_DescriptorSetLayout& layout, const ShaderResources& shaderRscs);

protected:
	void bindBuffer	(Vk_DescriptorSet& dstSet, const ConstBuffer&	constBuf,		VkShaderStageFlags stageFlag);
	void bindTexture(Vk_DescriptorSet& dstSet, const TexParam&		texParam,		VkShaderStageFlags stageFlag);
	void bindSampler(Vk_DescriptorSet& dstSet, const SamplerParam&	samplerParam,	VkShaderStageFlags stageFlag);

	void bindTextureWithSampler(Vk_DescriptorSet& dstSet, const TexParam& texParam, const ShaderResources& shaderRscs, VkShaderStageFlags stageFlag);

	void _bindSampler(Vk_DescriptorSet& dstSet, const SamplerParam&	samplerParam, Vk_Sampler_T* samplerHnd,	VkShaderStageFlags stageFlag);

protected:
	Vk_DescriptorBuilder();

	RenderDevice_Vk* renderDeviceVk();

private:
	Vk_DescriptorAllocator* _alloc = nullptr;

	Vector<VkWriteDescriptorSet,	16>	_writeDescs;
	Vector<VkDescriptorBufferInfo,	16>	_bufInfos;
	Vector<VkDescriptorImageInfo,	16>	_imageInfos;
};

inline RenderDevice_Vk* Vk_DescriptorBuilder::renderDeviceVk() { return _alloc->renderDeviceVk(); }


#endif

}

#endif