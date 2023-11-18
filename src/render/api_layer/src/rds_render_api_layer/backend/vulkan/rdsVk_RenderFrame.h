#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandBuffer.h"
#include "rds_render_api_layer/backend/vulkan/shader/rdsVk_DescriptorAllocator.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class RenderDevice_Vk;
class RenderContext_Vk;

#if 0
#pragma mark --- rdsVk_RenderFrame-Decl ---
#endif // 0
#if 1

class Vk_RenderFrame : public NonCopyable	// not a derived class of RenderFrame
{
public:
	using Util = Vk_RenderApiUtil;
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kThreadCount		= RenderApiLayerTraits::s_kThreadCount;
	static constexpr SizeType s_kFrameInFlightCount	= RenderApiLayerTraits::s_kFrameInFlightCount;

public:
	Vk_RenderFrame();
	~Vk_RenderFrame();

	Vk_RenderFrame(Vk_RenderFrame&& rhs);
	void operator=(Vk_RenderFrame&& rhs);

	void create	(RenderContext_Vk* rdCtxVk);
	void destroy();

	void clear();

						Vk_CommandBuffer*	requestCommandBuffer	(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel);
	template<size_t N>	void				requestCommandBuffersTo	(Vector<Vk_CommandBuffer*, N>& out, SizeType n, QueueTypeFlags queueType, VkCommandBufferLevel bufLevel);


	void resetCommandPools();
	void resetCommandPool(QueueTypeFlags queueType);

	Vk_CommandPool&			commandPool(QueueTypeFlags queueType);
	Vk_DescriptorAllocator& descriptorAllocator();

	Vk_Semaphore*	imageAvaliableSmp();
	Vk_Semaphore*	renderCompletedSmp();
	Vk_Fence*		inFlightFence();

	RenderDevice_Vk*	rdDevVk();
	RenderContext_Vk*	rdCtxVk();

protected:
	void createCommandPool (Vector<Vk_CommandPool, s_kThreadCount>& cmdPool, u32 familyIdx);
	void destroyCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool);

	void createSyncObjects	();
	void destroySyncObjects	();

protected:
	RenderContext_Vk* _rdCtxVk = nullptr;

	Vector<Vk_CommandPool, s_kThreadCount> _graphicsCommandPools;
	Vector<Vk_CommandPool, s_kThreadCount> _computeCommandPools;
	Vector<Vk_CommandPool, s_kThreadCount> _transferCommandPools;

	Vk_DescriptorAllocator _descriptorAlloc;

	//Vector<Vk_DescriptorPool, s_kThreadCount> // use VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, only reset the pool

	// maybe put back on RenderContext_Vk
	Vk_Semaphore	_imageAvailableVkSmp;
	Vk_Semaphore	_renderCompletedVkSmp;
	Vk_Fence		_inFlightVkFence;
};

template<size_t N> inline
void 
Vk_RenderFrame::requestCommandBuffersTo(Vector<Vk_CommandBuffer*, N>& out, SizeType n, QueueTypeFlags queueType, VkCommandBufferLevel bufLevel)
{
	out.clear();
	out.resize(n);
	for (size_t i = 0; i < n; i++)
	{
		auto& e = out[i];
		e = requestCommandBuffer(queueType, bufLevel);
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

inline Vk_DescriptorAllocator& Vk_RenderFrame::descriptorAllocator() { return _descriptorAlloc; }

inline Vk_Semaphore*	Vk_RenderFrame::imageAvaliableSmp()		{ return &_imageAvailableVkSmp; }
inline Vk_Semaphore*	Vk_RenderFrame::renderCompletedSmp()	{ return &_renderCompletedVkSmp; }
inline Vk_Fence*		Vk_RenderFrame::inFlightFence()			{ return &_inFlightVkFence; }

inline RenderContext_Vk* Vk_RenderFrame::rdCtxVk() { return _rdCtxVk; }



#endif


}
#endif