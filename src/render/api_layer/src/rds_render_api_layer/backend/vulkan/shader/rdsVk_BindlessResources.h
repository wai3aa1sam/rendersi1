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
	void bind(Vk_CommandBuffer_T* vkCmdBufHnd, VkPipelineBindPoint bindPt);

	template<size_t N> void getDescriptorSetLayoutTo(Vector<Vk_DescriptorSetLayout_T*, N>& o);


public:
	Vk_PipelineLayout& vkPipelineLayoutCommon();

	Vk_DescriptorSet& descrSetBuf();
	Vk_DescriptorSet& descrSetTex();
	Vk_DescriptorSet& descrSetImg();

	Vk_DescriptorSetLayout& descrSetLayoutBuf();
	Vk_DescriptorSetLayout& descrSetLayoutTex();
	Vk_DescriptorSetLayout& descrSetLayoutImg();

	u32 descriptorCount()	const;

	Span<Vk_Sampler> vkSamplers();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onCommit();

private:
	void _createDescritporSet(Vk_DescriptorSet& dstSet, Vk_DescriptorSetLayout& dstLayout, VkDescriptorType type, SizeType descrCount);

protected:
	Vk_DescriptorAllocator	_descrAlloc;

	Vector<Vk_DescriptorSet,		s_kTypeCount> _descrSets;
	Vector<Vk_DescriptorSetLayout,	s_kTypeCount> _descrSetLayouts;

	Vector<Vk_Sampler, 64> _vkSamplers;

	Vk_PipelineLayout _vkPipelineLayoutCommon;
};

template<size_t N> inline
void 
BindlessResources_Vk::getDescriptorSetLayoutTo(Vector<Vk_DescriptorSetLayout_T*, N>& o)
{
	o.reserve(_descrSetLayouts.size());
	for (auto& layout : _descrSetLayouts)
	{
		o.emplace_back(layout.hnd());
	}
}

inline Vk_PipelineLayout& BindlessResources_Vk::vkPipelineLayoutCommon()	{ return _vkPipelineLayoutCommon; }

inline Vk_DescriptorSet& BindlessResources_Vk::descrSetBuf()				{ return _descrSets[enumInt(BindlessResourceType::Buffer)]; }
inline Vk_DescriptorSet& BindlessResources_Vk::descrSetTex()				{ return _descrSets[enumInt(BindlessResourceType::Texture)]; }
inline Vk_DescriptorSet& BindlessResources_Vk::descrSetImg()				{ return _descrSets[enumInt(BindlessResourceType::Image)]; }

inline Vk_DescriptorSetLayout& BindlessResources_Vk::descrSetLayoutBuf()	{ return _descrSetLayouts[enumInt(BindlessResourceType::Buffer)]; }
inline Vk_DescriptorSetLayout& BindlessResources_Vk::descrSetLayoutTex()	{ return _descrSetLayouts[enumInt(BindlessResourceType::Texture)]; }
inline Vk_DescriptorSetLayout& BindlessResources_Vk::descrSetLayoutImg()	{ return _descrSetLayouts[enumInt(BindlessResourceType::Image)]; }

inline u32 BindlessResources_Vk::descriptorCount() const { return _descrAlloc.descriptorCount(); }

inline Span<Vk_Sampler> BindlessResources_Vk::vkSamplers() { return _vkSamplers; }

#endif

}