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

class RenderFrame_Vk : public NonCopyable
{
public:
	using Util = RenderApiUtil_Vk;
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kThreadCount = RenderApiLayerTraits::s_kThreadCount;

public:
	static RenderFrame_Vk* instance();

public:
	RenderFrame_Vk();
	~RenderFrame_Vk();

	VkPtr<Vk_CommandBuffer> requestCommandBuffer(QueueTypeFlags queueType, VkCommandBufferLevel bufLevel);

protected:


protected:
	//Vector<Vk_CommandPool, s_kThreadCount> _graphicsCommandPools;
	//Vector<Vk_CommandPool, s_kThreadCount> _computeCommandPools;
	//Vector<Vk_CommandPool, s_kThreadCount> _transferCommandPools;
};
#endif


}
#endif