#pragma once

#include "rds_render_api_layer/backend/vulkan/rdsVk_RenderApi_Common.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- Vk_CommandBuffer_T-Decl ---
#endif // 0
#if 1

class Vk_CommandPool;
class Vk_CommandBuffer : public Vk_RenderApiPrimitive<Vk_CommandBuffer_T>
{
public:
	Vk_CommandBuffer();
	~Vk_CommandBuffer();

	void create(Vk_CommandPool* vkCommandPool, VkCommandBufferLevel level);
	void destroy();

	void reset();

	void beginRecord(Vk_Queue* vkQueue, VkCommandBufferUsageFlags usageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, const VkCommandBufferInheritanceInfo* inheriInfo = nullptr);
	void beginSecondaryRecord(Vk_Queue* vkQueue, Vk_RenderPass* vkRenderPass, Vk_Framebuffer* vkFramebuffer
							, u32 subpassIdx = 0, VkCommandBufferUsageFlags usageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);
	void endRecord();

	void submit(Vk_Fence* signalFence, Vk_Semaphore* waitVkSmp, VkPipelineStageFlags2 waitStage, Vk_Semaphore* signalVkSmp, VkPipelineStageFlags2 signalStage);
	void submit();
	void waitIdle();

	void swapBuffers(Vk_Queue* vkPresentQueue, Vk_Swapchain* vkSwpachain, u32 imageIdx, Vk_Semaphore* vkWaitSmp);

	void setViewport(const math::Rect2f& rect, float minDepth = 0.0f, float maxDepth = 1.0f);
	void setScissor(const math::Rect2f& rect);
	
	void beginRenderPass(Vk_RenderPass* vkRenderPass, Vk_Framebuffer* vkFramebuffer, const math::Rect2f& rect2, Span<VkClearValue> vkClearValues);
	void endRenderPass();


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