#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsRenderApi_Common_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- Vk_CommandBuffer_T-Decl ---
#endif // 0
#if 1

class Vk_CommandPool;
class Vk_CommandBuffer : public RenderApiPrimitive_Vk<Vk_CommandBuffer_T>
{
public:
	Vk_CommandBuffer();
	~Vk_CommandBuffer();

	void create(Vk_CommandPool* vkCommandPool, VkCommandBufferLevel level);
	void destroy();

	void reset();

	void beginRecord(Vk_Queue* vkQueue, VkCommandBufferUsageFlags usageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	void endRecord();
	void submit(Vk_Fence* signalFence, Vk_Semaphore* waitVkSmp, VkPipelineStageFlags2 waitStage, Vk_Semaphore* signalVkSmp, VkPipelineStageFlags2 signalStage);
	void submit();
	void waitIdle();

	bool isPrimiary() const;
	bool isSecondary() const;

public:


public:
	void cmd_CopyBuffer(Vk_Buffer* dst, Vk_Buffer* src, VkDeviceSize size, VkDeviceSize dstOffset, VkDeviceSize srcOffset);

public:

protected:
	Vk_CommandPool*			_vkCommandPool	= nullptr;
	Vk_Queue*				_vkQueue		= nullptr;

	VkCommandBufferLevel	_level;
};

inline bool Vk_CommandBuffer::isPrimiary()	const { return _level == VK_COMMAND_BUFFER_LEVEL_PRIMARY; }
inline bool Vk_CommandBuffer::isSecondary()	const { return _level == VK_COMMAND_BUFFER_LEVEL_SECONDARY; }


#endif



}
#endif