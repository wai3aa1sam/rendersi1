#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

class RenderDevice_Vk;

struct Vk_Cmd_AddImageMemBarrierDesc;

#if 0
#pragma mark --- Vk_CommandBuffer_T-Decl ---
#endif // 0
#if 1

class Vk_CommandPool;
class Vk_Swapchain;

struct Vk_SmpSubmitInfo : public VkSemaphoreSubmitInfoKHR
{
	Vk_SmpSubmitInfo(Vk_Semaphore_T* vkSmp, VkPipelineStageFlags2 stage)
	{
		memory_set(this, 1, 0);
		sType		= VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		semaphore	= vkSmp;
		stageMask	= stage;
	}

	~Vk_SmpSubmitInfo()
	{
		RDS_CORE_ASSERT(sizeof(VkSemaphoreSubmitInfoKHR) == sizeof(*this), "");
	}
};

class Vk_CommandBuffer : public Vk_RenderApiPrimitive<Vk_CommandBuffer_T, VK_OBJECT_TYPE_COMMAND_BUFFER>
{
	using Base = Vk_RenderApiPrimitive<Vk_CommandBuffer_T, VK_OBJECT_TYPE_COMMAND_BUFFER>;
public:
	Vk_CommandBuffer();
	~Vk_CommandBuffer();

	void create	(Vk_CommandPool* vkCommandPool, VkCommandBufferLevel level, RenderDevice_Vk* rdDevVk);
	void destroy(RenderDevice_Vk* rdDevVk);

	void reset();

	void beginRecord(Vk_Queue* vkQueue, VkCommandBufferUsageFlags usageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, const VkCommandBufferInheritanceInfo* inheriInfo = nullptr);
	void beginSecondaryRecord(Vk_Queue* vkQueue, Vk_RenderPass* vkRenderPass, Vk_Framebuffer* vkFramebuffer
							, u32 subpassIdx = 0, VkCommandBufferUsageFlags usageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);
	void endRecord();

	void submit(Vk_Fence* signalFence, Vk_Semaphore* signalVkSmp, Vk_PipelineStageFlags signalStage);
	void submit(Vk_Fence* signalFence, Vk_Semaphore* waitVkSmp, Vk_PipelineStageFlags waitStage, Vk_Semaphore* signalVkSmp, Vk_PipelineStageFlags signalStage);
	void submit(Vk_Fence* signalFence, Span<Vk_SmpSubmitInfo> waitSmps, Span<Vk_SmpSubmitInfo> signalSmps);
	void submit();
	void executeSecondaryCmdBufs(Span<Vk_CommandBuffer*> cmdBufs);
	void waitIdle();

	VkResult swapBuffers(Vk_Queue* vkPresentQueue, Vk_Swapchain* vkSwpachain, u32 imageIdx, Vk_Semaphore* vkWaitSmp);

	void setViewport(const math::Rect2f& rect, float minDepth = 0.0f, float maxDepth = 1.0f);
	void setScissor (const math::Rect2f& rect);
	
	void beginRenderPass(Vk_RenderPass* vkRenderPass, Vk_Framebuffer* vkFramebuffer, const math::Rect2f& rect2, Span<VkClearValue> vkClearValues, VkSubpassContents subpassContents);
	void endRenderPass();


	bool isPrimiary() const;
	bool isSecondary() const;

	Vk_RenderPass* getRenderPass();

public:
	//RenderDevice_Vk*	renderDeviceVk();

public:
	void cmd_copyBuffer(Vk_Buffer*		dst, Vk_Buffer*		src, VkDeviceSize size, VkDeviceSize dstOffset, VkDeviceSize srcOffset);
	void cmd_copyBuffer(Vk_Buffer_T*	dst, Vk_Buffer_T*	src, VkDeviceSize size, VkDeviceSize dstOffset, VkDeviceSize srcOffset);

	void cmd_copyBufferToImage(Vk_Image_T* dst, Vk_Buffer_T* src, VkImageLayout layout, u32 width, u32 height);

	void cmd_addImageMemBarrier(Vk_Image*	image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout);

	void cmd_addImageMemBarrier(Vk_Image_T*	image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout);
	void cmd_addImageMemBarrier(Vk_Image_T*	image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout, u32 srcQueueFamilyIdx, u32 dstQueueFamilyIdx, bool isSrcQueueOwner);
	void cmd_addImageMemBarrier(Vk_Image_T* image, VkFormat vkFormat, VkImageLayout srcLayout, VkImageLayout dstLayout, u32 baseMip, u32 mipCount, u32 srcQueueFamilyIdx, u32 dstQueueFamilyIdx, bool isSrcQueueOwner);
	void cmd_addImageMemBarrier(const Vk_Cmd_AddImageMemBarrierDesc& desc);

public:

protected:
	Vk_CommandPool*			_vkCommandPool	= nullptr;
	Vk_Queue*				_vkQueue		= nullptr;
	Vk_RenderPass*			_vkRdPass		= nullptr;

	VkCommandBufferLevel	_level;
};

inline bool Vk_CommandBuffer::isPrimiary()	const { return _level == VK_COMMAND_BUFFER_LEVEL_PRIMARY; }
inline bool Vk_CommandBuffer::isSecondary()	const { return _level == VK_COMMAND_BUFFER_LEVEL_SECONDARY; }



inline Vk_RenderPass* Vk_CommandBuffer::getRenderPass() { return _vkRdPass; }

#endif

struct Vk_Cmd_AddImageMemBarrierDesc
{
	Vk_Image_T*		image		= nullptr;
	VkFormat		format;
	VkImageLayout	srcLayout; 
	VkImageLayout	dstLayout;
	u32				baseMip		= 0;
	u32				mipCount	= 1;

	u32				srcQueueFamilyIdx	= VK_QUEUE_FAMILY_IGNORED;
	u32				dstQueueFamilyIdx	= VK_QUEUE_FAMILY_IGNORED;
	bool			isSrcQueueOwner		= true;
	//QueueTypeFlags	srcQueueType;
	//QueueTypeFlags	dstQueueType;
};


}
#endif