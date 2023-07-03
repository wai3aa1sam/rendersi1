#pragma once

#include "rdsRenderApi_Common_Vk.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsRenderContext_Vk-Decl ---
#endif // 0
#if 1

class RenderContext_Vk : public RenderContext
{
public:
	using Base = RenderContext;

	using Util = RenderApiUtil_Vk;


public:
	static constexpr SizeType s_kSwapchainImageLocalSize = 4;

public:
	RenderContext_Vk();
	virtual ~RenderContext_Vk();

	Renderer_Vk* renderer();


protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onBeginRender() override;
	virtual void onEndRender()	 override;

protected:
	void createSwapchainInfo(SwapchainInfo_Vk& out, const SwapchainAvailableInfo_Vk& info, const Rect2f& windowRect2f);

	void createFramebuffers();

	void createCommandPool();
	void createCommandBuffer();
	void createSyncObjects();

	void createTestRenderPass();
	void createTestGraphicsPipeline();

	void beginRecord(Vk_CommandBuffer* vkCmdBuf, u32 imageIdx);
	void endRecord(Vk_CommandBuffer* vkCmdBuf);
	void bindPipeline(Vk_CommandBuffer* vkCmdBuf, Vk_Pipeline* vkPipeline);

	void testDrawCall(Vk_CommandBuffer* vkCmdBuf, u32 imageIdx);

protected:
	Renderer_Vk* _renderer = nullptr;

	SwapchainInfo_Vk	_swapchainInfo;

	VkPtr<Vk_Surface>			_vkSurface;
	VkPtr<Vk_Swapchain>			_vkSwapchain;
	SwapChainImages_Vk			_vkSwapchainImages;
	SwapChainImageViews_Vk		_vkSwapchainImageViews;
	SwapChainFramebuffers_Vk	_vkSwapchainFramebuffers;
	
	VkPtr<Vk_Pipeline>		_testVkPipeline;
	VkPtr<Vk_RenderPass>	_testVkRenderPass;
	VkPtr<Vk_PipelineLayout>_testVkPipelineLayout;

	VkPtr<Vk_CommandPool>	_vkCommandPool;
	VkPtr<Vk_CommandBuffer> _vkCommandBuffer;

	VkPtr<Vk_Queue>		_vkGraphicsQueue;
	VkPtr<Vk_Queue>		_vkPresentQueue;

	VkPtr<Vk_Semaphore>	_imageAvailableVkSmp;
	VkPtr<Vk_Semaphore>	_renderFinishedVkSmp;
	VkPtr<Vk_Fence>		_inFlightVkFence;

	u32 _currentImageIdx;
};

Renderer_Vk* RenderContext_Vk::renderer() { return _renderer; }


#endif
}

#endif // RDS_RENDER_HAS_VULKAN


