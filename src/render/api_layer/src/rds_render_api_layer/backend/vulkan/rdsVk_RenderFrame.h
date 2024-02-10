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

class Vk_RenderFrame : public NonCopyable	// not a derived class of RenderFrame
{
public:
	using Util = Vk_RenderApiUtil;
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kThreadCount		= RenderApiLayerTraits::s_kThreadCount;
	static constexpr SizeType s_kFrameInFlightCount	= RenderApiLayerTraits::s_kFrameInFlightCount;

public:
	Vk_FramebufferPool	_vkFramebufPool;	// TODO: remove

public:
	Vk_RenderFrame();
	~Vk_RenderFrame();

	Vk_RenderFrame(Vk_RenderFrame&& rhs);
	void operator=(Vk_RenderFrame&& rhs);

	void create	(RenderContext_Vk* rdCtxVk);
	void destroy();

	void reset();

						Vk_CommandBuffer*	requestCommandBuffer	(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel, StrView debugName);
	template<size_t N>	void				requestCommandBuffersTo	(Vector<Vk_CommandBuffer*, N>& out, SizeType n, QueueTypeFlags queueType, VkCommandBufferLevel bufLevel, StrView debugName);

	void resetCommandPools();
	void resetCommandPool(QueueTypeFlags queueType);

	void setSubmitCount(SizeType n) { _submitCount = n; }

	Vk_CommandPool&			commandPool(QueueTypeFlags queueType);
	Vk_DescriptorAllocator& descriptorAllocator();

	Vk_Semaphore*	imageAvaliableSmp();
	Vk_Semaphore*	renderCompletedSmp();
	Vk_Fence*		inFlightFence();

	SizeType		submitCount() const;

	RenderDevice_Vk*	renderDeviceVk();
	RenderContext_Vk*	rdCtxVk();

protected:
	void createCommandPool (Vector<Vk_CommandPool, s_kThreadCount>& cmdPool, QueueTypeFlags type);
	void destroyCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool);

	void createSyncObjects	();
	void destroySyncObjects	();

protected:
	void _setDebugName();

protected:
	RenderContext_Vk* _rdCtxVk = nullptr;

	Vector<Vk_CommandPool, s_kThreadCount> _graphicsCommandPools;
	Vector<Vk_CommandPool, s_kThreadCount> _computeCommandPools;
	Vector<Vk_CommandPool, s_kThreadCount> _transferCommandPools;

	// maybe put back on RenderContext_Vk
	Vk_Semaphore	_imageAvailableVkSmp;
	Vk_Semaphore	_renderCompletedVkSmp;
	Vk_Fence		_inFlightVkFence;

	Vk_DescriptorAllocator _descriptorAlloc;
	//Vector<Vk_DescriptorPool, s_kThreadCount> // use VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, only reset the pool

	SizeType _submitCount = 0;
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

inline Vk_DescriptorAllocator& Vk_RenderFrame::descriptorAllocator() { return _descriptorAlloc; }

inline Vk_Semaphore*			Vk_RenderFrame::imageAvaliableSmp()		{ return &_imageAvailableVkSmp; }
inline Vk_Semaphore*			Vk_RenderFrame::renderCompletedSmp()	{ return &_renderCompletedVkSmp; }
inline Vk_Fence*				Vk_RenderFrame::inFlightFence()			{ return &_inFlightVkFence; }

inline Vk_RenderFrame::SizeType	Vk_RenderFrame::submitCount() const		{ return _submitCount; }

inline RenderContext_Vk*		Vk_RenderFrame::rdCtxVk() { return _rdCtxVk; }

#endif


}
#endif