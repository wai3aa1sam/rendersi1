#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/common/rdsRenderResource_Vk.h"

#include "rds_render_api_layer/shader/rdsBindlessResources.h"
#include "rdsVk_DescriptorAllocator.h"

namespace rds
{

#if 0
#pragma mark --- rdsBindlessResources_Vk-Decl ---
#endif // 0
#if 1

class BindlessResources_Vk : public RenderResource_Vk<BindlessResources>
{
public:


public:
	BindlessResources_Vk();
	virtual ~BindlessResources_Vk();

public:
	void bind(Vk_CommandBuffer_T* vkCmdBufHnd, VkPipelineLayout_T* vkPipelineLayoutHnd);

	template<size_t N> void getDescriptorSetLayoutTo(Vector<Vk_DescriptorSetLayout_T*, N>& o);


public:
	u32 descriptorCount() const;

	Span<Vk_Sampler> vkSamplers();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onCommit();

private:
	void _createDescritporSet(Vk_DescriptorSet& dstSet, Vk_DescriptorSetLayout& dstLayout, VkDescriptorType type, SizeType descrCount);

protected:
	Vk_DescriptorAllocator	_descrAlloc;

	Vk_DescriptorSet _descrSetBuf;
	Vk_DescriptorSet _descrSetTex;
	Vk_DescriptorSet _descrSetImg;
	Vk_DescriptorSet _descrSetSampler;

	Vk_DescriptorSetLayout		_descrSetLayoutBuf;
	Vk_DescriptorSetLayout		_descrSetLayoutTex;
	Vk_DescriptorSetLayout		_descrSetLayoutImg;
	Vk_DescriptorSetLayout		_descrSetLayoutSampler;

	Vector<Vk_Sampler, 64> _vkSamplers;
};

template<size_t N> inline
void 
BindlessResources_Vk::getDescriptorSetLayoutTo(Vector<Vk_DescriptorSetLayout_T*, N>& o)
{
	o.emplace_back(_descrSetLayoutBuf.hnd());
	o.emplace_back(_descrSetLayoutTex.hnd());
	o.emplace_back(_descrSetLayoutImg.hnd());
	o.emplace_back(_descrSetLayoutSampler.hnd());;
}

inline u32 BindlessResources_Vk::descriptorCount() const { return _descrAlloc.descriptorCount(); }

inline Span<Vk_Sampler> BindlessResources_Vk::vkSamplers() { return _vkSamplers; }

#endif

}