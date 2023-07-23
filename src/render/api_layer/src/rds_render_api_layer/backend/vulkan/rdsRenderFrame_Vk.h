#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandPool.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandBuffer.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsRenderFrame_Vk-Decl ---
#endif // 0
#if 1

class RenderFrame_Vk : public NonCopyable	// not a derived class of RenderFrame
{
public:
	using Util = RenderApiUtil_Vk;
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kThreadCount		= RenderApiLayerTraits::s_kThreadCount;
	static constexpr SizeType s_kFrameInFlightCount	= RenderApiLayerTraits::s_kFrameInFlightCount;

public:
	RenderFrame_Vk();
	~RenderFrame_Vk();

	RenderFrame_Vk(RenderFrame_Vk&& rhs);
	void operator=(RenderFrame_Vk&& rhs);

	void create();
	void destroy();

	void reset();

	Vk_CommandBuffer* requestCommandBuffer(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel);

	void resetCommandPools();
	void resetCommandPool(QueueTypeFlags queueType);

	Vk_CommandPool& commandPool(QueueTypeFlags queueType);

protected:
	void createCommandPool (Vector<Vk_CommandPool, s_kThreadCount>& cmdPool, u32 queueIdx);
	void destroyCommandPool(Vector<Vk_CommandPool, s_kThreadCount>& cmdPool);

protected:
	Vector<Vk_CommandPool, s_kThreadCount> _graphicsCommandPools;
	Vector<Vk_CommandPool, s_kThreadCount> _computeCommandPools;
	Vector<Vk_CommandPool, s_kThreadCount> _transferCommandPools;
};

inline
Vk_CommandPool& 
RenderFrame_Vk::commandPool(QueueTypeFlags queueType)
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

#endif


}
#endif