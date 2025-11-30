#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandBuffer.h"
#include "rds_render_api_layer/backend/vulkan/shader/rdsVk_DescriptorAllocator.h"

#include "rds_render_api_layer/backend/vulkan/pass/rdsVk_FramebufferPool.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class RenderDevice_Vk;
class RenderContext_Vk;

#if 0
#pragma mark --- rdsVk_RenderFrame-Decl ---
#endif // 0
#if 1

class Vk_RenderFrame : public NC_RenderApiLayerCommon_Base	// not a derived class of RenderFrame
{
	RDS_DebugLabel_COMMON_BODY();
public:
	using Util = Vk_RenderApiUtil;
	using SizeType = RenderApiLayerTraits::SizeType;

	static constexpr int _kMaxVkCommandPoolCount = 1;
	using Vk_CommandPools = Vector<Vk_CommandPool, _kMaxVkCommandPoolCount>;

public:
	Vk_FramebufferPool	_vkFramebufPool;	// TODO: remove

public:
	Vk_RenderFrame();
	~Vk_RenderFrame();

	Vk_RenderFrame(Vk_RenderFrame&& rhs);
	void operator=(Vk_RenderFrame&& rhs);

	void create	(RenderDevice_Vk* rdDevVk);
	void destroy();

public:
	void reset(RenderDevice_Vk* rdDevVk);

						Vk_CommandBuffer*	requestCommandBuffer	(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel, StrView debugName);
	template<size_t N>	void				requestCommandBuffersTo	(Vector<Vk_CommandBuffer*, N>& out, SizeType n, QueueTypeFlags queueType, VkCommandBufferLevel bufLevel, StrView debugName);

	void resetCommandPools();
	void resetCommandPool(QueueTypeFlags queueType);

	void setDebugName(StrView name);

public:
	Vk_CommandPool&			commandPool(QueueTypeFlags queueType);

	Vk_DescriptorAllocator& descriptorAllocator();
	Set<Vk_DescriptorSet*>& nonBindlessUpdatedDescriptorSets();

	Vk_Semaphore*	imageAvaliableSmp();
	Vk_Semaphore*	renderCompletedSmp();
	Vk_Fence*		inFlightFence();

	RenderDevice_Vk*	renderDeviceVk();

protected:
	void createCommandPool (Vk_CommandPools& cmdPool, QueueTypeFlags type);
	void destroyCommandPool(Vk_CommandPools& cmdPool);

	void createSyncObjects	();
	void destroySyncObjects	();

protected:
	RenderDevice_Vk* _rdDevVk = nullptr;

	Vk_CommandPools _graphicsCommandPools;
	Vk_CommandPools _computeCommandPools;
	Vk_CommandPools _transferCommandPools;

	// maybe put back on RenderContext_Vk
	Vk_Semaphore	_imageAvailableVkSmp;
	Vk_Semaphore	_renderCompletedVkSmp;
	Vk_Fence		_inFlightVkFence;

	Vk_DescriptorAllocator	_descriptorAlloc;
	Set<Vk_DescriptorSet*>	_nonBindlessUpdatedDescrSets;

	//Vector<Vk_DescriptorPool, s_kThreadCount> // use VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, only reset the pool
};

template<size_t N> inline
void 
Vk_RenderFrame::requestCommandBuffersTo(Vector<Vk_CommandBuffer*, N>& out, SizeType n, QueueTypeFlags queueType, VkCommandBufferLevel bufLevel, StrView debugName)
{
	out.clear();
	out.resize(n);
	for (size_t i = 0; i < n; i++)
	{
		auto& e = out[i];
		e = requestCommandBuffer(queueType, bufLevel, debugName);
	}
}

inline
Vk_CommandPool& 
Vk_RenderFrame::commandPool(QueueTypeFlags queueType)
{
	using SRC = QueueTypeFlags;

	auto tlid = OsTraits::threadLocalId();

	switch (queueType)
	{
		case SRC::Graphics: { return _graphicsCommandPools[tlid]; } break;
		case SRC::Compute:	{ return  _computeCommandPools[tlid]; } break;
		case SRC::Transfer: { return _transferCommandPools[tlid]; } break;
		default: { throwIf(true, ""); } break;
	}

	return _graphicsCommandPools[tlid];
}

inline Vk_DescriptorAllocator& Vk_RenderFrame::descriptorAllocator()				{ return _descriptorAlloc; }
inline Set<Vk_DescriptorSet*>& Vk_RenderFrame::nonBindlessUpdatedDescriptorSets()	{ return _nonBindlessUpdatedDescrSets; }

inline Vk_Semaphore*			Vk_RenderFrame::imageAvaliableSmp()		{ return &_imageAvailableVkSmp; }
inline Vk_Semaphore*			Vk_RenderFrame::renderCompletedSmp()	{ return &_renderCompletedVkSmp; }
inline Vk_Fence*				Vk_RenderFrame::inFlightFence()			{ return &_inFlightVkFence; }

#endif


}
#endif