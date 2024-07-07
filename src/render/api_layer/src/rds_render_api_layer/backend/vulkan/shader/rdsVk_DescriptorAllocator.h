#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/shader/rdsMaterialPass.h"

// references:
// - https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

struct	MaterialPass_Stage;
class	ShaderPass_Vk;

#if 0
#pragma mark --- rdsVk_DescriptorAllocator-Decl ---
#endif // 0
#if 1

//using DescriptorPoolSizes = Vector<Pair<VkDescriptorType, float>, 32 >;

using Vk_DescriptorTypePair = Pair<VkDescriptorType, float>;

struct DescriptorPoolSizes : public Vector<Vk_DescriptorTypePair, 16 >
{
	//Vector<Vk_DescriptorTypePair, 16 > sizes;
};

struct Vk_DescriptorAllocator_CreateDesc
{
	DescriptorPoolSizes			poolSizes;
	u32							descrCount	= 1000;
	VkDescriptorPoolCreateFlags cFlag		= 0;
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
	void createBindless(CreateDesc& cDesc, RenderDevice_Vk* rdDevVk);

	void destroy();

	void reset();
	void alloc(Vk_DescriptorSet* oSet, const Vk_DescriptorSetLayout* layout);

public:
	u32 descriptorCount() const;

	Vk_Device_T*		vkDevHnd();
	RenderDevice_Vk*	renderDeviceVk();

protected:
	Vk_DescriptorPool* requestPool();

	VkResult createPool	(Vk_DescriptorPool** out, const PoolSizes& poolSizes, u32 setReservedSize, VkDescriptorPoolCreateFlags cFlag, RenderDevice_Vk* rdDevVk);
	VkResult createSet	(Vk_DescriptorSet& out, const Vk_DescriptorSetLayout* layout, Vk_DescriptorPool* pool);

protected:
	void _setDebugName();

private:
	RenderDevice_Vk*	_rdDevVk = nullptr;
	CreateDesc			_cDesc;
	
	Vk_DescriptorPool*				_curPool = nullptr;
	LinearAllocator					_alloc;
	Vector<Vk_DescriptorPool*, 16>	_pools;
	Vector<Vk_DescriptorPool*, 8>	_freePools;
};

inline u32 Vk_DescriptorAllocator::descriptorCount() const { return _cDesc.descrCount; }

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
	using BufferParam	= MaterialPass_Stage::BufferParam;

public:
	static Vk_DescriptorBuilder make(Vk_DescriptorAllocator* alloc);

public:
	bool build(Vk_DescriptorSet& dstSet, const Vk_DescriptorSetLayout& layout, ShaderResources& shaderRscs, ShaderPass_Vk* pass);
	bool buildBindless(Vk_DescriptorSet& dstSet, const Vk_DescriptorSetLayout& layout, ShaderResources& shaderRscs, ShaderPass_Vk* pass);

protected:
	void create(ShaderResources& shaderRscs, ShaderPass_Vk* pass);

	void clear();

	void bindConstantBuffer	(Vk_DescriptorSet& dstSet, ConstBuffer&		constBuf,		VkShaderStageFlags stageFlag, ShaderPass_Vk* pass);
	void bindTexture		(Vk_DescriptorSet& dstSet, TexParam&		texParam,		VkShaderStageFlags stageFlag, ShaderPass_Vk* pass);
	void bindSampler		(Vk_DescriptorSet& dstSet, SamplerParam&	samplerParam,	VkShaderStageFlags stageFlag, ShaderPass_Vk* pass);
	void bindBuffer			(Vk_DescriptorSet& dstSet, BufferParam&		bufParam,		VkShaderStageFlags stageFlag, ShaderPass_Vk* pass);

	void bindCombinedTexture(Vk_DescriptorSet& dstSet, TexParam&		texParam,		VkShaderStageFlags stageFlag, ShaderPass_Vk* pass);

	void bindTextureWithSampler(Vk_DescriptorSet& dstSet, TexParam& texParam, const ShaderResources& shaderRscs, VkShaderStageFlags stageFlag, ShaderPass_Vk* pass);

	void bindSamplers( Vk_DescriptorSet& dstSet, const SamplerParam&	samplerParam, Span<Vk_Sampler> samplerHnds,				VkShaderStageFlags stageFlag, ShaderPass_Vk* pass);
	void _bindSampler( Vk_DescriptorSet& dstSet, const SamplerParam&	samplerParam, Vk_Sampler_T*	   samplerHnd, u32 dstIdx,	VkShaderStageFlags stageFlag, ShaderPass_Vk* pass);

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